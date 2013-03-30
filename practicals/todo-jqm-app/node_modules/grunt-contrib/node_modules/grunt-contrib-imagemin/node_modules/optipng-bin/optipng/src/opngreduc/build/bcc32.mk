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

OPNGREDUC_LIB = opngreduc.lib

OPNGREDUC_OBJS = opngreduc.obj
OPNGREDUC_LIBOBJS = +opngreduc.obj

OPNGREDUC_DEPINCLUDE_ZLIB = -I$(ZDIR)
OPNGREDUC_DEPINCLUDE_LIBPNG = -I$(PNGDIR)
OPNGREDUC_DEPINCLUDES = \
  $(OPNGREDUC_DEPINCLUDE_ZLIB) \
  $(OPNGREDUC_DEPINCLUDE_LIBPNG)

all: $(OPNGREDUC_LIB)

test:

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(OPNGREDUC_DEPINCLUDES) -o$@ $<

$(OPNGREDUC_LIB): $(OPNGREDUC_OBJS)
	$(AR) $(ARFLAGS) $@ $(OPNGREDUC_LIBOBJS)

opngreduc.obj: opngreduc.c opngreduc.h

clean:
	-$(RM_F) $(OPNGREDUC_LIB) $(OPNGREDUC_OBJS)

distclean: clean
	-$(RM_F) Makefile
