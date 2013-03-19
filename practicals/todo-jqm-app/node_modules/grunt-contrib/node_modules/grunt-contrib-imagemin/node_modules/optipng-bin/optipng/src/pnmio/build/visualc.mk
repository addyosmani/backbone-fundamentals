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

PNMIO_LIB = pnmio.lib
PNMIO_OBJS = pnmin.obj pnmout.obj pnmutil.obj

all: $(PNMIO_LIB)

test:

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -Fo$@ $<

$(PNMIO_LIB): $(PNMIO_OBJS)
	$(AR) $(ARFLAGS) -out:$@ $(PNMIO_OBJS)

pnmin.obj: pnmin.c pnmio.h
pnmout.obj: pnmout.c pnmio.h
pnmutil.obj: pnmutil.c pnmio.h

clean:
	-$(RM_F) $(PNMIO_LIB) $(PNMIO_OBJS)

distclean: clean
	-$(RM_F) Makefile
