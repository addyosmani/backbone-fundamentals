/*
 * pngxrbmp.c - libpng external I/O: BMP reader.
 * Copyright (C) 2003-2012 Cosmin Truta.
 *
 * This code was derived from "bmp2png.c" by MIYASAKA Masaru, and
 * is distributed under the same copyright and warranty terms as libpng.
 */

#include "pngxtern.h"
#include "pngxutil.h"
#include <stdio.h>
#include <string.h>

#define PNGX_INTERNAL
#include "pngxpriv.h"


/*****************************************************************************/
/* BMP file header macros                                                    */
/*****************************************************************************/

/* BMP file signature */
#define BMP_SIGNATURE       0x4d42  /* "BM" */
#define BMP_SIG_BYTES       2

/* BITMAPFILEHEADER */
#define BFH_WTYPE           0       /* WORD   bfType;           */
#define BFH_DSIZE           2       /* DWORD  bfSize;           */
#define BFH_WRESERVED1      6       /* WORD   bfReserved1;      */
#define BFH_WRESERVED2      8       /* WORD   bfReserved2;      */
#define BFH_DOFFBITS        10      /* DWORD  bfOffBits;        */
#define BFH_DBIHSIZE        14      /* DWORD  biSize;           */
#define FILEHED_SIZE        14      /* sizeof(BITMAPFILEHEADER) */
#define BIHSIZE_SIZE        4       /* sizeof(biSize)           */

/* BITMAPINFOHEADER */
#define BIH_DSIZE           0       /* DWORD  biSize;             */
#define BIH_LWIDTH          4       /* LONG   biWidth;            */
#define BIH_LHEIGHT         8       /* LONG   biHeight;           */
#define BIH_WPLANES         12      /* WORD   biPlanes;           */
#define BIH_WBITCOUNT       14      /* WORD   biBitCount;         */
#define BIH_DCOMPRESSION    16      /* DWORD  biCompression;      */
#define BIH_DSIZEIMAGE      20      /* DWORD  biSizeImage;        */
#define BIH_LXPELSPERMETER  24      /* LONG   biXPelsPerMeter;    */
#define BIH_LYPELSPERMETER  28      /* LONG   biYPelsPerMeter;    */
#define BIH_DCLRUSED        32      /* DWORD  biClrUsed;          */
#define BIH_DCLRIMPORTANT   36      /* DWORD  biClrImportant;     */
#define B4H_DREDMASK        40      /* DWORD  bV4RedMask;         */
#define B4H_DGREENMASK      44      /* DWORD  bV4GreenMask;       */
#define B4H_DBLUEMASK       48      /* DWORD  bV4BlueMask;        */
#define B4H_DALPHAMASK      52      /* DWORD  bV4AlphaMask;       */
#define B4H_DCSTYPE         56      /* DWORD  bV4CSType;          */
#define B4H_XENDPOINTS      60      /* CIEXYZTRIPLE bV4Endpoints; */
#define B4H_DGAMMARED       96      /* DWORD  bV4GammaRed;        */
#define B4H_DGAMMAGREEN     100     /* DWORD  bV4GammaGreen;      */
#define B4H_DGAMMABLUE      104     /* DWORD  bV4GammaBlue;       */
#define B5H_DINTENT         108     /* DWORD  bV5Intent;          */
#define B5H_DPROFILEDATA    112     /* DWORD  bV5ProfileData;     */
#define B5H_DPROFILESIZE    116     /* DWORD  bV5ProfileSize;     */
#define B5H_DRESERVED       120     /* DWORD  bV5Reserved;        */
#define INFOHED_SIZE        40      /* sizeof(BITMAPINFOHEADER)   */
#define BMPV4HED_SIZE       108     /* sizeof(BITMAPV4HEADER)     */
#define BMPV5HED_SIZE       124     /* sizeof(BITMAPV5HEADER)     */

/* BITMAPCOREHEADER */
#define BCH_DSIZE           0       /* DWORD  bcSize;           */
#define BCH_WWIDTH          4       /* WORD   bcWidth;          */
#define BCH_WHEIGHT         6       /* WORD   bcHeight;         */
#define BCH_WPLANES         8       /* WORD   bcPlanes;         */
#define BCH_WBITCOUNT       10      /* WORD   bcBitCount;       */
#define COREHED_SIZE        12      /* sizeof(BITMAPCOREHEADER) */

/* RGB */
#define RGB_BLUE            0       /* BYTE   rgbBlue;     */
#define RGB_GREEN           1       /* BYTE   rgbGreen;    */
#define RGB_RED             2       /* BYTE   rgbRed;      */
#define RGB_RESERVED        3       /* BYTE   rgbReserved; */
#define RGBTRIPLE_SIZE      3       /* sizeof(RGBTRIPLE)   */
#define RGBQUAD_SIZE        4       /* sizeof(RGBQUAD)     */

/* Constants for the biCompression field */
#ifndef BI_RGB
#define BI_RGB              0       /* Uncompressed format       */
#define BI_RLE8             1       /* RLE format (8 bits/pixel) */
#define BI_RLE4             2       /* RLE format (4 bits/pixel) */
#define BI_BITFIELDS        3       /* Bitfield format           */
#define BI_JPEG             4       /* JPEG format               */
#define BI_PNG              5       /* PNG format                */
#endif


/*****************************************************************************/
/* BMP memory utilities                                                      */
/*****************************************************************************/

static unsigned int
bmp_get_word(png_bytep ptr)
{
   return (unsigned int)ptr[0] + ((unsigned int)ptr[1] << 8);
}

static png_uint_32
bmp_get_dword(png_bytep ptr)
{
   return ((png_uint_32)ptr[0])       + ((png_uint_32)ptr[1] << 8) +
          ((png_uint_32)ptr[2] << 16) + ((png_uint_32)ptr[3] << 24);
}


/*****************************************************************************/
/* BMP RLE helpers                                                           */
/*****************************************************************************/

static void
bmp_rle8_memset(png_bytep ptr, size_t offset, int ch, size_t len)
{
   memset(ptr + offset, ch, len);
}

static void
bmp_rle4_memset(png_bytep ptr, size_t offset, int ch, size_t len)
{
   if (len == 0)
      return;
   ptr += offset / 2;
   if (offset & 1)  /* use half-byte operations at odd offset */
   {
      *ptr = (png_byte)((*ptr & 0xf0) | (ch & 0x0f));
      ch = ((ch & 0xf0) >> 4) | ((ch & 0x0f) << 4);
      ++ptr;
      --len;
   }
   memset(ptr, ch, len / 2);
   if (len & 1)
      ptr[len / 2] = (png_byte)(ch & 0xf0);
}

static size_t
bmp_rle8_fread(png_bytep ptr, size_t offset, size_t len, FILE *stream)
{
   size_t result;

   result = fread(ptr + offset, 1, len, stream);
   if (len & 1)
      getc(stream);  /* skip padding */
   return result;
}

static size_t
bmp_rle4_fread(png_bytep ptr, size_t offset, size_t len, FILE *stream)
{
   size_t result;
   int ch;

   ptr += offset / 2;
   if (offset & 1)  /* use half-byte operations at odd offset */
   {
      for (result = 0; result < len; result += 2)
      {
         ch = getc(stream);
         if (ch == EOF)
            break;
         *ptr = (png_byte)((*ptr & 0xf0) | ((ch & 0xf0) >> 4));
         ++ptr;
         *ptr = (png_byte)((ch & 0x0f) << 4);
      }
   }
   else
   {
      result = fread(ptr, 1, (len + 1) / 2, stream) * 2;
   }
   if (len & 2)
      getc(stream);  /* skip padding */
   return (result <= len) ? result : len;
}


/*****************************************************************************/
/* BMP packbit (BITFIELDS) helpers                                           */
/*****************************************************************************/

static void
bmp_process_mask(png_uint_32 bmp_mask, png_bytep sig_bit, png_bytep shift_bit)
{
   *sig_bit = *shift_bit = (png_byte)0;
   if (bmp_mask == 0)
      return;
   while ((bmp_mask & 1) == 0)
   {
      bmp_mask >>= 1;
      ++*shift_bit;
   }
   while (bmp_mask != 0)
   {
      if ((bmp_mask & 1) == 0 || *sig_bit >= 8)
      {
         *sig_bit = (png_byte)0;
         return;
      }
      bmp_mask >>= 1;
      ++*sig_bit;
   }
}


/*****************************************************************************/
/* BMP I/O utilities                                                         */
/*****************************************************************************/

static size_t
bmp_read_rows(png_bytepp begin_row, png_bytepp end_row, size_t row_size,
              unsigned int compression, FILE *stream)
{
   size_t result;
   png_bytepp crt_row;
   int inc;
   size_t crtn, dcrtn, endn;
   unsigned int len, b1, b2;
   int ch;
   void (*bmp_memset_fn)(png_bytep, size_t, int, size_t);
   size_t (*bmp_fread_fn)(png_bytep, size_t, size_t, FILE *);

   if (row_size == 0)
      return 0;  /* this should not happen */

   inc = (begin_row <= end_row) ? 1 : -1;
   crtn = 0;
   result = 0;
   if (compression == BI_RLE4)
   {
      endn = row_size * 2;
      if (endn <= row_size)
         return 0;  /* overflow */
      bmp_memset_fn = bmp_rle4_memset;
      bmp_fread_fn = bmp_rle4_fread;
   }
   else
   {
      endn = row_size;
      bmp_memset_fn = bmp_rle8_memset;
      bmp_fread_fn = bmp_rle8_fread;
   }

   if (compression == BI_RGB || compression == BI_BITFIELDS)
   {
      /* Read uncompressed bitmap. */
      for (crt_row = begin_row; crt_row != end_row; crt_row += inc)
      {
         crtn = bmp_fread_fn(*crt_row, 0, endn, stream);
         if (crtn != endn)
            break;
         ++result;
      }
   }
   else if (compression == BI_RLE8 || compression == BI_RLE4)
   {
      /* Read RLE-compressed bitmap. */
      if (compression == BI_RLE8)
      {
         endn = row_size;
         bmp_memset_fn = bmp_rle8_memset;
         bmp_fread_fn = bmp_rle8_fread;
      }
      else /* BI_RLE4 */
      {
         endn = row_size * 2;
         if (endn <= row_size)
            return 0;  /* overflow */
         bmp_memset_fn = bmp_rle4_memset;
         bmp_fread_fn = bmp_rle4_fread;
      }
      crt_row = begin_row;
      for ( ; ; )
      {
         ch = getc(stream); b1 = (unsigned int)ch;
         ch = getc(stream); b2 = (unsigned int)ch;
         if (ch == EOF)
            break;
         if (b1 == 0)  /* escape */
         {
            if (b2 == 0)  /* end of line */
            {
               bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
               crt_row += inc;
               crtn = 0;
               ++result;
               if (crt_row == end_row)  /* all rows are read */
               {
                  ch = getc(stream);  /* check for the end of bitmap */
                  if (ch != EOF && ch != 0)
                  {
                     ungetc(ch, stream);  /* forget about the end of bitmap */
                     break;
                  }
                  getc(stream);  /* expect 1, but break the loop anyway */
                  break;
               }
            }
            else if (b2 == 1)  /* end of bitmap */
            {
               bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
               crt_row += inc;
               result = (begin_row <= end_row) ?
                  (end_row - begin_row) : (begin_row - end_row);
               break;  /* the rest is wiped out at the end */
            }
            else if (b2 == 2)  /* delta */
            {
               ch = getc(stream); b1 = (unsigned int)ch;  /* horiz. offset */
               ch = getc(stream); b2 = (unsigned int)ch;  /* vert. offset */
               if (ch == EOF)
                  break;
               dcrtn = (b1 < endn - crtn) ? (crtn + b1) : endn;
               if (b2 > (size_t)((end_row - crt_row) * inc))
                  b2 = (unsigned int)((end_row - crt_row) * inc);
               for ( ; b2 > 0; --b2)
               {
                  bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
                  crt_row += inc;
                  crtn = 0;
                  ++result;
               }
               bmp_memset_fn(*crt_row, crtn, 0, dcrtn - crtn);
            }
            else  /* b2 >= 3 bytes in absolute mode */
            {
               len = (b2 <= endn - crtn) ? b2 : (unsigned int)(endn - crtn);
               if (bmp_fread_fn(*crt_row, crtn, len, stream) != len)
                  break;
               crtn += len;
            }
         }
         else  /* b1 > 0 bytes in run-length encoded mode */
         {
            len = (b1 <= endn - crtn) ? b1 : (unsigned int)(endn - crtn);
            bmp_memset_fn(*crt_row, crtn, (int)b2, len);
            crtn += len;
         }
      }
   }
   else
      return 0;  /* error: compression method not applicable. */

   /* Wipe out the portion left unread. */
   for ( ; crt_row != end_row; crt_row += inc)
   {
      bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
      crtn = 0;
   }

   return result;
}


/*****************************************************************************/
/* BMP-to-PNG sample conversion                                              */
/*****************************************************************************/

static void
bmp_to_png_rows(png_bytepp row_pointers,
                png_uint_32 width, png_uint_32 height, unsigned int pixdepth,
                png_bytep rgba_sig, png_bytep rgba_shift)
{
   png_bytep src_ptr, dest_ptr;
   unsigned int rgba_mask[4];
   unsigned int num_samples, sample, mask;
   unsigned int wpix;
   png_uint_32 dwpix;
   png_uint_32 x, y;
   unsigned int i;

   if (pixdepth == 24)  /* BGR -> RGB */
   {
      for (y = 0; y < height; ++y)
      {
         src_ptr = row_pointers[y];
         for (x = 0; x < width; ++x, src_ptr += 3)
         {
            png_byte tmp = src_ptr[0];
            src_ptr[0] = src_ptr[2];
            src_ptr[2] = tmp;
         }
      }
      return;
   }

   num_samples = (rgba_sig[3] != 0) ? 4 : 3;
   for (i = 0; i < num_samples; ++i)
      rgba_mask[i] = (1U << rgba_sig[i]) - 1;

   if (pixdepth == 16)
   {
      for (y = 0; y < height; ++y)
      {
         src_ptr = row_pointers[y] + (width - 1) * 2;
         dest_ptr = row_pointers[y] + (width - 1) * num_samples;
         for (x = 0; x < width; ++x, src_ptr -= 2, dest_ptr -= num_samples)
         {
            /* Inline bmp_get_word() for performance reasons. */
            wpix = (unsigned int)src_ptr[0] + ((unsigned int)src_ptr[1] << 8);
            for (i = 0; i < num_samples; ++i)
            {
               mask = rgba_mask[i];
               sample = (wpix >> rgba_shift[i]) & mask;
               dest_ptr[i] = (png_byte)((sample * 255 + mask / 2) / mask);
            }
         }
      }
   }
   else if (pixdepth == 32)
   {
      for (y = 0; y < height; ++y)
      {
         src_ptr = dest_ptr = row_pointers[y];
         for (x = 0; x < width; ++x, src_ptr += 4, dest_ptr += num_samples)
         {
            /* Inline bmp_get_dword() for performance reasons. */
            dwpix = (png_uint_32)src_ptr[0] + ((png_uint_32)src_ptr[1] << 8) +
            ((png_uint_32)src_ptr[2] << 16) + ((png_uint_32)src_ptr[3] << 24);
            for (i = 0; i < num_samples; ++i)
            {
               mask = rgba_mask[i];
               sample = (dwpix >> rgba_shift[i]) & mask;
               dest_ptr[i] = (png_byte)((sample * 255 + mask / 2) / mask);
            }
         }
      }
   }
   /* else do nothing */
}


/*****************************************************************************/
/* BMP read support for pngxtern                                             */
/*****************************************************************************/

int /* PRIVATE */
pngx_sig_is_bmp(png_bytep sig, size_t sig_size,
                png_const_charpp fmt_name_ptr,
                png_const_charpp fmt_long_name_ptr)
{
   static const char bmp_fmt_name[] = "BMP";
   static const char os2bmp_fmt_long_name[] = "OS/2 Bitmap";
   static const char winbmp_fmt_long_name[] = "Windows Bitmap";
   png_uint_32 bihsize;

   /* Require at least the bitmap file header and the subsequent 4 bytes. */
   if (sig_size < FILEHED_SIZE + 4)
      return -1;  /* insufficient data */
   if (bmp_get_word(sig) != BMP_SIGNATURE)
      return 0;  /* not BMP */
   /* Avoid using bfhsize because it is not reliable. */
   bihsize = bmp_get_dword(sig + FILEHED_SIZE);
   if ((bihsize > PNG_UINT_31_MAX) ||
       (bihsize != COREHED_SIZE && bihsize < INFOHED_SIZE))
      return 0;  /* garbage in bihsize, this cannot be BMP */

   /* Store the format name. */
   if (fmt_name_ptr != NULL)
      *fmt_name_ptr = bmp_fmt_name;
   if (fmt_long_name_ptr != NULL)
   {
      if (bihsize == COREHED_SIZE)
         *fmt_long_name_ptr = os2bmp_fmt_long_name;
      else
         *fmt_long_name_ptr = winbmp_fmt_long_name;
   }
   return 1;  /* BMP */
}

int /* PRIVATE */
pngx_read_bmp(png_structp png_ptr, png_infop info_ptr, FILE *stream)
{
   png_byte bfh[FILEHED_SIZE + BMPV5HED_SIZE];
   png_bytep const bih = bfh + FILEHED_SIZE;
   png_byte rgbq[RGBQUAD_SIZE];
   png_uint_32 offbits, bihsize, skip;
   png_uint_32 width, height, rowsize;
   int topdown;
   unsigned int pixdepth;
   png_uint_32 compression;
   unsigned int palsize, palnum;
   png_uint_32 rgba_mask[4];
   png_byte rgba_sig[4], rgba_shift[4];
   int bit_depth, color_type;
   png_color palette[256];
   png_color_8 sig_bit;
   png_bytepp row_pointers, begin_row, end_row;
   unsigned int i;
   size_t y;

   /* Find the BMP header. */
   for (i = 0; ; ++i)  /* skip macbinary header */
   {
      if (fread(bfh, FILEHED_SIZE + BIHSIZE_SIZE, 1, stream) != 1)
         ++i;
      else if (bmp_get_word(bfh + BFH_WTYPE) == BMP_SIGNATURE)
         break;
      if (fread(bfh, 128 - FILEHED_SIZE - BIHSIZE_SIZE, 1, stream) != 1)
         ++i;
      if (i > 0)
         return 0;  /* not a BMP file */
   }

   /* Read the BMP header. */
   offbits = bmp_get_dword(bfh + BFH_DOFFBITS);
   bihsize = bmp_get_dword(bfh + BFH_DBIHSIZE);
   if ((offbits > PNG_UINT_31_MAX) || (bihsize > PNG_UINT_31_MAX) ||
       (offbits < bihsize + FILEHED_SIZE) ||
       (bihsize != COREHED_SIZE && bihsize < INFOHED_SIZE))
      return 0;  /* not a BMP file, just a file with a matching signature */
   if (bihsize > BMPV5HED_SIZE)
   {
      skip = bihsize - BMPV5HED_SIZE;
      bihsize = BMPV5HED_SIZE;
   }
   else
      skip = 0;
   if (fread(bih + BIHSIZE_SIZE, bihsize - BIHSIZE_SIZE, 1, stream) != 1)
      return 0;
   if (skip > 0)
      if (fseek(stream, (long)skip, SEEK_CUR) != 0)
         return 0;
   skip = offbits - bihsize - FILEHED_SIZE;  /* new skip */
   topdown = 0;
   if (bihsize < INFOHED_SIZE)  /* OS/2 BMP */
   {
      width       = bmp_get_word(bih + BCH_WWIDTH);
      height      = bmp_get_word(bih + BCH_WHEIGHT);
      pixdepth    = bmp_get_word(bih + BCH_WBITCOUNT);
      compression = BI_RGB;
      palsize     = RGBTRIPLE_SIZE;
   }
   else  /* Windows BMP */
   {
      width       = bmp_get_dword(bih + BIH_LWIDTH);
      height      = bmp_get_dword(bih + BIH_LHEIGHT);
      pixdepth    = bmp_get_word(bih + BIH_WBITCOUNT);
      compression = bmp_get_dword(bih + BIH_DCOMPRESSION);
      palsize     = RGBQUAD_SIZE;
      if (height > PNG_UINT_31_MAX)  /* top-down BMP */
      {
         height  = PNG_UINT_32_MAX - height + 1;
         topdown = 1;
      }
      if (bihsize == INFOHED_SIZE && compression == BI_BITFIELDS)
      {
         /* Read the RGB[A] mask. */
         i = (skip <= 16) ? (unsigned int)skip : 16;
         if (fread(bih + B4H_DREDMASK, i, 1, stream) != 1)
            return 0;
         bihsize += i;
         skip -= i;
      }
   }

   memset(rgba_mask, 0, sizeof(rgba_mask));
   if (pixdepth > 8)
   {
      if (compression == BI_RGB)
      {
         if (pixdepth == 16)
         {
            compression  = BI_BITFIELDS;
            rgba_mask[0] = 0x7c00;
            rgba_mask[1] = 0x03e0;
            rgba_mask[2] = 0x001f;
         }
         else /* pixdepth == 24 || pixdepth == 32 */
         {
            rgba_mask[0] = (png_uint_32)0x00ff0000L;
            rgba_mask[1] = (png_uint_32)0x0000ff00L;
            rgba_mask[2] = (png_uint_32)0x000000ffL;
         }
      }
      else if (compression == BI_BITFIELDS)
      {
         if (bihsize >= INFOHED_SIZE + 12)
         {
            rgba_mask[0] = bmp_get_dword(bih + B4H_DREDMASK);
            rgba_mask[1] = bmp_get_dword(bih + B4H_DGREENMASK);
            rgba_mask[2] = bmp_get_dword(bih + B4H_DBLUEMASK);
         }
         else
            png_error(png_ptr, "Missing color mask in BMP file");
      }
      if (bihsize >= INFOHED_SIZE + 16)
         rgba_mask[3] = bmp_get_dword(bih + B4H_DALPHAMASK);
   }

   switch (compression)
   {
   case BI_RGB:
      /* Allow pixel depth values 1, 2, 4, 8, 16, 24, 32.
       * (Although the BMP spec does not mention pixel depth = 2,
       * it is supported for robustness reasons, at no extra cost.)
       */
      if (pixdepth > 0 && 32 % pixdepth != 0 && pixdepth != 24)
         pixdepth = 0;
      break;
   case BI_RLE8:
      if (pixdepth != 8)
         pixdepth = 0;
      break;
   case BI_RLE4:
      if (pixdepth != 4)
         pixdepth = 0;
      break;
   case BI_BITFIELDS:
      if (pixdepth != 16 && pixdepth != 32)
         pixdepth = 0;
      break;
   case BI_JPEG:
      png_error(png_ptr, "JPEG-compressed BMP files are not supported");
      /* NOTREACHED */
      break;
   case BI_PNG:
      if (ungetc(getc(stream), stream) == 0)  /* IHDR is likely to follow */
         png_set_sig_bytes(png_ptr, 8);
      png_set_read_fn(png_ptr, stream, NULL);
      png_read_png(png_ptr, info_ptr, 0, NULL);
      /* TODO: Check for mismatches between the BMP and PNG info. */
      return 1;
   default:
      png_error(png_ptr, "Unsupported compression method in BMP file");
   }

   /* Check the BMP image parameters. */
   if (width == 0 || width > PNG_UINT_31_MAX || height == 0)
      png_error(png_ptr, "Invalid image dimensions in BMP file");
   if (pixdepth == 0)
      png_error(png_ptr, "Invalid pixel depth in BMP file");

   /* Compute the PNG image parameters. */
   if (pixdepth <= 8)
   {
      palnum = skip / palsize;
      if (palnum > 256)
         palnum = 256;
      skip -= palsize * palnum;
      rowsize = (width + (32 / pixdepth) - 1) / (32 / pixdepth) * 4;
      /* rowsize becomes 0 on overflow. */
      bit_depth = pixdepth;
      color_type = (palnum > 0) ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_GRAY;
   }
   else
   {
      palnum = 0;
      bit_depth = 8;
      switch (pixdepth)
      {
      case 16:
         rowsize = (width * 2 + 3) & (~3);
         break;
      case 24:
         rowsize = (width * 3 + 3) & (~3);
         break;
      case 32:
         rowsize = width * 4;
         break;
      default:  /* never get here */
         bit_depth = 0;
         rowsize = 0;
      }
      if (rowsize / width < pixdepth / 8)
         rowsize = 0;  /* overflow */
      color_type = (rgba_mask[3] != 0) ?
         PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
   }
   if (rowsize == 0)
      png_error(png_ptr, "Exceedingly large image dimensions in BMP file");

   /* Set the PNG image type. */
   png_set_IHDR(png_ptr, info_ptr,
      width, height, bit_depth, color_type,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   if (pixdepth > 8)
   {
      for (i = 0; i < 4; ++i)
         bmp_process_mask(rgba_mask[i], &rgba_sig[i], &rgba_shift[i]);
      if (rgba_sig[0] == 0 || rgba_sig[1] == 0 || rgba_sig[2] == 0)
         png_error(png_ptr, "Invalid color mask in BMP file");
      if (rgba_sig[0] != 8 || rgba_sig[1] != 8 ||
          rgba_sig[2] != 8 || (rgba_sig[3] != 0 && rgba_sig[3] != 8))
      {
         sig_bit.red   = rgba_sig[0];
         sig_bit.green = rgba_sig[1];
         sig_bit.blue  = rgba_sig[2];
         sig_bit.alpha = rgba_sig[3];
         png_set_sBIT(png_ptr, info_ptr, &sig_bit);
      }
   }

   /* Read the color palette (if applicable). */
   if (palnum > 0)
   {
      for (i = 0; i < palnum; ++i)
      {
         if (fread(rgbq, palsize, 1, stream) != 1)
            break;
         palette[i].red   = rgbq[RGB_RED];
         palette[i].green = rgbq[RGB_GREEN];
         palette[i].blue  = rgbq[RGB_BLUE];
      }
      png_set_PLTE(png_ptr, info_ptr, palette, i);
      if (i != palnum)
         png_error(png_ptr, "Error reading color palette in BMP file");
   }

   /* Allocate memory and read the image data. */
   row_pointers = pngx_malloc_rows_extended(png_ptr, info_ptr, rowsize, -1);
   if (topdown)
   {
      begin_row = row_pointers;
      end_row = row_pointers + height;
   }
   else
   {
      begin_row = row_pointers + height - 1;
      end_row = row_pointers - 1;
   }
   if (skip > 0)
      fseek(stream, (long)skip, SEEK_CUR);
   y = bmp_read_rows(begin_row, end_row, rowsize, compression, stream);

   /* Postprocess the image data, even if it has not been read entirely. */
   if (pixdepth > 8)
      bmp_to_png_rows(row_pointers, width, height, pixdepth,
         rgba_sig, rgba_shift);

   /* Check the result. */
   if (y != (size_t)height)
      png_error(png_ptr, "Error reading BMP file");

   return 1;  /* one image has been successfully read */
}
