# Makefile for Microsoft Visual C++
# Usage: nmake -f build\visualc.mk

all:
	cd src\optipng
	$(MAKE) -f build\visualc.mk
	cd ..\..

test:
	cd src\optipng
	$(MAKE) -f build\visualc.mk test
	cd ..\..

check: test

clean:
	cd src\optipng
	$(MAKE) -f build\visualc.mk clean
	cd ..\..

distclean:
	cd src\optipng
	$(MAKE) -f build\visualc.mk distclean
	cd ..\..
