# Makefile for Borland C++
# Usage: make -f build\bcc32.mk

all:
	cd src\optipng
	$(MAKE) -f build\bcc32.mk
	cd ..\..

test:
	cd src\optipng
	$(MAKE) -f build\bcc32.mk test
	cd ..\..

check: test

clean:
	cd src\optipng
	$(MAKE) -f build\bcc32.mk clean
	cd ..\..

distclean:
	cd src\optipng
	$(MAKE) -f build\bcc32.mk distclean
	cd ..\..
