# bcc32.mk
# Generated from Makefile.in
# Preconfigured for Borland C++
#
# Usage: make -f build\bcc32.mk

CC = bcc32
CFLAGS = -O2 -d -k- -w
CPP = #cpp32 -P-
CPPFLAGS =
LD = $(CC)
LDFLAGS = $(CFLAGS)
AR = tlib
ARFLAGS = /C
RM_F = del /q

ZDIR = ..\zlib
PNGDIR = ..\libpng
GIFDIR = ..\gifread
PNMDIR = ..\pnmio
TIFFDIR = ..\minitiff

PNGXTERN_LIB = pngxtern.lib

PNGXREAD_OBJS = pngxread.obj pngxrbmp.obj pngxrgif.obj pngxrjpg.obj pngxrpnm.obj pngxrtif.obj
PNGXREAD_LIBOBJS = +pngxread.obj +pngxrbmp.obj +pngxrgif.obj +pngxrjpg.obj +pngxrpnm.obj +pngxrtif.obj
PNGXUTIL_OBJS = pngxio.obj pngxmem.obj pngxset.obj
PNGXUTIL_LIBOBJS = +pngxio.obj +pngxmem.obj +pngxset.obj
PNGXTERN_OBJS = $(PNGXREAD_OBJS) $(PNGXUTIL_OBJS)
PNGXTERN_LIBOBJS = $(PNGXREAD_LIBOBJS) $(PNGXUTIL_LIBOBJS)

PNGXTERN_DEPINCLUDE_ZLIB = -I$(ZDIR)
PNGXTERN_DEPINCLUDE_LIBPNG = -I$(PNGDIR)
PNGXTERN_DEPINCLUDES = \
  $(PNGXTERN_DEPINCLUDE_ZLIB) \
  $(PNGXTERN_DEPINCLUDE_LIBPNG) \
  -I$(GIFDIR) \
  -I$(PNMDIR) \
  -I$(TIFFDIR)

all: $(PNGXTERN_LIB)

test:

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(PNGXTERN_DEPINCLUDES) -o$@ $<

$(PNGXTERN_LIB): $(PNGXTERN_OBJS)
	$(AR) $(ARFLAGS) $@ $(PNGXTERN_LIBOBJS)

pngxio.obj: pngxio.c pngxutil.h
pngxmem.obj: pngxmem.c pngxutil.h
pngxset.obj: pngxset.c pngxutil.h
pngxread.obj: pngxread.c pngxtern.h pngxutil.h
pngxrbmp.obj: pngxrbmp.c pngxtern.h pngxutil.h
pngxrgif.obj: pngxrgif.c pngxtern.h pngxutil.h
pngxrjpg.obj: pngxrjpg.c pngxtern.h pngxutil.h
pngxrpnm.obj: pngxrpnm.c pngxtern.h pngxutil.h
pngxrtif.obj: pngxrtif.c pngxtern.h pngxutil.h

clean:
	-$(RM_F) $(PNGXTERN_LIB) $(PNGXTERN_OBJS)

distclean: clean
	-$(RM_F) Makefile
