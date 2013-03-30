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

GIFREAD_LIB = gifread.lib
GIFREAD_OBJS = gifread.obj
GIFDUMP = test\gifdump.exe
GIFDUMP_OBJS = test\gifdump.obj
GIFDUMP_OUT = test\*.gifdump.out

all: $(GIFREAD_LIB) $(GIFDUMP)

test: $(GIFDUMP)
	$(GIFDUMP) img\compass.gif > test\compass.gifdump.out
	-@echo gifdump test ... ok

check: test

.c.obj:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -Fo$@ $<

$(GIFREAD_LIB): $(GIFREAD_OBJS)
	$(AR) $(ARFLAGS) -out:$@ $(GIFREAD_OBJS)

$(GIFDUMP): $(GIFDUMP_OBJS) $(GIFREAD_LIB)
	$(LD) $(LDFLAGS) -out:$@ $(GIFDUMP_OBJS) $(GIFREAD_LIB) $(LIBS)

gifread.obj: gifread.c gifread.h

test\gifdump.obj: test\gifdump.c gifread.h
	$(CC) -c -I. $(CPPFLAGS) $(CFLAGS) -Fo$@ $*.c

clean:
	-$(RM_F) $(GIFREAD_LIB) $(GIFREAD_OBJS)
	-$(RM_F) $(GIFDUMP) $(GIFDUMP_OBJS) $(GIFDUMP_OUT)

distclean: clean
	-$(RM_F) Makefile
