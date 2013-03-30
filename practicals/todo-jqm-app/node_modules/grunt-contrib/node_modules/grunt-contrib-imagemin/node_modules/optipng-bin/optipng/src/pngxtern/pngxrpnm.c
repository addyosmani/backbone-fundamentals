/*
 * pngxrpnm.c - libpng external I/O: PNM reader.
 * Copyright (C) 2003-2012 Cosmin Truta.
 */

#include "pngxtern.h"
#include "pngxutil.h"
#include "pnmio.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


#if UINT_MAX >= 0x7fffffffUL
#define PNGX_PNM_LENGTH_MAX 0x7fffffffU
#else
#define PNGX_PNM_LENGTH_MAX UINT_MAX
#endif


static const char pbm_fmt_name[] = "PBM";
static const char pgm_fmt_name[] = "PGM";
static const char ppm_fmt_name[] = "PPM";
static const char pam_fmt_name[] = "PAM";
static const char pbm_fmt_long_name[] = "Portable Bitmap";
static const char pgm_fmt_long_name[] = "Portable Graymap";
static const char ppm_fmt_long_name[] = "Portable Pixmap";
static const char pam_fmt_long_name[] = "Portable Anymap";


int /* PRIVATE */
pngx_sig_is_pnm(png_bytep sig, size_t sig_size,
                png_const_charpp fmt_name_ptr,
                png_const_charpp fmt_long_name_ptr)
{
   static const char *fmt_names[] =
   {
      pbm_fmt_name, pgm_fmt_name, ppm_fmt_name,
      pbm_fmt_name, pgm_fmt_name, ppm_fmt_name,
      pam_fmt_name
   };
   static const char *fmt_long_names[] =
   {
      pbm_fmt_long_name, pgm_fmt_long_name, ppm_fmt_long_name,
      pbm_fmt_long_name, pgm_fmt_long_name, ppm_fmt_long_name,
      pam_fmt_long_name
   };

   /* Require at least the PNM magic signature and the trailing whitespace. */
   if (sig_size < 4)
      return -1;  /* insufficient data */
   if (sig[0] != 'P' || sig[1] < '1' || sig[1] > '7')
      return 0;  /* not PNM */
   if (sig[2] != ' ' && sig[2] != '\t' && sig[2] != '\n' && sig[2] != '\r' &&
       sig[2] != '#')
      return 0;  /* not PNM */

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
      *fmt_name_ptr = fmt_names[sig[1] - '1'];
   if (fmt_long_name_ptr != NULL)
      *fmt_long_name_ptr = fmt_long_names[sig[1] - '1'];
   return 1;  /* PNM */
}

static int
pnm_fpeek_eof(pnm_struct *pnm_ptr, FILE *stream)
{
   int ch;

   if (pnm_ptr->format >= PNM_P1 && pnm_ptr->format <= PNM_P3)
   {
      do
      {
         ch = getc(stream);
         if (ch == '#')  /* skip comments */
         {
            do
               ch = getc(stream);
            while (ch != EOF && ch != '\n' && ch != '\r');
         }
         if (ch == EOF)
            return 1;
      } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
   }
   else
   {
      ch = getc(stream);
      if (ch == EOF)
         return 1;
   }

   ungetc(ch, stream);
   return 0;
}

int /* PRIVATE */
pngx_read_pnm(png_structp png_ptr, png_infop info_ptr, FILE *stream)
{
   pnm_struct pnminfo;
   unsigned int format, depth, width, height, maxval;
   unsigned int max_width, num_samples, sample_size;
   unsigned int *pnmrow;
   size_t row_size;
   png_bytepp row_pointers;
   png_color_8 sig_bit;
   unsigned int i, j;
   int failed, overflow;

   /* Read the PNM header. */
   if (pnm_fget_header(&pnminfo, stream) != 1)
      return 0;  /* not PNM */
   format = pnminfo.format;
   depth = pnminfo.depth;
   width = pnminfo.width;
   height = pnminfo.height;
   maxval = pnminfo.maxval;
   if (format > PNM_P6)
      png_error(png_ptr, "Can't handle PNM formats newer than PPM (\"P6\")");
   max_width =
      (sizeof(size_t) <= sizeof(unsigned int)) ?
         UINT_MAX / sizeof(unsigned int) / depth : UINT_MAX;
#if UINT_MAX > PNGX_PNM_LENGTH_MAX
   if (max_width > PNGX_PNM_LENGTH_MAX)
      max_width = PNGX_PNM_LENGTH_MAX;
#endif
   if (width > max_width)
      png_error(png_ptr, "Can't handle exceedingly large PNM dimensions");
   sample_size = 1;
   row_size = num_samples = depth * width;
   if (maxval > 65535)
      png_error(png_ptr, "Can't handle PNM samples larger than 16 bits");
   else if (maxval > 255)
   {
      sample_size = 2;
      row_size *= 2;
   }

   /* Set the PNG image type. */
   png_set_IHDR(png_ptr, info_ptr,
      width, height,
      (maxval <= 255) ? 8 : 16,
      (depth == 1) ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   for (i = 1, j = 2; j - 1 < maxval; ++i, j <<= 1) { }
   if (j - 1 != maxval)
      png_warning(png_ptr,
         "Possibly inexact sample conversion from PNM to PNG");
   else if (i % 8 != 0 && (depth > 1 || 8 % i != 0))
   {
      sig_bit.red = sig_bit.green = sig_bit.blue = sig_bit.gray = (png_byte)i;
      sig_bit.alpha = 0;
      png_set_sBIT(png_ptr, info_ptr, &sig_bit);
   }

   /* Allocate memory. */
   row_pointers = pngx_malloc_rows(png_ptr, info_ptr, -1);
   if ((format >= PNM_P4) && (maxval == 255 || maxval == 65535))
      pnmrow = NULL;  /* can read raw data directly into row_pointers */
   else
      pnmrow = (unsigned int *)
         png_malloc(png_ptr, num_samples * sizeof(unsigned int));

   /* Read the image data. */
   failed = 0;
   overflow = 0;
   if (pnmrow != NULL)
   {
      for (i = 0; i < height; ++i)
      {
         if (pnm_fget_values(&pnminfo, pnmrow, 1, stream) <= 0)
            failed = 1;
         /* Transfer the samples, even on partial (unsuccessful) reads. */
         if (maxval <= 255)
         {
            for (j = 0; j < num_samples; ++j)
            {
               unsigned int val = pnmrow[j];
               if (val > maxval)
               {
                  val = 255;
                  overflow = 1;
               }
               else if (maxval != 255)
                  val = (val * 255 + maxval/2) / maxval;
               row_pointers[i][j] = (png_byte)val;
            }
         }
         else  /* maxval > 255 */
         {
            for (j = 0; j < num_samples; ++j)
            {
               png_uint_32 val = pnmrow[j];
               if (val > maxval)
               {
                  val = 65535;
                  overflow = 1;
               }
               else if (maxval != 65535)
                  val = (val * 65535 + maxval/2) / maxval;
               row_pointers[i][2 * j] = (png_byte)(val >> 8);
               row_pointers[i][2 * j + 1] = (png_byte)(val & 0xff);
            }
         }
         if (failed)
            break;
      }
   }
   else  /* read the raw data directly */
   {
      for (i = 0; i < height; ++i)
      {
         if (pnm_fget_bytes(&pnminfo,
             row_pointers[i], sample_size, 1, stream) <= 0)
         {
            failed = 1;
            break;
         }
      }
   }

   /* Wipe out the portion left unread. */
   for ( ; i < height; ++i)
      memset(row_pointers[i], 0, row_size);

   /* Deallocate the temporary row buffer. */
   if (pnmrow != NULL)
      png_free(png_ptr, pnmrow);

   /* Check the results. */
   if (overflow)
      png_warning(png_ptr, "Overflow in PNM samples");
   if (failed)
      png_error(png_ptr, "Error in PNM image file");
   else if (!pnm_fpeek_eof(&pnminfo, stream))
      png_warning(png_ptr, "Extraneous data found after PNM image");
   /* FIXME: A PNM file can have more than one image. */

   return 1;  /* one image has been successfully read */
}
