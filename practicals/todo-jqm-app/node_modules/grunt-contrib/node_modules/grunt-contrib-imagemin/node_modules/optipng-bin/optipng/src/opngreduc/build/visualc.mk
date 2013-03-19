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

OPNGREDUC_LIB = opngreduc.lib

OPNGREDUC_OBJS = opngreduc.obj

OPNGREDUC_DEPINCLUDE_ZLIB = -I$(ZDIR)
OPNGREDUC_DEPINCLUDE_LIBPNG = -I$(PNGDIR)
OPNGREDUC_DEPINCLUDES = \
  $(OPNGREDUC_DEPINCLUDE_ZLIB) \
  $(OPNGREDUC_DEPINCLUDE_LIBPNG)

all: $(OPNGREDUC_LIB)

test:

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(OPNGREDUC_DEPINCLUDES) -Fo$@ $<

$(OPNGREDUC_LIB): $(OPNGREDUC_OBJS)
	$(AR) $(ARFLAGS) -out:$@ $(OPNGREDUC_OBJS)

opngreduc.obj: opngreduc.c opngreduc.h

clean:
	-$(RM_F) $(OPNGREDUC_LIB) $(OPNGREDUC_OBJS)

distclean: clean
	-$(RM_F) Makefile
