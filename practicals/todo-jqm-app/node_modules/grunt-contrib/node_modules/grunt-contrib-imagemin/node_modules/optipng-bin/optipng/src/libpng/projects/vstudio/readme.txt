
VisualStudio instructions

libpng version 1.4.12 - July 10, 2012

Copyright (c) 1998-2010 Glenn Randers-Pehrson

This code is released under the libpng license.
For conditions of distribution and use, see the disclaimer
and license in png.h

This directory  contains support for building libpng under MicroSoft
VisualStudio 2010.  It may also work under later versions of VisualStudio.
You should be familiar with VisualStudio before using this directory.

This is a slightly modified copy of the 'vstudio' solution from libpng
1.5; it is intended to provide compatibility for libpng 1.4 with later
releases of zlib (from 1.2.4) and to provide a migration aid for 1.5.

A 'VB' DLL is no longer produced - use the standard 'Release' DLL.

Initial preparations
====================
You should install the 'official' build of zlib on your system - follow the
instructions provided on http://www.zlib.net.  When a new release is being
made of zlib the instructions disappear for a while until the new official
build is available.  You will have to wait.

You must enter some information in zlib.props before attempting to build
with this 'solution'.  Please read and edit zlib.props first.  You will
probably not be familiar with the contents of zlib.props - do not worry,
it is mostly harmless.

Make sure that your official build layout matches the requirements listed
in zlib.props.  Prior to zlib-1.2.4 the official build (1.2.3) placed
include and library files in different directories - if you find this
has happened just put them all in the same directory.

This is all you need to do to build the 'release' and 'release library'
configurations.

Debugging
=========
The release configurations default to /Ox optimization.  Full debugging
information is produced (in the .pdb), but if you encounter a problem the
optimization may make it difficult to debug.  Simply rebuild with a lower
optimization level (e.g. /Od.)

Stop reading here
=================
You have enough information to build a working application.

Debug versions have limited support
===================================
This solution includes limited support for debug versions of libpng.  You
do not need these unless your own solution itself uses debug builds (it is
far more effective to debug on the release builds, there is no point building
a special debug build.)

If, despite the previous paragraph, you still feel you need a debug build you
will have to download the zlib source as well (http://www.zlib.net) and include
the directory where you unpack it in zlib.props.  This solution will then be
able to build a minimal zlib sufficient for libpng.  This build is only
suitable for making a debug libpng.

The debug build of libpng is minimally supported.  Support for debug builds of
zlib is also minimal.  You really don't want to do this.

This solution only supports the Multi-Threaded DLL C Runtime
============================================================
If you examine the solution you will find that it very deliberately lists
exactly which MicroSoft DLLs it is linking against.  It explicitly links
against msvcrt.lib.  The debug version links against msvcrtd.lib (this is
why it is a debug version - it's nothing to do with having any special
debug support, it doesn't).

These versions of the MicroSoft C runtime correspond to the compiler flags
/MD (release) and /MDd (debug) - the "Multi-Threaded DLL" implementation of the
C runtime library.

If you need to change this then you will have to make sure that both the
compiler flags and the ".lib" in the linker dependences match up.  Fortunately
neither zlib nor libpng require libraries other than a C runtime and
kernel32.

You cannot pass (FILE*) objects from a different runtime (including msvcrtd
to msvcrt) to libpng.  If you do then your program will crash within libpng
when it first makes a file read or write call.

It is likely if you have read this far that you have found you can't avoid
having multiple C runtimes in your program.  This is OK, it does work, but
you will have to implement the libpng file read/write callbacks rather than
using the internal libpng default.  This is easy.
