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

GIFREAD_LIB = libgifread.a
GIFREAD_OBJS = gifread.o
GIFDUMP = test/gifdump$(EXEEXT)
GIFDUMP_OBJS = test/gifdump.o
GIFDUMP_OUT = test/*.gifdump.out

all: $(GIFREAD_LIB) $(GIFDUMP)

test: $(GIFDUMP)
	$(GIFDUMP) img/compass.gif > test/compass.gifdump.out
	-@echo gifdump test ... ok

check: test

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(GIFREAD_LIB): $(GIFREAD_OBJS)
	$(AR) $(ARFLAGS) $@ $(GIFREAD_OBJS)
	$(RANLIB) $@

$(GIFDUMP): $(GIFDUMP_OBJS) $(GIFREAD_LIB)
	$(LD) $(LDFLAGS) -o $@ $(GIFDUMP_OBJS) $(GIFREAD_LIB) $(LIBS)

gifread.o: gifread.c gifread.h

test/gifdump.o: test/gifdump.c gifread.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -o $@ $*.c

clean:
	-$(RM_F) $(GIFREAD_LIB) $(GIFREAD_OBJS)
	-$(RM_F) $(GIFDUMP) $(GIFDUMP_OBJS) $(GIFDUMP_OUT)

distclean: clean
	-$(RM_F) Makefile
