# visualc.mk
# Generated from Makefile.in
# Preconfigured for Microsoft Visual C++
#
# Usage: nmake -f build\visualc.mk

CC = cl -nologo
CFLAGS = -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -MD -O2 -W4
CPP = cl -nologo -E
CPPFLAGS =
LD = link -nologo
LDFLAGS =
AR = lib -nologo
ARFLAGS =
RM_F = del /q

ZDIR = ..\zlib
PNGDIR = ..\libpng
GIFDIR = ..\gifread
PNMDIR = ..\pnmio
TIFFDIR = ..\minitiff

PNGXTERN_LIB = pngxtern.lib

PNGXREAD_OBJS = pngxread.obj pngxrbmp.obj pngxrgif.obj pngxrjpg.obj pngxrpnm.obj pngxrtif.obj
PNGXUTIL_OBJS = pngxio.obj pngxmem.obj pngxset.obj
PNGXTERN_OBJS = $(PNGXREAD_OBJS) $(PNGXUTIL_OBJS)

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
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(PNGXTERN_DEPINCLUDES) -Fo$@ $<

$(PNGXTERN_LIB): $(PNGXTERN_OBJS)
	$(AR) $(ARFLAGS) -out:$@ $(PNGXTERN_OBJS)

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
