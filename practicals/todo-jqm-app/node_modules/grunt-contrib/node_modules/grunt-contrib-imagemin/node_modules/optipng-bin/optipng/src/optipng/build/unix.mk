# unix.mk
# Generated from Makefile.in
# Preconfigured for Unix (generic)
#
# Usage: make -f build/unix.mk

.PHONY: all test check clean distclean install uninstall
.PRECIOUS: Makefile
.SUFFIXES: .c .o .a

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
mandir = $(prefix)/man
man1dir = $(mandir)/man1

CC = cc
CFLAGS = -O
CPP = $(CC) -E
CPPFLAGS =
LD = $(CC)
LDFLAGS = -s
DIFF = diff -b -u
RM_F = rm -f

LIB_LIBPNG =
#LIB_LIBPNG = -lpng
LIB_ZLIB =
#LIB_ZLIB = -lz
LIBM = -lm
LIBS =
ALL_LIBS = $(LIB_LIBPNG) $(LIB_ZLIB) $(LIBM) $(LIBS)

OPTIPNG_DIR = ../optipng
CEXCEPT_DIR = ../cexcept
OPNGREDUC_DIR = ../opngreduc
OPNGREDUC_LIB = libopngreduc.a
OPNGREDUC_MK = build/unix.mk
PNGXTERN_DIR = ../pngxtern
PNGXTERN_LIB = libpngxtern.a
PNGXTERN_MK = build/unix.mk
LIBPNG_DIR = ../libpng
LIBPNG_LIB = libpng.a
#LIBPNG_LIB = -lpng
LIBPNG_MK = scripts/makefile.std
ZLIB_DIR = ../zlib
ZLIB_LIB = libz.a
#ZLIB_LIB = -lz
ZLIB_MK = Makefile
GIF_DIR = ../gifread
GIF_LIB = libgifread.a
GIF_MK = build/unix.mk
PNM_DIR = ../pnmio
PNM_LIB = libpnmio.a
PNM_MK = build/unix.mk
TIFF_DIR = ../minitiff
TIFF_LIB = libminitiff.a
TIFF_MK = build/unix.mk

OPTIPNG_OBJS = \
  optipng.o \
  optim.o \
  cbitset.o \
  osys.o \
  wildargs.o

OPTIPNG_DEPLIB_ZLIB = $(ZLIB_DIR)/$(ZLIB_LIB)
#OPTIPNG_DEPLIB_ZLIB =
OPTIPNG_DEPLIB_LIBPNG = $(LIBPNG_DIR)/$(LIBPNG_LIB)
#OPTIPNG_DEPLIB_ZLIB =
OPTIPNG_DEPLIBS = \
  $(OPNGREDUC_DIR)/$(OPNGREDUC_LIB) \
  $(PNGXTERN_DIR)/$(PNGXTERN_LIB) \
  $(OPTIPNG_DEPLIB_LIBPNG) \
  $(OPTIPNG_DEPLIB_ZLIB) \
  $(GIF_DIR)/$(GIF_LIB) \
  $(PNM_DIR)/$(PNM_LIB) \
  $(TIFF_DIR)/$(TIFF_LIB)

OPTIPNG_DEPINCLUDE_ZLIB = -I$(ZLIB_DIR)
#OPTIPNG_DEPINCLUDE_ZLIB =
OPTIPNG_DEPINCLUDE_LIBPNG = -I$(LIBPNG_DIR)
#OPTIPNG_DEPINCLUDE_LIBPNG =
OPTIPNG_DEPINCLUDES = \
  -I$(CEXCEPT_DIR) \
  $(OPTIPNG_DEPINCLUDE_ZLIB) \
  $(OPTIPNG_DEPINCLUDE_LIBPNG) \
  -I$(OPNGREDUC_DIR) \
  -I$(PNGXTERN_DIR)

OPTIPNG_TESTS = \
  test/cbitset_test$(EXEEXT) \
  test/print_ratio_test$(EXEEXT)
OPTIPNG_TESTOBJS = \
  test/cbitset_test.o \
  test/print_ratio_test.o \
  test/sprint_ratio.generated.o
OPTIPNG_TESTOUT = *.out.png test/*.out

all: optipng$(EXEEXT)

optipng$(EXEEXT): $(OPTIPNG_OBJS) $(OPTIPNG_DEPLIBS)
	$(LD) $(LDFLAGS) -o $@ $(OPTIPNG_OBJS) $(OPTIPNG_DEPLIBS) $(ALL_LIBS)

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(OPTIPNG_DEPINCLUDES) -o $@ $<

optipng.o: optipng.c optipng.h cbitset.h osys.h proginfo.h
optim.o: optim.c optipng.h cbitset.h osys.h
cbitset.o: cbitset.c cbitset.h
osys.o: osys.c osys.h
wildargs.o: wildargs.c

$(OPNGREDUC_DIR)/$(OPNGREDUC_LIB):
	cd $(OPNGREDUC_DIR) && \
	$(MAKE) -f $(OPNGREDUC_MK) $(OPNGREDUC_LIB) && \
	cd $(OPTIPNG_DIR)

$(PNGXTERN_DIR)/$(PNGXTERN_LIB): \
  $(OPTIPNG_DEPLIB_LIBPNG) \
  $(GIF_DIR)/$(GIF_LIB) \
  $(PNM_DIR)/$(PNM_LIB) \
  $(TIFF_DIR)/$(TIFF_LIB)
	cd $(PNGXTERN_DIR) && \
	$(MAKE) -f $(PNGXTERN_MK) $(PNGXTERN_LIB) && \
	cd $(OPTIPNG_DIR)

$(LIBPNG_DIR)/$(LIBPNG_LIB): \
  $(OPTIPNG_DEPLIB_ZLIB)
	cd $(LIBPNG_DIR) && \
	$(MAKE) -f $(LIBPNG_MK) $(LIBPNG_LIB) && \
	cd $(OPTIPNG_DIR)

$(ZLIB_DIR)/$(ZLIB_LIB):
	cd $(ZLIB_DIR) && \
	$(MAKE) -f $(ZLIB_MK) $(ZLIB_LIB) && \
	cd $(OPTIPNG_DIR)

$(GIF_DIR)/$(GIF_LIB):
	cd $(GIF_DIR) && \
	$(MAKE) -f $(GIF_MK) $(GIF_LIB) && \
	cd $(OPTIPNG_DIR)

$(PNM_DIR)/$(PNM_LIB):
	cd $(PNM_DIR) && \
	$(MAKE) -f $(PNM_MK) $(PNM_LIB) && \
	cd $(OPTIPNG_DIR)

$(TIFF_DIR)/$(TIFF_LIB):
	cd $(TIFF_DIR) && \
	$(MAKE) -f $(TIFF_MK) $(TIFF_LIB) && \
	cd $(OPTIPNG_DIR)

test: local-test test-libpng test-gifread test-minitiff

.PHONY: local-test
local-test: optipng$(EXEEXT) $(OPTIPNG_TESTS)
	-@$(RM_F) pngtest.out.png
	./optipng$(EXEEXT) -o1 -q img/pngtest.png -out=pngtest.out.png
	-@echo optipng ... ok
	test/cbitset_test$(EXEEXT) < test/cbitset_test.dat > test/cbitset_test.out
	diff -b -u test/cbitset_test.expect test/cbitset_test.out
	-@echo cbitset_test ... ok
	test/print_ratio_test$(EXEEXT) > test/print_ratio_test.out
	-@echo print_ratio_test ... ok

test/cbitset_test$(EXEEXT): test/cbitset_test.o cbitset.o
	$(LD) $(LDFLAGS) -o $@ \
	  test/cbitset_test.o cbitset.o $(LIBS)

test/print_ratio_test$(EXEEXT): \
  test/print_ratio_test.o test/sprint_ratio.generated.o
	$(LD) $(LDFLAGS) -o $@ \
	  test/print_ratio_test.o test/sprint_ratio.generated.o

test/cbitset_test.o: test/cbitset_test.c cbitset.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -o $@ $*.c

test/print_ratio_test.o: test/print_ratio_test.c test/print_ratio.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $*.c

test/sprint_ratio.generated.o: \
  test/sprint_ratio.generated.c test/print_ratio.h osys.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $*.c

#test/sprint_ratio.generated.c: test/extract_print_ratio.sh optim.c
#	$(SHELL) -c test/extract_print_ratio.sh

.PHONY: test-libpng
test-libpng: test-zlib
	cd $(LIBPNG_DIR) && \
	$(MAKE) -f $(LIBPNG_MK) test && \
	cd $(OPTIPNG_DIR)

# FIXME:
# Can't test zlib if NO_GZCOMPRES and NO_GZIP are enabled.
.PHONY: test-zlib
test-zlib:
#	cd $(ZLIB_DIR) && \
#	$(MAKE) -f $(ZLIB_MK) test && \
#	cd $(OPTIPNG_DIR)

.PHONY: test-gifread
test-gifread:
	cd $(GIF_DIR) && \
	$(MAKE) -f $(GIF_MK) test && \
	cd $(OPTIPNG_DIR)

.PHONY: test-minitiff
test-minitiff:
	cd $(TIFF_DIR) && \
	$(MAKE) -f $(TIFF_MK) test && \
	cd $(OPTIPNG_DIR)

check: test

clean: \
  local-clean \
  clean-opngreduc \
  clean-pngxtern-gif-pnm-tiff \
  clean-libpng \
  clean-zlib

.PHONY: clean-opngreduc
clean-opngreduc:
	cd $(OPNGREDUC_DIR) && \
	$(MAKE) -f $(OPNGREDUC_MK) clean && \
	cd $(OPTIPNG_DIR)

.PHONY: clean-pngxtern-gif-pnm-tiff
clean-pngxtern-gif-pnm-tiff:
	cd $(PNGXTERN_DIR) && \
	$(MAKE) -f $(PNGXTERN_MK) clean && \
	cd $(OPTIPNG_DIR)
	cd $(GIF_DIR) && \
	$(MAKE) -f $(GIF_MK) clean && \
	cd $(OPTIPNG_DIR)
	cd $(PNM_DIR) && \
	$(MAKE) -f $(PNM_MK) clean && \
	cd $(OPTIPNG_DIR)
	cd $(TIFF_DIR) && \
	$(MAKE) -f $(TIFF_MK) clean && \
	cd $(OPTIPNG_DIR)

.PHONY: clean-libpng
clean-libpng:
	cd $(LIBPNG_DIR) && \
	$(MAKE) -f $(LIBPNG_MK) clean && \
	cd $(OPTIPNG_DIR)

.PHONY: clean-zlib
clean-zlib:
	cd $(ZLIB_DIR) && \
	$(MAKE) -f $(ZLIB_MK) clean && \
	cd $(OPTIPNG_DIR)

distclean: \
  local-clean \
  distclean-opngreduc \
  distclean-pngxtern-gif-pnm-tiff \
  distclean-libpng \
  distclean-zlib
	-$(RM_F) Makefile man/Makefile

.PHONY: distclean-opngreduc
distclean-opngreduc:
	cd $(OPNGREDUC_DIR) && \
	$(MAKE) -f $(OPNGREDUC_MK) distclean && \
	cd $(OPTIPNG_DIR)

.PHONY: distclean-pngxtern-gif-pnm-tiff
distclean-pngxtern-gif-pnm-tiff:
	cd $(PNGXTERN_DIR) && \
	$(MAKE) -f $(PNGXTERN_MK) distclean && \
	cd $(OPTIPNG_DIR)
	cd $(GIF_DIR) && \
	$(MAKE) -f $(GIF_MK) distclean && \
	cd $(OPTIPNG_DIR)
	cd $(PNM_DIR) && \
	$(MAKE) -f $(PNM_MK) distclean && \
	cd $(OPTIPNG_DIR)
	cd $(TIFF_DIR) && \
	$(MAKE) -f $(TIFF_MK) distclean && \
	cd $(OPTIPNG_DIR)

.PHONY: distclean-libpng
distclean-libpng:
	cd $(LIBPNG_DIR) && \
	$(MAKE) -f $(LIBPNG_MK) clean && \
	cd $(OPTIPNG_DIR)

.PHONY: distclean-zlib
distclean-zlib:
	cd $(ZLIB_DIR) && \
	$(MAKE) -f $(ZLIB_MK) distclean && \
	cd $(OPTIPNG_DIR)

.PHONY: local-clean
local-clean:
	-$(RM_F) optipng$(EXEEXT) $(OPTIPNG_OBJS)
	-$(RM_F) $(OPTIPNG_TESTS) $(OPTIPNG_TESTOBJS) $(OPTIPNG_TESTOUT)

install: optipng$(EXEEXT)
	mkdir -p $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(man1dir)
	-@$(RM_F) $(DESTDIR)$(bindir)/optipng$(EXEEXT)
	-@$(RM_F) $(DESTDIR)$(man1dir)/optipng.1
	cp -f -p optipng$(EXEEXT) $(DESTDIR)$(bindir)
	cp -f -p man/optipng.1 $(DESTDIR)$(man1dir)

uninstall:
	-$(RM_F) $(DESTDIR)$(bindir)/optipng$(EXEEXT)
	-$(RM_F) $(DESTDIR)$(man1dir)/optipng.1
