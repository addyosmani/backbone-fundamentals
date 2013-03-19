# gcc.mk
# Generated from Makefile.in
# Preconfigured for gcc
#
# Usage: make -f build/gcc.mk

.PHONY: all test check clean distclean
.PRECIOUS: Makefile
.SUFFIXES: .c .o .a

CC = gcc
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

OPNGREDUC_LIB = libopngreduc.a

OPNGREDUC_OBJS = opngreduc.o

OPNGREDUC_DEPINCLUDE_ZLIB = -I$(ZDIR)
#OPNGREDUC_DEPINCLUDE_ZLIB =
OPNGREDUC_DEPINCLUDE_LIBPNG = -I$(PNGDIR)
#OPNGREDUC_DEPINCLUDE_LIBPNG =
OPNGREDUC_DEPINCLUDES = \
  $(OPNGREDUC_DEPINCLUDE_ZLIB) \
  $(OPNGREDUC_DEPINCLUDE_LIBPNG)

all: $(OPNGREDUC_LIB)

test:

check: test

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(OPNGREDUC_DEPINCLUDES) -o $@ $<

$(OPNGREDUC_LIB): $(OPNGREDUC_OBJS)
	$(AR) $(ARFLAGS) $@ $(OPNGREDUC_OBJS)
	$(RANLIB) $@

opngreduc.o: opngreduc.c opngreduc.h

clean:
	-$(RM_F) $(OPNGREDUC_LIB) $(OPNGREDUC_OBJS)

distclean: clean
	-$(RM_F) Makefile
