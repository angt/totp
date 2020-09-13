CC     = cc
CFLAGS = -Wall -O2
prefix = /usr/local

totp:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) totp.c -o totp

install: totp
	mkdir -p $(DESTDIR)$(prefix)/bin
	mv -f totp $(DESTDIR)$(prefix)/bin

uninstall:
	rm -f $(DESTDIR)$(prefix)/bin/totp

clean:
	rm -f totp

.PHONY: totp install uninstall clean
