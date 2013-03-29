all:
	cd src/optipng && \
	$(MAKE) && \
	cd ../..

test:
	cd src/optipng && \
	$(MAKE) test && \
	cd ../..

check: test

install:
	cd src/optipng && \
	$(MAKE) install && \
	cd ../..

uninstall:
	cd src/optipng && \
	$(MAKE) uninstall && \
	cd ../..

clean:
	cd src/optipng && \
	$(MAKE) clean && \
	cd ../..

distclean:
	cd src/optipng && \
	$(MAKE) distclean && \
	cd ../..
	-rm -f src/Makefile Makefile
