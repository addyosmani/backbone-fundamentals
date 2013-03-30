# clang.mk
# Generated from Makefile.in
# Preconfigured for clang
#
# Usage: make -f build/clang.mk

.PHONY: all test check clean distclean
.PRECIOUS: Makefile
.SUFFIXES: .c .o .a

CC = clang
CFLAGS = -O2 -Wall -Wextra
CPP = $(CC) -E
CPPFLAGS =
LD = $(CC)
LDFLAGS = -s
AR = ar
ARFLAGS = cru
RANLIB = ranlib
RM_F = rm -f

ZDIR = ../zlib
PNGDIR = ../libpng
GIFDIR = ../gifread
PNMDIR = ../pnmio
TIFFDIR = ../minitiff

PNGXTERN_LIB = libpngxtern.a

PNGXREAD_OBJS = pngxread.o pngxrbmp.o pngxrgif.o pngxrjpg.o pngxrpnm.o pngxrtif.o
PNGXUTIL_OBJS = pngxio.o pngxmem.o pngxset.o
PNGXTERN_OBJS = $(PNGXREAD_OBJS) $(PNGXUTIL_OBJS)

PNGXTERN_DEPINCLUDE_ZLIB = -I$(ZDIR)
#PNGXTERN_DEPINCLUDE_ZLIB =
PNGXTERN_DEPINCLUDE_LIBPNG = -I$(PNGDIR)
#PNGXTERN_DEPINCLUDE_LIBPNG =
PNGXTERN_DEPINCLUDES = \
  $(PNGXTERN_DEPINCLUDE_ZLIB) \
  $(PNGXTERN_DEPINCLUDE_LIBPNG) \
  -I$(GIFDIR) \
  -I$(PNMDIR) \
  -I$(TIFFDIR)

all: $(PNGXTERN_LIB)

test:

check: test

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(PNGXTERN_DEPINCLUDES) -o $@ $<

$(PNGXTERN_LIB): $(PNGXTERN_OBJS)
	$(AR) $(ARFLAGS) $@ $(PNGXTERN_OBJS)
	$(RANLIB) $@

pngxio.o: pngxio.c pngxutil.h
pngxmem.o: pngxmem.c pngxutil.h
pngxset.o: pngxset.c pngxutil.h
pngxread.o: pngxread.c pngxtern.h pngxutil.h
pngxrbmp.o: pngxrbmp.c pngxtern.h pngxutil.h
pngxrgif.o: pngxrgif.c pngxtern.h pngxutil.h
pngxrjpg.o: pngxrjpg.c pngxtern.h pngxutil.h
pngxrpnm.o: pngxrpnm.c pngxtern.h pngxutil.h
pngxrtif.o: pngxrtif.c pngxtern.h pngxutil.h

clean:
	-$(RM_F) $(PNGXTERN_LIB) $(PNGXTERN_OBJS)

distclean: clean
	-$(RM_F) Makefile
