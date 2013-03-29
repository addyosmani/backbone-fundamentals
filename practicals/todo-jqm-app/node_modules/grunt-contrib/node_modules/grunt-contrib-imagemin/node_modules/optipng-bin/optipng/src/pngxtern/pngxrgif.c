/*
 * pngxrgif.c - libpng external I/O: GIF reader.
 * Copyright (C) 2003-2012 Cosmin Truta.
 */

#include "pngxtern.h"
#include "pngxutil.h"
#include "gifread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


static const char gif_fmt_name[] = "GIF";
static const char gif_fmt_long_name[] = "Graphics Interchange Format";

static const png_byte gif_sig_gif87a[6] =
   { 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };  /* "GIF87a" */
static const png_byte gif_sig_gif89a[6] =
   { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };  /* "GIF89a" */


int /* PRIVATE */
pngx_sig_is_gif(png_bytep sig, size_t sig_size,
                png_const_charpp fmt_name_ptr,
                png_const_charpp fmt_long_name_ptr)
{
   /* Require at least the GIF signature and the screen descriptor. */
   if (sig_size < 6 + 7)
      return -1;  /* insufficient data */
   if (memcmp(sig, gif_sig_gif87a, 6) != 0 &&
       memcmp(sig, gif_sig_gif89a, 6) != 0)
      return 0;  /* not GIF */

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
      *fmt_name_ptr = gif_fmt_name;
   if (fmt_long_name_ptr != NULL)
      *fmt_long_name_ptr = gif_fmt_long_name;
   return 1;  /* GIF */
}


/* FIXME: Not thread-safe. */
static png_structp err_png_ptr;
static struct GIFImage *err_gif_image_ptr;
static struct GIFExtension *err_gif_ext_ptr;

static void
pngx_gif_error(const char *msg)
{
   if (err_gif_image_ptr != NULL)
      GIFDestroyImage(err_gif_image_ptr);
   if (err_gif_ext_ptr != NULL)
      GIFDestroyExtension(err_gif_ext_ptr);
   png_error(err_png_ptr, msg);
}

static void
pngx_gif_warning(const char *msg)
{
   png_warning(err_png_ptr, msg);
}


static void
pngx_set_gif_palette(png_structp png_ptr, png_infop info_ptr,
   unsigned char *color_table, unsigned int num_colors)
{
   png_color palette[256];
   unsigned int i;

   PNGX_ASSERT(color_table != NULL);
   PNGX_ASSERT(num_colors <= 256);

   for (i = 0; i < num_colors; ++i)
   {
      palette[i].red   = color_table[3 * i];
      palette[i].green = color_table[3 * i + 1];
      palette[i].blue  = color_table[3 * i + 2];
   }
   png_set_PLTE(png_ptr, info_ptr, palette, (int)num_colors);
}

static void
pngx_set_gif_transparent(png_structp png_ptr, png_infop info_ptr,
   unsigned int transparent)
{
   png_byte trans[256];
   unsigned int i;

   PNGX_ASSERT(transparent < 256);

   for (i = 0; i < transparent; ++i)
      trans[i] = 255;
   trans[transparent] = 0;
   png_set_tRNS(png_ptr, info_ptr, trans, (int)transparent + 1, NULL);
}

int /* PRIVATE */
pngx_read_gif(png_structp png_ptr, png_infop info_ptr, FILE *stream)
{
   /* GIF-specific data */
   struct GIFScreen screen;
   struct GIFImage image;
   struct GIFExtension ext;
   struct GIFGraphicCtlExt graphicExt;
   int blockCode;
   unsigned char *colorTable;
   unsigned int numColors;
   unsigned int transparent;
   unsigned int numImages;
   /* PNG-specific data */
   png_uint_32 width, height;
   png_bytepp row_pointers;

   /* Set up the custom error handling. */
   GIFError = pngx_gif_error;
   GIFWarning = pngx_gif_warning;
   err_png_ptr = png_ptr;
   err_gif_image_ptr = NULL;
   err_gif_ext_ptr = NULL;

   /* Read the GIF screen. */
   GIFReadScreen(&screen, stream);
   width = screen.Width;
   height = screen.Height;

   /* Set the PNG image type. */
   png_set_IHDR(png_ptr, info_ptr,
      width, height, 8, PNG_COLOR_TYPE_PALETTE,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   /* Allocate memory. */
   row_pointers = pngx_malloc_rows(png_ptr, info_ptr, (int)screen.Background);

   /* Complete the initialization of the GIF reader. */
   GIFInitImage(&image, &screen, row_pointers);
   err_gif_image_ptr = &image;
   GIFInitExtension(&ext, &screen, 256);
   err_gif_ext_ptr = &ext;
   numImages = 0;
   transparent = (unsigned int)(-1);

   /* Iterate over the GIF file. */
   for ( ; ; )
   {
      blockCode = GIFReadNextBlock(&image, &ext, stream);
      if (blockCode == GIF_IMAGE)  /* ',' */
      {
         if (image.Rows != NULL)
         {
            /* Complete the PNG info. */
            if (image.InterlaceFlag)
               pngx_set_interlace_type(png_ptr, info_ptr,
                  PNG_INTERLACE_ADAM7);
            GIFGetColorTable(&colorTable, &numColors, &image);
            pngx_set_gif_palette(png_ptr, info_ptr, colorTable, numColors);
            if (transparent < 256)
               pngx_set_gif_transparent(png_ptr, info_ptr, transparent);

            /* Inform the GIF routines not to read the upcoming images. */
            image.Rows = NULL;
         }
         ++numImages;
      }
      else if (blockCode == GIF_EXTENSION)  /* '!' */
      {
         if (ext.Label == GIF_GRAPHICCTL)
         {
            GIFGetGraphicCtl(&graphicExt, &ext);
            if (image.Rows != NULL && graphicExt.TransparentFlag)
            {
               if (transparent >= 256)
                  transparent = graphicExt.Transparent;
            }
         }
      }
      else if (blockCode == GIF_TERMINATOR)  /* ';' */
         break;
   }

   if (image.Rows != NULL)
      png_error(png_ptr, "No image in GIF file");

   GIFDestroyImage(&image);
   GIFDestroyExtension(&ext);
   return numImages;
}
