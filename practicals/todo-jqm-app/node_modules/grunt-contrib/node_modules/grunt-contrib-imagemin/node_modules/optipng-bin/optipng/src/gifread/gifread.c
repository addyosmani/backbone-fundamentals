/**
 * @file gifread.c
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
 *
 * @bug GIF/LZW decompression is not thread-safe.
 **/

#include "gifread.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if UCHAR_MAX == 255
#define GIF_UCHAR_MAX 255
#else
#error This module requires 8-bit bytes.
#endif

#define LZW_BITS_MAX 12
#define LZW_CODE_MAX ((1 << LZW_BITS_MAX) - 1)

#define LZW_FALSE 0
#define LZW_TRUE  1


static void GIFReadNextImage(struct GIFImage *image, FILE *stream);
static void GIFReadImageData(struct GIFImage *image, FILE *stream);
static int  GIFReadDataBlock(unsigned char *buffer, FILE *stream);
static void GIFSkipDataBlocks(FILE *stream);
static int  LZWGetCode(int code_size, int init_flag, FILE *stream);
static int  LZWReadByte(int init_flag, int input_code_size, FILE *stream);
static void GIFReadNextExtension(struct GIFExtension *ext, FILE *stream);

static void DefaultError(const char *message);
static void DefaultWarning(const char *message);
static void MemoryError(void);
static void ReadError(void);


#define GIF_GETW(buffer) \
    ((buffer)[0] + ((buffer)[1] << 8))

#define GIF_FGETC(ch, file) \
    { if ((ch = getc(file)) == EOF) ReadError(); }

#define GIF_FREAD(buffer, len, file) \
    { if (fread(buffer, len, 1, file) <= 0) ReadError(); }

#ifdef GIF_DEBUG
#define GIF_TRACE(args) (printf args)
#else
#define GIF_TRACE(args) ((void)0)
#endif


/*
 * Reads the GIF screen and the global color table.
 */
void GIFReadScreen(struct GIFScreen *screen, FILE *stream)
{
    unsigned char buffer[7];

    GIF_TRACE(("Reading Header\n"));
    GIF_FREAD(buffer, 6, stream);
    if (memcmp(buffer, "GIF", 3) != 0)
        GIFError("Not a GIF file");
    if ((memcmp(buffer + 3, "87a", 3) != 0) &&
            (memcmp(buffer + 3, "89a", 3) != 0))
        GIFWarning("Invalid GIF version number, not \"87a\" or \"89a\"");

    GIF_TRACE(("Reading Logical Screen Descriptor\n"));
    GIF_FREAD(buffer, 7, stream);
    screen->Width            = GIF_GETW(buffer + 0);
    screen->Height           = GIF_GETW(buffer + 2);
    screen->GlobalColorFlag  = (buffer[4] & 0x80) ? 1 : 0;
    screen->ColorResolution  = ((buffer[4] & 0x70) >> 3) + 1;
    screen->SortFlag         = (buffer[4] & 0x08) ? 1 : 0;
    screen->GlobalNumColors  = 2 << (buffer[4] & 0x07);
    screen->Background       = buffer[5];
    screen->PixelAspectRatio = buffer[6];

    if (screen->GlobalColorFlag)
    {
        GIF_TRACE(("Reading Global Color Table\n"));
        GIF_FREAD(screen->GlobalColorTable, 3 * screen->GlobalNumColors,
            stream);
    }

    GIF_TRACE(("Validating Logical Screen Descriptor\n"));
    if (screen->Width == 0 || screen->Height == 0)
        GIFError("Invalid image dimensions");
    if (screen->Background > 0)
    {
        if ((screen->GlobalColorFlag &&
             (screen->Background >= screen->GlobalNumColors)) ||
            !screen->GlobalColorFlag)
        {
#if 0       /* too noisy */
            GIFWarning("Invalid background color index");
#endif
            screen->Background = 0;
        }
    }
}

/*
 * Initializes a GIF image object.
 */
void GIFInitImage(struct GIFImage *image,
                  struct GIFScreen *screen, unsigned char **rows)
{
    image->Screen = screen;
    image->Rows   = rows;
}

/*
 * Destroys a GIF image object.
 */
void GIFDestroyImage(struct GIFImage *image)
{
    (void)image;  /* nothing to do */
}

/*
 * Reads the next GIF block into an image or extension object.
 */
int GIFReadNextBlock(struct GIFImage *image, struct GIFExtension *ext,
                     FILE *stream)
{
    int ch;
    int foundBogus;

    foundBogus = 0;
    for ( ; ; )
    {
        GIF_FGETC(ch, stream);
        switch (ch)
        {
        case GIF_IMAGE:       /* ',' */
            GIFReadNextImage(image, stream);
            return ch;
        case GIF_EXTENSION:   /* '!' */
            GIFReadNextExtension(ext, stream);
            return ch;
        case GIF_TERMINATOR:  /* ';' */
            return ch;
        default:
            if (!foundBogus)
                GIFWarning("Bogus data in GIF");
            foundBogus = 1;
        }
    }
}

/*
 * Reads the next GIF image and local color table.
 */
static void GIFReadNextImage(struct GIFImage *image, FILE *stream)
{
    struct GIFScreen *screen;
    unsigned char    buffer[9];

    GIF_TRACE(("Reading Local Image Descriptor\n"));
    GIF_FREAD(buffer, 9, stream);
    if (image == NULL)
    {
        GIFSkipDataBlocks(stream);
        return;
    }

    image->LeftPos        = GIF_GETW(buffer + 0);
    image->TopPos         = GIF_GETW(buffer + 2);
    image->Width          = GIF_GETW(buffer + 4);
    image->Height         = GIF_GETW(buffer + 6);
    image->LocalColorFlag = (buffer[8] & 0x80) ? 1 : 0;
    image->InterlaceFlag  = (buffer[8] & 0x40) ? 1 : 0;
    image->SortFlag       = (buffer[8] & 0x20) ? 1 : 0;
    image->LocalNumColors = image->LocalColorFlag ? (2 << (buffer[8] & 0x07)) : 0;

    if (image->LocalColorFlag)
    {
        GIF_TRACE(("Reading Local Color Table\n"));
        GIF_FREAD(image->LocalColorTable, 3 * image->LocalNumColors, stream);
    }

    GIF_TRACE(("Validating Logical Screen Descriptor\n"));
    screen = image->Screen;

    if (image->Width == 0 || image->Height == 0 ||
            image->LeftPos + image->Width > screen->Width ||
            image->TopPos + image->Height > screen->Height)
        GIFError("Invalid image dimensions");

    GIFReadImageData(image, stream);
}

static void GIFReadImageData(struct GIFImage *image, FILE *stream)
{
    int           minCodeSize;
    unsigned char **rows;
    unsigned int  width, height, interlaced;
    unsigned char *colors;
    unsigned int  numColors;
    unsigned int  xpos, ypos;
    int           pass, val;

    GIF_TRACE(("Reading Image Data\n"));

    /* Initialize the compression routines. */
    GIF_FGETC(minCodeSize, stream);
    if (minCodeSize >= LZW_BITS_MAX)  /* this should be in fact <= 8 */
        GIFError("GIF/LZW error: invalid LZW code size");

    if (LZWReadByte(LZW_TRUE, minCodeSize, stream) < 0)
        GIFError("Error reading GIF image");

    /* Ignore the picture if it is "uninteresting". */
    rows = image->Rows;
    if (rows == NULL)
    {
#if 0
        while (LZWReadByte(LZW_FALSE, minCodeSize, stream) >= 0)
        {
        }
#else
        /* This is faster, but possible LZW errors may go undetected. */
        GIFSkipDataBlocks(stream);
#endif
        return;
    }

    width      = image->Width;
    height     = image->Height;
    interlaced = image->InterlaceFlag;
    GIFGetColorTable(&colors, &numColors, image);
    xpos = ypos = 0;
    pass = 0;
    while ((val = LZWReadByte(LZW_FALSE, minCodeSize, stream)) >= 0)
    {
        if ((unsigned int)val >= numColors)
        {
            GIFWarning("Pixel value out of range");
            val = numColors - 1;
        }
        rows[ypos][xpos] = (unsigned char)val;
        if (++xpos == width)
        {
            xpos = 0;
            if (interlaced)
            {
                switch (pass)
                {
                case 0:
                case 1:
                    ypos += 8;
                    break;
                case 2:
                    ypos += 4;
                    break;
                case 3:
                    ypos += 2;
                    break;
                }
                if (ypos >= height)
                {
                    switch (++pass)
                    {
                    case 1:
                        ypos = 4;
                        break;
                    case 2:
                        ypos = 2;
                        break;
                    case 3:
                        ypos = 1;
                        break;
                    default:
                        goto fini;
                    }
                }
            }
            else
                ++ypos;
        }
        if (ypos >= height)
            break;
    }
fini:
    /* Ignore the trailing garbage. */
    while (LZWReadByte(LZW_FALSE, minCodeSize, stream) >= 0)
    {
    }
}

static int DataBlockSize = 0;

static int GIFReadDataBlock(unsigned char *buffer, FILE *stream)
{
    int count;

    GIF_FGETC(count, stream);
    DataBlockSize = count;
    if (count > 0)
    {
        GIF_FREAD(buffer, (unsigned int)count, stream);
    }
    return count;
}

static void GIFSkipDataBlocks(FILE *stream)
{
    int           count;
    unsigned char buffer[GIF_UCHAR_MAX + 1];

    for ( ; ; )
    {
        GIF_FGETC(count, stream)
        if (count > 0)
        {
            GIF_FREAD(buffer, (unsigned int)count, stream);
        }
        else
            return;
    }
}

static int LZWGetCode(int code_size, int init_flag, FILE *stream)
{
    static unsigned char buffer[280];
    static int           curbit, lastbit, done, last_byte;
    int                  count, i, j, ret;

    if (init_flag)
    {
        curbit = 0;
        lastbit = 0;
        done = LZW_FALSE;
        return 0;
    }

    if ((curbit + code_size) >= lastbit)
    {
        if (done)
        {
            if (curbit >= lastbit)
                GIFError("GIF/LZW error: ran off the end of my bits");
            return -1;
        }
        buffer[0] = buffer[last_byte-2];
        buffer[1] = buffer[last_byte-1];

        if ((count = GIFReadDataBlock(&buffer[2], stream)) == 0)
            done = LZW_TRUE;

        last_byte = 2 + count;
        curbit = (curbit - lastbit) + 16;
        lastbit = (2 + count) * 8;
    }

    ret = 0;
    for (i = curbit, j = 0; j < code_size; ++i, ++j)
        ret |= ((buffer[ i / 8 ] & (1 << (i % 8))) != 0) << j;

    curbit += code_size;
    return ret;
}

static int LZWReadByte(int init_flag, int input_code_size, FILE *stream)
{
    static int fresh = LZW_FALSE;
    int        code, incode;
    static int code_size, set_code_size;
    static int max_code, max_code_size;
    static int firstcode, oldcode;
    static int clear_code, end_code;
    static int table[2][LZW_CODE_MAX + 1];
    static int stack[(LZW_CODE_MAX + 1) * 2], *sp;
    int        i;

    if (init_flag)
    {
        set_code_size = input_code_size;
        code_size = set_code_size+1;
        clear_code = 1 << set_code_size;
        end_code = clear_code + 1;
        max_code_size = 2 * clear_code;
        max_code = clear_code + 2;

        LZWGetCode(0, LZW_TRUE, stream);

        fresh = LZW_TRUE;

        for (i = 0; i < clear_code; ++i)
        {
            table[0][i] = 0;
            table[1][i] = i;
        }
        for ( ; i <= LZW_CODE_MAX; ++i)
        {
            table[0][i] = 0;
            table[1][i] = 0;
        }

        sp = stack;
        return 0;
    }
    else if (fresh)
    {
        fresh = LZW_FALSE;
        do
        {
            firstcode = oldcode =
                LZWGetCode(code_size, LZW_FALSE, stream);
        } while (firstcode == clear_code);
        return firstcode;
    }

    if (sp > stack)
        return *--sp;

    while ((code = LZWGetCode(code_size, LZW_FALSE, stream)) >= 0)
    {
        if (code == clear_code)
        {
            for (i = 0; i < clear_code; ++i)
            {
                table[0][i] = 0;
                table[1][i] = i;
            }
            for ( ; i <= LZW_CODE_MAX; ++i)
            {
                table[0][i] = 0;
                table[1][i] = 0;
            }

            code_size = set_code_size+1;
            max_code_size = 2*clear_code;
            max_code = clear_code+2;
            sp = stack;
            firstcode = oldcode =
                LZWGetCode(code_size, LZW_FALSE, stream);
            return firstcode;
        }
        else if (code == end_code)
        {
            int           count;
            unsigned char buffer[260];

            if (DataBlockSize == 0)
                return -2;

            while ((count = GIFReadDataBlock(buffer, stream)) > 0)
            {
            }

#if 0       /* too noisy */
            if (count != 0)
                GIFWarning("missing EOD in data stream (common occurence)");
#else
            (void)count;
#endif
            return -2;
        }

        incode = code;

        if (code >= max_code)
        {
            *sp++ = firstcode;
            code = oldcode;
        }

        while (code >= clear_code)
        {
            *sp++ = table[1][code];
            if (code == table[0][code])
                GIFError("GIF/LZW error: circular table entry");
            code = table[0][code];
        }

        *sp++ = firstcode = table[1][code];

        if ((code = max_code) <= LZW_CODE_MAX)
        {
            table[0][code] = oldcode;
            table[1][code] = firstcode;
            ++max_code;
            if ((max_code >= max_code_size) && (max_code_size <= LZW_CODE_MAX))
            {
                max_code_size *= 2;
                ++code_size;
            }
        }

        oldcode = incode;

        if (sp > stack)
            return *--sp;
    }
    return code;
}

/*
 * The GIF spec says that if neither global nor local
 * color maps are present, the decoder should use a system
 * default map, which should have black and white as the
 * first two colors. So we use black, white, red, green, blue,
 * yellow, purple and cyan.
 * Missing color tables are not a common case, and are not
 * handled by most GIF readers.
 */
static unsigned char DefaultColorTable[] =
{
      0,   0,   0,  /* black  */
    255, 255, 255,  /* white  */
    255,   0,   0,  /* red    */
      0, 255, 255,  /* cyan   */
      0, 255,   0,  /* green  */
    255,   0, 255,  /* purple */
      0,   0, 255,  /* blue   */
    255, 255,   0,  /* yellow */
};

/*
 * Returns the local or the global color table (whichever is applicable),
 * or a predefined color table if both of these tables are missing.
 */
void GIFGetColorTable(unsigned char **colors, unsigned int *numColors,
                      struct GIFImage *image)
{
    struct GIFScreen *screen;

    if (image->LocalColorFlag)
    {
        GIF_TRACE(("Loading Local Color Table\n"));
        *colors = image->LocalColorTable;
        *numColors = image->LocalNumColors;
        return;
    }

    screen = image->Screen;
    if (screen->GlobalColorFlag)
    {
        GIF_TRACE(("Loading Global Color Table\n"));
        *colors = screen->GlobalColorTable;
        *numColors = screen->GlobalNumColors;
        return;
    }

    GIF_TRACE(("Loading Default Color Table\n"));
    *colors = DefaultColorTable;
    *numColors = sizeof(DefaultColorTable) / 3;
}

/*
 * Initializes a GIF extension object.
 */
void GIFInitExtension(struct GIFExtension *ext,
                      struct GIFScreen *screen, unsigned int initBufferSize)
{
    unsigned char *newBuffer;

    ext->Screen     = screen;
    if (initBufferSize > 0)
    {
        newBuffer = (unsigned char *)malloc(initBufferSize);
        if (newBuffer == NULL)
            MemoryError();
        ext->Buffer     = newBuffer;
        ext->BufferSize = initBufferSize;
    }
    else
    {
        ext->Buffer     = NULL;
        ext->BufferSize = 0;
    }
}

/*
 * Destroys a GIF extension object.
 */
void GIFDestroyExtension(struct GIFExtension *ext)
{
    free(ext->Buffer);
}

/*
 * Reads the next GIF extension.
 */
static void GIFReadNextExtension(struct GIFExtension *ext, FILE *stream)
{
    unsigned char *newBuffer;
    unsigned int  newBufferSize;
    unsigned int  offset, len;
    int           count, label;

    GIF_FGETC(label, stream);
    GIF_TRACE(("Reading Extension (0x%X)\n", label));
    if (ext == NULL)
    {
        GIFSkipDataBlocks(stream);
        return;
    }
    ext->Label = (unsigned char)label;

    offset = 0;
    len = ext->BufferSize;
    for ( ; ; )
    {
        if (len < GIF_UCHAR_MAX)
        {
            newBufferSize = ext->BufferSize + 1024;
            newBuffer = (unsigned char *)realloc(ext->Buffer, newBufferSize);
            if (newBuffer == NULL)
                MemoryError();
            ext->BufferSize = newBufferSize;
            ext->Buffer = newBuffer;
            len += 1024;
        }
        count = GIFReadDataBlock(ext->Buffer + offset, stream);
        if (count == 0)
            break;
        offset += count;
        len -= count;
    }
}

/*
 * Constructs a GIF graphic control extension object
 * from a raw extension object.
 */
void GIFGetGraphicCtl(struct GIFGraphicCtlExt *graphicExt,
                      struct GIFExtension *ext)
{
    unsigned char *buffer;

    GIF_TRACE(("Loading Graphic Control Extension\n"));
    if (ext->Label != GIF_GRAPHICCTL)
    {
        GIFWarning("Not a graphic control extension");
        return;
    }
    if (ext->BufferSize < 4)
    {
        GIFWarning("Broken graphic control extension");
        return;
    }

    buffer = ext->Buffer;
    graphicExt->DisposalMethod  = (buffer[0] >> 2) & 0x07;
    graphicExt->InputFlag       = (buffer[0] >> 1) & 0x01;
    graphicExt->TransparentFlag = buffer[0] & 0x01;
    graphicExt->DelayTime       = GIF_GETW(buffer + 1);
    graphicExt->Transparent     = buffer[3];
}


/*
 * The default error handler.
 */
static void DefaultError(const char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

/*
 * The default warning handler.
 */
static void DefaultWarning(const char *message)
{
    fprintf(stderr, "%s\n", message);
}

/*
 * The common memory error handler.
 */
static void MemoryError(void)
{
    GIFError("Out of memory");
}

/*
 * The common read error handler.
 */
static void ReadError(void)
{
    GIFError("Error reading file or unexpected end of file");
}

/*
 * The error handling callback.
 */
void (*GIFError)(const char *message) = DefaultError;

/*
 * The warning handling callback.
 */
void (*GIFWarning)(const char *message) = DefaultWarning;

