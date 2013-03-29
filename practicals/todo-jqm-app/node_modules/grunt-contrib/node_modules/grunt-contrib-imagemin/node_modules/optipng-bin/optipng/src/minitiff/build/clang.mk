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
LIBS =
RM_F = rm -f

MINITIFF_LIB = libminitiff.a
MINITIFF_OBJS = tiffbase.o tiffread.o #tiffwrite.o
TIFF2PNM = test/tiff2pnm$(EXEEXT)
TIFF2PNM_OBJS = test/tiff2pnm.o

all: $(MINITIFF_LIB) $(TIFF2PNM)

test: $(TIFF2PNM)
# TODO: run $(TIFF2PNM)

check: test

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(MINITIFF_LIB): $(MINITIFF_OBJS)
	$(AR) $(ARFLAGS) $@ $(MINITIFF_OBJS)
	$(RANLIB) $@

$(TIFF2PNM): $(TIFF2PNM_OBJS) $(MINITIFF_LIB)
	$(LD) $(LDFLAGS) -o $@ $(TIFF2PNM_OBJS) $(MINITIFF_LIB) $(LIBS)

tiffbase.o: tiffbase.c minitiff.h
tiffread.o: tiffread.c minitiff.h tiffdef.h
tiffwrite.o: tiffwrite.c

test/tiff2pnm.o: test/tiff2pnm.c minitiff.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -o $@ $*.c

clean:
	-$(RM_F) $(MINITIFF_LIB) $(MINITIFF_OBJS)
	-$(RM_F) $(TIFF2PNM) $(TIFF2PNM_OBJS)

distclean: clean
	-$(RM_F) Makefile
