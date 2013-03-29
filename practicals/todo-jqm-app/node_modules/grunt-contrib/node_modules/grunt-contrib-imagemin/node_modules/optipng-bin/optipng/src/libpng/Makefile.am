# Makefile.am:
#   Source file for Makefile.in (and hence Makefile)
#

PNGLIB_BASENAME= libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@

# libpng does not follow GNU file name conventions
AUTOMAKE_OPTIONS = foreign

# test programs - run on make check, make distcheck
check_PROGRAMS= pngtest
pngtest_SOURCES = pngtest.c
pngtest_LDADD = libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@.la
TESTS = test-pngtest.sh
TESTS_ENVIRONMENT= srcdir=$(srcdir)

# man pages
dist_man_MANS= libpng.3 libpngpf.3 png.5

# generate the -config scripts if required
binconfigs= libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@-config
EXTRA_SCRIPTS= libpng-config libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@-config
bin_SCRIPTS= @binconfigs@

# rules to build libpng, only build the old library on request
lib_LTLIBRARIES=libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@.la
# EXTRA_LTLIBRARIES= libpng.la
libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_SOURCES = png.c pngset.c pngget.c pngrutil.c \
	pngtrans.c pngwutil.c \
	pngread.c pngrio.c pngwio.c pngwrite.c pngrtran.c \
	pngwtran.c pngmem.c pngerror.c pngpread.c \
	png.h pngconf.h pngpriv.h

libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_CPPFLAGS = @LIBPNG_DEFINES@

libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_LDFLAGS = -no-undefined -export-dynamic \
	-version-number @PNGLIB_MAJOR@@PNGLIB_MINOR@:@PNGLIB_RELEASE@:0

if HAVE_LD_VERSION_SCRIPT
  # Versioned symbols and restricted exports

if HAVE_SOLARIS_LD
  libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_LDFLAGS += -Wl,-M -Wl,libpng.vers
else
  libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_LDFLAGS += -Wl,--version-script=libpng.vers
endif

  libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_DEPENDENCIES = libpng.vers
else
  # Only restricted exports when possible
  libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_LDFLAGS += -export-symbols libpng.sym
  libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@_la_DEPENDENCIES = libpng.sym
endif

# Avoid depending upon Character Ranges.
AN = '_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'

#distribute headers in /usr/include/libpng/*
pkgincludedir= $(includedir)/$(PNGLIB_BASENAME)
pkginclude_HEADERS= png.h pngconf.h

# pkg-config stuff, note that libpng.pc is always required in order
# to get the correct library
pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@.pc

#extra source distribution files.
EXTRA_DIST= \
	ANNOUNCE CHANGES INSTALL LICENSE README TODO \
	pngtest.png pngbar.png pngnow.png pngbar.jpg autogen.sh \
	${srcdir}/projects/visualc6/* \
	${srcdir}/projects/visualc71/* \
	${srcdir}/projects/vstudio/* \
	${srcdir}/projects/xcode/* \
	${srcdir}/scripts/* \
	${srcdir}/contrib/gregbook/* \
	${srcdir}/contrib/pngminim/* \
	${srcdir}/contrib/pngminus/* \
	${srcdir}/contrib/pngsuite/* \
	${srcdir}/contrib/visupng/* \
	$(TESTS) \
	example.c libpng-1.4.12.txt

CLEANFILES= pngout.png libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@.pc libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@-config libpng.vers \
libpng.sym

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.sub configure depcomp install-sh ltmain.sh missing

$(PNGLIB_BASENAME).pc: libpng.pc
	cp libpng.pc $@

$(PNGLIB_BASENAME)-config: libpng-config
	cp libpng-config $@

libpng.sym: png.h pngconf.h
	rm -f $@ $@.new
	$(CPP) @LIBPNG_DEFINES@ $(CPPFLAGS) -DPNG_BUILDSYMS $(srcdir)/png.h | \
		$(SED) -n -e \
		's|^.*PNG_FUNCTION_EXPORT[ 	]*\([$(AN)]*\).*$$|$(SYMBOL_PREFIX)\1|p' \
		-e 's|^.*PNG_DATA_EXPORT[ 	]*\([$(AN)]*\).*$$|$(SYMBOL_PREFIX)\1|p' \
			>$@.new
	mv $@.new $@

libpng.vers: libpng.sym
	rm -f $@ $@.new
	echo PNG@PNGLIB_MAJOR@@PNGLIB_MINOR@_0 '{global:' > $@.new
	$(SED) s/$$/\;/ libpng.sym >> $@.new
	echo 'local: *; };' >> $@.new
	mv $@.new $@

test: check

# install the .../include headers as links to the new ones
install-data-hook:
	cd $(DESTDIR)$(includedir); rm -f png.h pngconf.h
	cd $(DESTDIR)$(includedir); $(LN_S) $(PNGLIB_BASENAME)/png.h png.h
	cd $(DESTDIR)$(includedir); $(LN_S) $(PNGLIB_BASENAME)/pngconf.h \
		pngconf.h
	cd $(DESTDIR)$(pkgconfigdir); rm -f libpng.pc
	cd $(DESTDIR)$(pkgconfigdir); $(LN_S) $(PNGLIB_BASENAME).pc libpng.pc

# do evil things to libpng to cause libpng@PNGLIB_MAJOR@@PNGLIB_MINOR@ to be used
install-exec-hook:
	cd $(DESTDIR)$(bindir); rm -f libpng-config
	cd $(DESTDIR)$(bindir); $(LN_S) $(PNGLIB_BASENAME)-config libpng-config
	@set -x;\
	cd $(DESTDIR)$(libdir);\
	for ext in a la so so.@PNGLIB_MAJOR@@PNGLIB_MINOR@.@PNGLIB_RELEASE@ sl dylib dll.a; do\
		rm -f libpng.$$ext;\
                if test -f $(PNGLIB_BASENAME).$$ext; then\
                       $(LN_S) $(PNGLIB_BASENAME).$$ext libpng.$$ext;\
                fi;\
	done

uninstall-hook:
	cd $(DESTDIR)$(includedir); rm -f png.h pngconf.h
	rm -f $(DESTDIR)$(pkgconfigdir)/libpng.pc
	rm -f $(DESTDIR)$(bindir)/libpng-config
