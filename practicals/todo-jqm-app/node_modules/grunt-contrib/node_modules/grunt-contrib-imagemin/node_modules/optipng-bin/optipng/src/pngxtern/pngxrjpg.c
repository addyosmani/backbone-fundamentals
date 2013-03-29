/*
 * pngxrjpg.c - libpng external I/O: JPEG reader stub.
 * Copyright (C) 2006-2011 Cosmin Truta.
 *
 * From the information-theoretic point of view, JPEG-to-PNG conversion
 * is not a worthwhile task.  Moreover, a JPEG decoder would add a
 * significant overhead to the application.  As such, we provide a stub
 * that recognizes the JPEG format, without actually processing it.
 */

#include "pngxtern.h"
#include <stdio.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


/*
 * These are the various JPEG format signatures.
 *
 * FF D8 FF E0 ........................... JFIF
 * FF D8 FF E1 ........................... EXIF
 * FF D8 FF F7 ........................... JPEG-LS
 * FF 4F FF 51 ........................... JPEG-2000 codestream
 * 00 00 00 0C 6A 50 20 20 0D 0A 87 0A ... JPEG-2000 .jp2
 * 8B 4A 4E 47 0D 0A 1A 0A ............... JNG
 * 00 00 00 10 4A 48 44 52 ............... JNG datastream
 * etc.
 */

#define JPEG_SIG_JP2_SIZE 12
#define JPEG_SIG_JPC_SIZE  4
#define JPEG_SIG_JNG_SIZE  8
#define JPEG_SIG_SIZE_MAX 12

static const png_byte jpeg_sig_jp2[JPEG_SIG_JP2_SIZE] =
   { 0x00, 0x00, 0x00, 0x0c, 0x6a, 0x50, 0x20, 0x20, 0x0d, 0x0a, 0x87, 0x0a };
static const png_byte jpeg_sig_jpc[JPEG_SIG_JPC_SIZE] =
   { 0xff, 0x4f, 0xff, 0x51 };
static const png_byte jpeg_sig_jng[JPEG_SIG_JNG_SIZE] =
   { 0x8b, 0x4a, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
static const png_byte jpeg_sig_jng_jhdr[JPEG_SIG_JNG_SIZE] =
   { 0x00, 0x00, 0x00, 0x1a, 0x4a, 0x48, 0x44, 0x52 };


int /* PRIVATE */
pngx_sig_is_jpeg(png_bytep sig, size_t sig_size,
                 png_const_charpp fmt_name_ptr,
                 png_const_charpp fmt_long_name_ptr)
{
   const char *fmt;
   unsigned int marker;
   int result;

   if (sig_size < JPEG_SIG_SIZE_MAX)
      return -1;
   if (sig[0] == 0xff && sig[1] == 0xd8 && sig[2] == 0xff)
   {
      marker = 0xff00U | sig[3];
      if ((marker >= 0xffc0U && marker <= 0xffcfU) ||
          (marker >= 0xffdaU && marker <= 0xfffeU))
      {
         fmt = "JPEG";
         result = 1;  /* JFIF, EXIF, JPEG-LS, codestream, etc. */
      }
      else
         return 0;  /* not JPEG */
   }
   else if (memcmp(sig, jpeg_sig_jp2, JPEG_SIG_JP2_SIZE) == 0 ||
            memcmp(sig, jpeg_sig_jpc, JPEG_SIG_JPC_SIZE) == 0)
   {
      fmt = "JPEG-2000";
      result = 2;  /* .jp2 or JPEG-2000 codestream */
   }
   else if (memcmp(sig, jpeg_sig_jng, JPEG_SIG_JNG_SIZE) == 0 ||
            memcmp(sig, jpeg_sig_jng_jhdr, JPEG_SIG_JNG_SIZE) == 0)
   {
      fmt = "JNG";
      result = 3;  /* JNG, standalone or datastream */
   }
   else
      return 0;  /* not JPEG */

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
      *fmt_name_ptr = fmt;
   if (fmt_long_name_ptr != NULL)
      *fmt_long_name_ptr = fmt;
   return result;
}

int /* PRIVATE */
pngx_read_jpeg(png_structp png_ptr, png_infop info_ptr, FILE *stream)
{
   png_byte buf[JPEG_SIG_SIZE_MAX];
   int sig_code;

   if (fread(buf, JPEG_SIG_SIZE_MAX, 1, stream) != 1)
      return 0;  /* not a JPEG file */
   sig_code = pngx_sig_is_jpeg(buf, JPEG_SIG_SIZE_MAX, NULL, NULL);
   /* TODO: Use the format names passed by pngx_sig_is_jpeg. */
   switch (sig_code)
   {
   case 1:
      png_error(png_ptr, "JPEG decoding is not supported");
      /* NOTREACHED */
      break;
   case 2:
      png_error(png_ptr, "JPEG-2000 decoding is not supported");
      /* NOTREACHED */
      break;
   case 3:
      png_error(png_ptr, "JNG (JPEG) decoding is not supported");
      /* NOTREACHED */
      break;
   }
   if (info_ptr == NULL)  /* dummy, keep compilers happy */
      return 0;
   return 0;  /* always fail */
}
