/*
 * pngxread.c - libpng external I/O: read utility functions.
 * Copyright (C) 2003-2011 Cosmin Truta.
 */

#include "pngxtern.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


static int
pngx_sig_is_png(png_structp png_ptr,
                png_bytep sig, size_t sig_size,
                png_const_charpp fmt_name_ptr,
                png_const_charpp fmt_long_name_ptr)
{
   /* The signature of this function differs from the other pngx_sig_is_X()
    * functions, to allow extra functionality (e.g. customized error messages)
    * without requiring a full pngx_read_png().
    */

   static const char pngx_png_standalone_fmt_name[] =
      "PNG";
   static const char pngx_png_datastream_fmt_name[] =
      "PNG datastream";
   static const char pngx_png_standalone_fmt_long_name[] =
      "Portable Network Graphics";
   static const char pngx_png_datastream_fmt_long_name[] =
      "Portable Network Graphics embedded datastream";

   static const png_byte png_file_sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   static const png_byte mng_file_sig[8] = {138, 77, 78, 71, 13, 10, 26, 10};
   static const png_byte png_ihdr_sig[8] = {0, 0, 0, 13, 73, 72, 68, 82};

   int has_png_sig;

   /* Since png_read_png() fails rather abruptly with png_error(),
    * spend a little more effort to ensure that the format is indeed PNG.
    * Among other things, look for the presence of IHDR.
    */
   if (sig_size <= 25 + 18)  /* size of (IHDR + IDAT) > (12+13) + (12+6) */
      return -1;
   has_png_sig = (memcmp(sig, png_file_sig, 8) == 0);
   if (memcmp(sig + (has_png_sig ? 8 : 0), png_ihdr_sig, 8) != 0)
   {
      /* This is not valid PNG: get as much information as possible. */
      if (memcmp(sig, png_file_sig, 4) == 0 && (sig[4] == 10 || sig[4] == 13))
         png_error(png_ptr,
            "PNG file appears to be corrupted by text file conversions");
      else if (memcmp(sig, mng_file_sig, 8) == 0)
         png_error(png_ptr, "MNG decoding is not supported");
      /* JNG is handled by the pngxrjpg module. */
      return 0;  /* not PNG */
   }

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
   {
      *fmt_name_ptr = has_png_sig ?
         pngx_png_standalone_fmt_name :
         pngx_png_datastream_fmt_name;
   }
   if (fmt_long_name_ptr != NULL)
   {
      *fmt_long_name_ptr = has_png_sig ?
         pngx_png_standalone_fmt_long_name :
         pngx_png_datastream_fmt_long_name;
   }
   return 1;  /* PNG, really! */
}

int PNGAPI
pngx_read_image(png_structp png_ptr, png_infop info_ptr,
                png_const_charpp fmt_name_ptr,
                png_const_charpp fmt_long_name_ptr)
{
   png_byte sig[128];
   size_t num;
   int (*read_fn)(png_structp, png_infop, FILE *);
   FILE *stream;
   fpos_t fpos;
   int result;

   /* Precondition. */
#ifdef PNG_FLAG_MALLOC_NULL_MEM_OK
   if (png_ptr->flags & PNG_FLAG_MALLOC_NULL_MEM_OK)
      png_error(png_ptr, "pngxtern requires a safe allocator");
#endif

   /* Read the signature bytes. */
   stream = (FILE *)png_get_io_ptr(png_ptr);
   if (fgetpos(stream, &fpos) != 0)
      png_error(png_ptr, "Can't ftell in input file stream");
   num = fread(sig, 1, sizeof(sig), stream);
   if (fsetpos(stream, &fpos) != 0)
      png_error(png_ptr, "Can't fseek in input file stream");

   /* Try the PNG format first. */
   if (pngx_sig_is_png(png_ptr, sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
   {
      png_read_png(png_ptr, info_ptr, 0, NULL);
      if (getc(stream) != EOF)
      {
         png_warning(png_ptr, "Extraneous data found after IEND");
         fseek(stream, 0, SEEK_END);
      }
      return 1;
   }

   /* Check the signature bytes against other known image formats. */
   if (pngx_sig_is_bmp(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
      read_fn = pngx_read_bmp;
   else if (pngx_sig_is_gif(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
      read_fn = pngx_read_gif;
   else if (pngx_sig_is_jpeg(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
      read_fn = pngx_read_jpeg;
   else if (pngx_sig_is_pnm(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
      read_fn = pngx_read_pnm;
   else if (pngx_sig_is_tiff(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
      read_fn = pngx_read_tiff;
   else
      return 0;  /* not a known image format */

   /* Read the image. */
   result = read_fn(png_ptr, info_ptr, stream);
   /* Signature checking may give false positives; reading can still fail. */
   if (result <= 0)  /* this isn't the format we thought it was */
      if (fsetpos(stream, &fpos) != 0)
         png_error(png_ptr, "Can't fseek in input file stream");
   return result;
}
