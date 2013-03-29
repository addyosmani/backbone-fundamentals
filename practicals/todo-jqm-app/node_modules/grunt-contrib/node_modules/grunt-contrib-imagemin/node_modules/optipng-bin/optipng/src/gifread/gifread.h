/**
 * @file gifread.h
 * A simple GIF reader.
 *
 * @author Cosmin Truta
 *
 * @section Copyright
 * Copyright (C) 2003-2011 Cosmin Truta.
 * This software was derived from "giftopnm.c" by David Koblas,
 * and is distributed under the same copyright and warranty terms.
 *
 * The original copyright notice is provided below.
 * <pre>
 * +-------------------------------------------------------------------+
 * | Copyright 1990, 1991, 1993, David Koblas.  (koblas@netcom.com)    |
 * |   Permission to use, copy, modify, and distribute this software   |
 * |   and its documentation for any purpose and without fee is hereby |
 * |   granted, provided that the above copyright notice appear in all |
 * |   copies and that both that copyright notice and this permission  |
 * |   notice appear in supporting documentation.  This software is    |
 * |   provided "as is" without express or implied warranty.           |
 * +-------------------------------------------------------------------+
 * </pre>
 **/

#ifndef GIFREAD_H
#define GIFREAD_H

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


#define GIF_PLAINTEXT   0x01
#define GIF_EXTENSION   0x21  /* '!' */
#define GIF_IMAGE       0x2c  /* ',' */
#define GIF_TERMINATOR  0x3b  /* ';' */
#define GIF_GRAPHICCTL  0xf9
#define GIF_COMMENT     0xfe
#define GIF_APPLICATION 0xff

#define GIF_NUMCOLORS_MAX 256

#define GIF_IND_RED   0
#define GIF_IND_GREEN 1
#define GIF_IND_BLUE  2


/**
 * The GIF screen structure.
 **/
struct GIFScreen
{
    unsigned int  Width;
    unsigned int  Height;
    unsigned int  GlobalColorFlag;
    unsigned int  ColorResolution;
    unsigned int  SortFlag;
    unsigned int  GlobalNumColors;
    unsigned int  Background;
    unsigned int  PixelAspectRatio;
    unsigned char GlobalColorTable[GIF_NUMCOLORS_MAX * 3];
};

/**
 * The GIF image structure.
 **/
struct GIFImage
{
    struct GIFScreen *Screen;
    unsigned int  LeftPos;
    unsigned int  TopPos;
    unsigned int  Width;
    unsigned int  Height;
    unsigned int  LocalColorFlag;
    unsigned int  InterlaceFlag;
    unsigned int  SortFlag;
    unsigned int  LocalNumColors;
    unsigned char LocalColorTable[GIF_NUMCOLORS_MAX * 3];
    unsigned char **Rows;
};

/**
 * The GIF extension structure.
 **/
struct GIFExtension
{
    struct GIFScreen *Screen;
    unsigned char *Buffer;
    unsigned int  BufferSize;
    unsigned char Label;
};

/**
 * The GIF graphic control extension structure.
 **/
struct GIFGraphicCtlExt
{
    unsigned int DisposalMethod;
    unsigned int InputFlag;
    unsigned int TransparentFlag;
    unsigned int DelayTime;
    unsigned int Transparent;
};


/**
 * Reads the GIF screen and the global color table.
 * @param screen  the destination screen.
 * @param stream  a file stream.
 **/
void GIFReadScreen(struct GIFScreen *screen, FILE *stream);

/**
 * Initializes a GIF image object.
 * @param image   the resulting image.
 * @param screen  a screen.
 * @param rows    an array of rows; can be NULL.
 **/
void GIFInitImage(struct GIFImage *image,
                  struct GIFScreen *screen, unsigned char **rows);

/**
 * Destroys a GIF image object.
 * @param image  an image.
 **/
void GIFDestroyImage(struct GIFImage *image);

/**
 * Reads the next GIF block into an image or extension object.
 * @param image   the destination image; can be NULL.
 * @param ext     the destination extension; can be NULL.
 * @param stream  a file stream.
 * @return        the block code or EOF.
 **/
int GIFReadNextBlock(struct GIFImage *image, struct GIFExtension *ext,
                     FILE *stream);

/**
 * Returns the local or the global color table (whichever is applicable),
 * or a predefined color table if both of these tables are missing.
 * @param colors     the resulting color table.
 * @param numColors  the size of the resulting color table.
 * @param image      an image.
 **/
void GIFGetColorTable(unsigned char **colors, unsigned int *numColors,
                      struct GIFImage *image);

/**
 * Initializes a GIF extension object.
 * @param ext             the resulting extension.
 * @param screen          a screen.
 * @param initBufferSize  an initial buffer size; can be 0.
 **/
void GIFInitExtension(struct GIFExtension *ext,
                      struct GIFScreen *screen, unsigned int initBufferSize);

/**
 * Destroys a GIF extension object.
 * @param ext  an extension.
 **/
void GIFDestroyExtension(struct GIFExtension *ext);

/**
 * Constructs a GIF graphic control extension object
 * from a raw extension object.
 * @param graphicExt  the resulting graphic control extension.
 * @param ext         a raw extension.
 **/
void GIFGetGraphicCtl(struct GIFGraphicCtlExt *graphicExt,
                      struct GIFExtension *ext);


/**
 * The error handling callback.
 * @param message  an error message.
 **/
extern void (*GIFError)(const char *message);

/**
 * The warning handling callback.
 * @param message  a warning message.
 **/
extern void (*GIFWarning)(const char *message);


#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif  /* GIFREAD_H */
