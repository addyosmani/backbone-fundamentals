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
LIBS = #noeh32.lib
RM_F = del /q

MINITIFF_LIB = minitiff.lib
MINITIFF_OBJS = tiffbase.obj tiffread.obj #tiffwrite.obj
MINITIFF_LIBOBJS = +tiffbase.obj +tiffread.obj #+tiffwrite.obj
TIFF2PNM = test\tiff2pnm.exe
TIFF2PNM_OBJS = test\tiff2pnm.obj
TIFF2PNM_LIBOBJS = test\+tiff2pnm.obj

all: $(MINITIFF_LIB) $(TIFF2PNM)

test: $(TIFF2PNM)
# TODO: run $(TIFF2PNM)

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o$@ $<

$(MINITIFF_LIB): $(MINITIFF_OBJS)
	$(AR) $(ARFLAGS) $@ $(MINITIFF_LIBOBJS)

$(TIFF2PNM): $(TIFF2PNM_OBJS) $(MINITIFF_LIB)
	$(LD) $(LDFLAGS) -e$@ $(TIFF2PNM_OBJS) $(MINITIFF_LIB) $(LIBS)

tiffbase.obj: tiffbase.c minitiff.h
tiffread.obj: tiffread.c minitiff.h tiffdef.h
tiffwrite.obj: tiffwrite.c

test\tiff2pnm.obj: test\tiff2pnm.c minitiff.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -o$@ $*.c

clean:
	-$(RM_F) $(MINITIFF_LIB) $(MINITIFF_OBJS)
	-$(RM_F) $(TIFF2PNM) $(TIFF2PNM_OBJS)

distclean: clean
	-$(RM_F) Makefile
