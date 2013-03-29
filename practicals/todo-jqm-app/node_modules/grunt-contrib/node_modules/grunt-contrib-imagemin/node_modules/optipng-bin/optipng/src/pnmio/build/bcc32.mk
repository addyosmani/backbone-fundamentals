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

PNMIO_LIB = pnmio.lib
PNMIO_OBJS = pnmin.obj pnmout.obj pnmutil.obj
PNMIO_LIBOBJS = +pnmin.obj +pnmout.obj +pnmutil.obj

all: $(PNMIO_LIB)

test:

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o$@ $<

$(PNMIO_LIB): $(PNMIO_OBJS)
	$(AR) $(ARFLAGS) $@ $(PNMIO_LIBOBJS)

pnmin.obj: pnmin.c pnmio.h
pnmout.obj: pnmout.c pnmio.h
pnmutil.obj: pnmutil.c pnmio.h

clean:
	-$(RM_F) $(PNMIO_LIB) $(PNMIO_OBJS)

distclean: clean
	-$(RM_F) Makefile
