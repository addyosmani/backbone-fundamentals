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

GIFREAD_LIB = gifread.lib
GIFREAD_OBJS = gifread.obj
GIFREAD_LIBOBJS = +gifread.obj
GIFDUMP = test\gifdump.exe
GIFDUMP_OBJS = test\gifdump.obj
GIFDUMP_LIBOBJS = test\+gifdump.obj
GIFDUMP_OUT = test\*.gifdump.out

all: $(GIFREAD_LIB) $(GIFDUMP)

test: $(GIFDUMP)
	$(GIFDUMP) img\compass.gif > test\compass.gifdump.out
	-@echo gifdump test ... ok

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o$@ $<

$(GIFREAD_LIB): $(GIFREAD_OBJS)
	$(AR) $(ARFLAGS) $@ $(GIFREAD_LIBOBJS)

$(GIFDUMP): $(GIFDUMP_OBJS) $(GIFREAD_LIB)
	$(LD) $(LDFLAGS) -e$@ $(GIFDUMP_OBJS) $(GIFREAD_LIB) $(LIBS)

gifread.obj: gifread.c gifread.h

test\gifdump.obj: test\gifdump.c gifread.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -o$@ $*.c

clean:
	-$(RM_F) $(GIFREAD_LIB) $(GIFREAD_OBJS)
	-$(RM_F) $(GIFDUMP) $(GIFDUMP_OBJS) $(GIFDUMP_OUT)

distclean: clean
	-$(RM_F) Makefile
