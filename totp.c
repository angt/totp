// Copyright (c) 2020 by angt <adrien@gallouet.fr>
// Don't try to reuse the HMAC-SHA1 implementation of this project,
// you will suffer unproductively :P

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static inline void
ebe32(uint8_t *dst, uint32_t x)
{
    dst[0] = (uint8_t)(x >> 24);
    dst[1] = (uint8_t)(x >> 16);
    dst[2] = (uint8_t)(x >>  8);
    dst[3] = (uint8_t)(x      );
}

static inline uint32_t
dbe32(const uint8_t *src)
{
    return ((uint32_t)src[0] << 24)
         | ((uint32_t)src[1] << 16)
         | ((uint32_t)src[2] <<  8)
         | ((uint32_t)src[3]      );
}

static inline void
ebe64(uint8_t *dst, uint64_t x)
{
    ebe32(dst,     (uint32_t)(x >> 32));
    ebe32(dst + 4, (uint32_t)(x      ));
}

static inline uint32_t
rotl(int n, uint32_t x)
{
    return (x << n) | (x >> (32 - n));
}

static void
sha1_process(const uint8_t *buf, uint32_t x[5])
{
    uint32_t w[80];
    uint32_t a = x[0], b = x[1], c = x[2], d = x[3], e = x[4];

    for (int i = 0; i < 16; i++)
        w[i] = dbe32(&buf[i << 2]);

    for (int i = 16; i < 80; i++)
        w[i] = rotl(1, w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);

    for (int i = 0; i < 80; i++) {
        uint32_t t = rotl(5, a) + e + w[i];
             if (i < 20) t += 0x5A827999 + ((b & c) | ((~b) & d));
        else if (i < 40) t += 0x6ED9EBA1 + (b ^ c ^ d);
        else if (i < 60) t += 0x8F1BBCDC + ((b & c) | (b & d) | (c & d));
        else             t += 0xCA62C1D6 + (b ^ c ^ d);
        e = d; d = c; c = rotl(30, b); b = a; a = t;
    }
    x[0] += a; x[1] += b; x[2] += c; x[3] += d; x[4] += e;
}

static void
sha1(uint8_t *digest, uint8_t *buf, size_t len)
{
    uint8_t tmp[64] = {0};
    uint32_t x[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    size_t p = 0;

    for (; p + 64 <= len; p += 64)
        sha1_process(buf + p, x);

    if (len > p)
        memcpy(tmp, buf + p, len - p);

    p = len - p;
    tmp[p++] = 0x80;

    if (p > 56) {
        sha1_process(tmp, x);
        memset(tmp, 0, sizeof(tmp));
    }
    ebe64(tmp + 56, len << 3);
    sha1_process(tmp, x);

    for (int i = 0; i < 5; i++)
        ebe32(&digest[i << 2], x[i]);
}

static void
erase(void *buf, size_t len)
{
    volatile uint8_t *volatile x = (volatile uint8_t *volatile)buf;

    for (size_t i = 0; i < len; i++)
        x[i] = 0;
}

int
main(int argc, char **argv)
{
    uint8_t h[20];
    uint8_t ki[64 +  8] = {0};
    uint8_t ko[64 + 20] = {0};
    ssize_t len = read(0, ki, 64);

    if (len <= 0)
        return -1;

    memcpy(ko, ki, len);

    for (int i = 0; i < 64; i++) {
        ki[i] ^= 0x36;
        ko[i] ^= 0x5c;
    }
    ebe64(&ki[64], ((uint64_t)time(NULL)) / 30);
    sha1(&ko[64], ki, sizeof(ki));
    sha1(h, ko, sizeof(ko));

    erase(ki, sizeof(ki));
    erase(ko, sizeof(ko));

    uint32_t ret = (dbe32(&h[h[19] & 0xF]) & ~(UINT32_C(1) << 31))
                 % UINT32_C(1000000);

    printf("%06" PRIu32 "\n", ret);
}
