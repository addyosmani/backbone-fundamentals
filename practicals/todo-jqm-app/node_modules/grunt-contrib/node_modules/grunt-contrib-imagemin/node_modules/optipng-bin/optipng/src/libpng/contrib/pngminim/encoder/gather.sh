#!/bin/sh

cp ../../pngminus/pnm2png.c pnm2pngm.c
cp ../../../*.h .
cp ../../../*.c .
rm example.c pngtest.c pngr*.c pngpread.c
# Change the next 2 lines if zlib is somewhere else.
cp ../../../../zlib/*.h .
cp ../../../../zlib/*.c .
rm inf*.[ch]
rm uncompr.c minigzip.c example.c gz*
