# totp
A tiny command line utility to generate OTP tokens

## Build and install

Clone the repository:

    $ git clone https://github.com/angt/totp
    $ cd totp

Then, run as `root`:

    # make install

As usual, you can customize the destination with `DESTDIR` and `prefix`.

## Usage

    $ echo -n JBSWY3DPEHPK3PXP | base32 -d | totp
    $ 123456

Or

    $ echo JBSWY3DPEHPK3PXP | totp -b
    $ 123456

It has been thought for [secret](https://github.com/angt/secret),
maybe it will be directly integrated in a future version, in the meantime:

### Add a new TOTP key

    $ echo -n JBSWY3DPEHPK3PXP | base32 -d | secret set test/totp
    
### Generate a TOTP token

    $ secret show test/totp | totp
    $ 123456
    
---
For feature requests and bug reports,
please create an [issue](https://github.com/angt/totp/issues).
