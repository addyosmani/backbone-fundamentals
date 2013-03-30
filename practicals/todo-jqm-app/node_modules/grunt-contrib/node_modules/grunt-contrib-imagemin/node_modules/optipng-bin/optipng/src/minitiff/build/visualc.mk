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
LIBS =
RM_F = del /q

MINITIFF_LIB = minitiff.lib
MINITIFF_OBJS = tiffbase.obj tiffread.obj #tiffwrite.obj
TIFF2PNM = test\tiff2pnm.exe
TIFF2PNM_OBJS = test\tiff2pnm.obj

all: $(MINITIFF_LIB) $(TIFF2PNM)

test: $(TIFF2PNM)
# TODO: run $(TIFF2PNM)

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -Fo$@ $<

$(MINITIFF_LIB): $(MINITIFF_OBJS)
	$(AR) $(ARFLAGS) -out:$@ $(MINITIFF_OBJS)

$(TIFF2PNM): $(TIFF2PNM_OBJS) $(MINITIFF_LIB)
	$(LD) $(LDFLAGS) -out:$@ $(TIFF2PNM_OBJS) $(MINITIFF_LIB) $(LIBS)

tiffbase.obj: tiffbase.c minitiff.h
tiffread.obj: tiffread.c minitiff.h tiffdef.h
tiffwrite.obj: tiffwrite.c

test\tiff2pnm.obj: test\tiff2pnm.c minitiff.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -Fo$@ $*.c

clean:
	-$(RM_F) $(MINITIFF_LIB) $(MINITIFF_OBJS)
	-$(RM_F) $(TIFF2PNM) $(TIFF2PNM_OBJS)

distclean: clean
	-$(RM_F) Makefile
