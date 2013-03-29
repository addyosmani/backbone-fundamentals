# unix.mk
# Generated from Makefile.in
# Preconfigured for Unix (generic)
#
# Usage: make -f build/unix.mk

.PHONY: all test check clean distclean
.PRECIOUS: Makefile
.SUFFIXES: .c .o .a

CC = cc
CFLAGS = -O
CPP = $(CC) -E
CPPFLAGS =
LD = $(CC)
LDFLAGS = -s
AR = ar
ARFLAGS = cru
RANLIB = ranlib
LIBS =
RM_F = rm -f

PNMIO_LIB = libpnmio.a
PNMIO_OBJS = pnmin.o pnmout.o pnmutil.o

all: $(PNMIO_LIB)

test:

check: test

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(PNMIO_LIB): $(PNMIO_OBJS)
	$(AR) $(ARFLAGS) $@ $(PNMIO_OBJS)
	$(RANLIB) $@

pnmin.o: pnmin.c pnmio.h
pnmout.o: pnmout.c pnmio.h
pnmutil.o: pnmutil.c pnmio.h

clean:
	-$(RM_F) $(PNMIO_LIB) $(PNMIO_OBJS)

distclean: clean
	-$(RM_F) Makefile
