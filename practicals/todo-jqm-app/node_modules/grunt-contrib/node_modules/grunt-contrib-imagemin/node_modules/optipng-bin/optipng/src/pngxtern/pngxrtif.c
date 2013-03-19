/*
 * pngxrtif.c - libpng external I/O: TIFF reader.
 * Copyright (C) 2006-2012 Cosmin Truta.
 */

#include "pngxtern.h"
#include "pngxutil.h"
#include "minitiff.h"
#include <stdio.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


/*
 * The TIFF format name.
 */
static const char tiff_fmt_name[] = "TIFF";
static const char tiff_fmt_long_name[] = "Tagged Image File Format";


int /* PRIVATE */
pngx_sig_is_tiff(png_bytep sig, size_t sig_size,
                 png_const_charpp fmt_name_ptr,
                 png_const_charpp fmt_long_name_ptr)
{
   /* Require at least the TIFF signature. */
   if (sig_size < 8)
      return -1;  /* insufficient data */
   if (memcmp(sig, minitiff_sig_m, 4) != 0 &&
       memcmp(sig, minitiff_sig_i, 4) != 0)
      return 0;  /* not TIFF */

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
      *fmt_name_ptr = tiff_fmt_name;
   if (fmt_long_name_ptr != NULL)
      *fmt_long_name_ptr = tiff_fmt_long_name;
   return 1;  /* TIFF */
}


/* FIXME: Not thread-safe. */
static png_structp err_png_ptr = NULL;
static unsigned int num_extra_images;

static void pngx_tiff_error(const char *msg)
{
   png_error(err_png_ptr, msg);
}

static void pngx_tiff_warning(const char *msg)
{
   /* FIXME:
    * Inspection of warning messages is fragile, but is
    * required by the limitations of minitiff version 0.1.
    */
   if (strstr(msg, "multi-image") != NULL)
      ++num_extra_images;

#if 0
   /* Metadata is not imported, so warnings need not be shown. */
   png_warning(err_png_ptr, msg);
#endif
}


int /* PRIVATE */
pngx_read_tiff(png_structp png_ptr, png_infop info_ptr, FILE *stream)
{
   struct minitiff_info tiff_info;
   unsigned int width, height, pixel_size, sample_depth, sample_max;
   int color_type;
   int sample_overflow;
   png_bytepp row_pointers;
   png_bytep row;
   unsigned int i, j, k;

   err_png_ptr = png_ptr;
   num_extra_images = 0;
   minitiff_init_info(&tiff_info);
   tiff_info.error_handler = pngx_tiff_error;
   tiff_info.warning_handler = pngx_tiff_warning;
   minitiff_read_info(&tiff_info, stream);
   minitiff_validate_info(&tiff_info);

   width = (unsigned int)tiff_info.width;
   height = (unsigned int)tiff_info.height;
   pixel_size = tiff_info.samples_per_pixel;
   sample_depth = tiff_info.bits_per_sample;
   switch (pixel_size)
   {
   case 1:
      color_type = PNG_COLOR_TYPE_GRAY;
      break;
   case 2:
      color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
      break;
   case 3:
      color_type = PNG_COLOR_TYPE_RGB;
      break;
   case 4:
      color_type = PNG_COLOR_TYPE_RGB_ALPHA;
      break;
   default:
      png_error(png_ptr, "Unsupported TIFF color space");
      /* NOTREACHED */
      return 0;  /* avoid "uninitialized color_type" warning */
   }
   if (sample_depth > 16)
      png_error(png_ptr, "Unsupported TIFF sample depth");
   sample_max = (1 << sample_depth) - 1;
   sample_overflow = 0;

   png_set_IHDR(png_ptr, info_ptr, width, height,
      (sample_depth <= 8) ? 8 : 16,
      color_type,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   row_pointers = pngx_malloc_rows(png_ptr, info_ptr, 0);

   if (sample_depth <= 8)
   {
      for (i = 0; i < height; ++i)
      {
         row = row_pointers[i];
         minitiff_read_row(&tiff_info, row, i, stream);
         if (sample_depth < 8)
         {
            for (j = 0; j < pixel_size * width; ++j)
            {
               unsigned int b = row[j];
               if (b > sample_max)
               {
                  b = sample_max;
                  sample_overflow = 1;
               }
               row[j] = (png_byte)((b * 255 + sample_max / 2) / sample_max);
            }
         }
         if (tiff_info.photometric == 0)
         {
            for (j = 0; j < pixel_size * width; ++j)
               row[j] = (png_byte)(255 - row[j]);
         }
      }
   }
   else
   {
      for (i = 0; i < height; ++i)
      {
         row = row_pointers[i];
         minitiff_read_row(&tiff_info, row, i, stream);
         if (tiff_info.byte_order == 'I')
         {
            /* "Intel" byte order => swap row bytes */
            for (j = k = 0; j < pixel_size * width; ++j, k+=2)
            {
               png_byte b = row[k];
               row[k] = row[k + 1];
               row[k + 1] = b;
            }
         }
         if (sample_depth < 16)
         {
            for (j = k = 0; k < pixel_size * width; ++j, k+=2)
            {
               unsigned int b = (row[k] << 8) + row[k + 1];
               if (b > sample_max)
               {
                  b = sample_max;
                  sample_overflow = 1;
               }
               b = (b * 65535U + sample_max / 2) / sample_max;
               row[k] = (png_byte)(b >> 8);
               row[k + 1] = (png_byte)(b & 255);
            }
         }
      }
   }

   if (sample_overflow)
      png_warning(png_ptr, "Overflow in TIFF samples");

   minitiff_destroy_info(&tiff_info);
   return 1 + num_extra_images;
}
