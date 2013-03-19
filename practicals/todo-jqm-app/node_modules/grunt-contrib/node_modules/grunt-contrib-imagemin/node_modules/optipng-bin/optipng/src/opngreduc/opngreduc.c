/*
 * opngreduc.c - libpng extension: lossless image reductions.
 *
 * Copyright (C) 2003-2012 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 */

/* CAUTION:
 * Image reductions do not work well under certain transformations.
 *
 * Transformations like PNG_BGR, PNG_SWAP_BYTES, PNG_FILLER, PNG_INVERT_ALPHA,
 * and possibly others, require special treatment. However, the libpng API
 * does not currently convey the effect of transformations on its internal
 * state or on the layout of pixel data.
 *
 * Transformations which affect pixel depth (e.g. PNG_FILLER) are especially
 * dangerous when used in conjunction with this code, and should be avoided.
 */

#include "opngreduc.h"

#ifndef OPNG_ASSERT
#include <assert.h>
#define OPNG_ASSERT(cond) assert(cond)
#define OPNG_ASSERT_MSG(cond, msg) assert(cond)
#endif

#ifdef png_debug
#define opng_debug(level, msg) png_debug(level, msg)
#else
#define opng_debug(level, msg) ((void)0)
#endif


#ifdef PNG_INFO_IMAGE_SUPPORTED

/*
 * Check if the image information is valid.
 * The image information is said to be valid if all the required
 * critical chunk data is present in the png structures.
 * The function returns 1 if this information is valid, and 0 otherwise.
 */
int PNGAPI
opng_validate_image(png_structp png_ptr, png_infop info_ptr)
{
   opng_debug(1, "in opng_validate_image");

   /* Validate IHDR. */
   if (png_get_bit_depth(png_ptr, info_ptr) == 0)
      return 0;

   /* Validate PLTE. */
   if (png_get_color_type(png_ptr, info_ptr) & PNG_COLOR_MASK_PALETTE)
   {
      if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
         return 0;
   }

   /* Validate IDAT. */
   if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_IDAT))
      return 0;

   return 1;
}

#endif /* PNG_INFO_IMAGE_SUPPORTED */


#ifdef OPNG_IMAGE_REDUCTIONS_SUPPORTED

#define OPNG_CMP_RGB(R1, G1, B1, R2, G2, B2) \
   (((int)(R1) != (int)(R2)) ?      \
      ((int)(R1) - (int)(R2)) :     \
      (((int)(G1) != (int)(G2)) ?   \
         ((int)(G1) - (int)(G2)) :  \
         ((int)(B1) - (int)(B2))))

#define OPNG_CMP_ARGB(A1, R1, G1, B1, A2, R2, G2, B2) \
   (((int)(A1) != (int)(A2)) ?          \
      ((int)(A1) - (int)(A2)) :         \
      (((int)(R1) != (R2)) ?            \
         ((int)(R1) - (int)(R2)) :      \
         (((int)(G1) != (int)(G2)) ?    \
            ((int)(G1) - (int)(G2)) :   \
            ((int)(B1) - (int)(B2)))))

/*
 * Build a color+alpha palette in which the entries are sorted by
 * (alpha, red, green, blue), in this particular order.
 * Use the insertion sort algorithm.
 * The alpha value is ignored if it is not in the range [0 .. 255].
 * The function returns:
 *   1 if the insertion is successful;  *index = position of new entry.
 *   0 if the insertion is unnecessary; *index = position of crt entry.
 *  -1 if overflow;            *num_palette = *num_trans = *index = -1.
 */
static int /* PRIVATE */
opng_insert_palette_entry(png_colorp palette, int *num_palette,
   png_bytep trans_alpha, int *num_trans, int max_tuples,
   unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha,
   int *index)
{
   int low, high, mid, cmp;
   int i;

   OPNG_ASSERT(*num_palette >= 0 && *num_palette <= max_tuples);
   OPNG_ASSERT(*num_trans >= 0 && *num_trans <= *num_palette);

   if (alpha < 255)
   {
      /* Do a binary search among transparent tuples. */
      low = 0;
      high = *num_trans - 1;
      while (low <= high)
      {
         mid = (low + high) / 2;
         cmp = OPNG_CMP_ARGB(alpha, red, green, blue,
            trans_alpha[mid],
            palette[mid].red, palette[mid].green, palette[mid].blue);
         if (cmp < 0)
            high = mid - 1;
         else if (cmp > 0)
            low = mid + 1;
         else
         {
            *index = mid;
            return 0;
         }
      }
   }
   else  /* alpha == 255 || alpha not in [0 .. 255] */
   {
      /* Do a (faster) binary search among opaque tuples. */
      low = *num_trans;
      high = *num_palette - 1;
      while (low <= high)
      {
         mid = (low + high) / 2;
         cmp = OPNG_CMP_RGB(red, green, blue,
            palette[mid].red, palette[mid].green, palette[mid].blue);
         if (cmp < 0)
            high = mid - 1;
         else if (cmp > 0)
            low = mid + 1;
         else
         {
            *index = mid;
            return 0;
         }
      }
   }
   if (alpha > 255)
   {
      /* The binary search among opaque tuples has failed. */
      /* Do a linear search among transparent tuples, ignoring alpha. */
      for (i = 0; i < *num_trans; ++i)
      {
         cmp = OPNG_CMP_RGB(red, green, blue,
            palette[i].red, palette[i].green, palette[i].blue);
         if (cmp == 0)
         {
            *index = i;
            return 0;
         }
      }
   }

   /* Check for overflow. */
   if (*num_palette >= max_tuples)
   {
      *num_palette = *num_trans = *index = -1;
      return -1;
   }

   /* Insert new tuple at [low]. */
   OPNG_ASSERT(low >= 0 && low <= *num_palette);
   for (i = *num_palette; i > low; --i)
      palette[i] = palette[i - 1];
   palette[low].red   = (png_byte)red;
   palette[low].green = (png_byte)green;
   palette[low].blue  = (png_byte)blue;
   ++(*num_palette);
   if (alpha < 255)
   {
      OPNG_ASSERT(low <= *num_trans);
      for (i = *num_trans; i > low; --i)
         trans_alpha[i] = trans_alpha[i - 1];
      trans_alpha[low] = (png_byte)alpha;
      ++(*num_trans);
   }
   *index = low;
   return 1;
}

/*
 * Change the size of the palette buffer.
 * Changing info_ptr->num_palette directly, avoiding reallocation, should
 * have been sufficient, but can't be done using the current libpng API.
 */
static void /* PRIVATE */
opng_realloc_PLTE(png_structp png_ptr, png_infop info_ptr, int num_palette)
{
   png_color buffer[PNG_MAX_PALETTE_LENGTH];
   png_colorp palette;
   int src_num_palette;

   opng_debug(1, "in opng_realloc_PLTE");

   OPNG_ASSERT(num_palette > 0);
   src_num_palette = 0;
   png_get_PLTE(png_ptr, info_ptr, &palette, &src_num_palette);
   if (num_palette == src_num_palette)
      return;
   memcpy(buffer, palette, num_palette * sizeof(png_color));
   if (num_palette > src_num_palette)
      memset(buffer + src_num_palette, 0,
         (num_palette - src_num_palette) * sizeof(png_color));
   png_set_PLTE(png_ptr, info_ptr, buffer, num_palette);
}

/*
 * Change the size of the transparency buffer.
 * Changing info_ptr->num_trans directly, avoiding reallocation, should
 * have been sufficient, but can't be done using the current libpng API.
 */
static void /* PRIVATE */
opng_realloc_tRNS(png_structp png_ptr, png_infop info_ptr, int num_trans)
{
   png_byte buffer[PNG_MAX_PALETTE_LENGTH];
   png_bytep trans_alpha;
   int src_num_trans;

   opng_debug(1, "in opng_realloc_tRNS");

   OPNG_ASSERT(num_trans > 0);  /* tRNS should be invalidated in this case */
   src_num_trans = 0;
   png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &src_num_trans, NULL);
   if (num_trans == src_num_trans)
      return;
   memcpy(buffer, trans_alpha, (size_t)num_trans);
   if (num_trans > src_num_trans)
      memset(buffer + src_num_trans, 0, num_trans - src_num_trans);
   png_set_tRNS(png_ptr, info_ptr, buffer, num_trans, NULL);
}

/*
 * Retrieve the alpha samples from the given image row.
 */
static void /* PRIVATE */
opng_get_alpha_row(png_row_infop row_info_ptr, png_color_16p trans_color,
   png_bytep row, png_bytep alpha_row)
{
   png_bytep sample_ptr;
   png_uint_32 width;
   int color_type, bit_depth, channels;
   png_byte trans_red, trans_green, trans_blue, trans_gray;
   png_uint_32 i;

   width = row_info_ptr->width;
   color_type = row_info_ptr->color_type;
   bit_depth = row_info_ptr->bit_depth;
   channels = row_info_ptr->channels;

   OPNG_ASSERT(!(color_type & PNG_COLOR_MASK_PALETTE));
   OPNG_ASSERT(bit_depth == 8);

   if (!(color_type & PNG_COLOR_MASK_ALPHA))
   {
      if (trans_color == NULL)
      {
         /* All pixels are fully opaque. */
         memset(alpha_row, 255, (size_t)width);
         return;
      }
      if (color_type == PNG_COLOR_TYPE_RGB)
      {
         OPNG_ASSERT(channels == 3);
         trans_red   = (png_byte)trans_color->red;
         trans_green = (png_byte)trans_color->green;
         trans_blue  = (png_byte)trans_color->blue;
         sample_ptr = row;
         for (i = 0; i < width; ++i, sample_ptr += 3)
            alpha_row[i] = (png_byte)
               ((sample_ptr[0] == trans_red &&
                 sample_ptr[1] == trans_green &&
                 sample_ptr[2] == trans_blue) ? 0 : 255);
      }
      else
      {
         OPNG_ASSERT(color_type == PNG_COLOR_TYPE_GRAY);
         OPNG_ASSERT(channels == 1);
         trans_gray = (png_byte)trans_color->gray;
         for (i = 0; i < width; ++i)
            alpha_row[i] = (png_byte)((row[i] == trans_gray) ? 0 : 255);
      }
      return;
   }

   /* There is a real alpha channel. The alpha sample is last in RGBA tuple. */
   OPNG_ASSERT(channels > 1);
   sample_ptr = row + (channels - 1);
   for (i = 0; i < width; ++i, sample_ptr += channels, ++alpha_row)
      *alpha_row = *sample_ptr;
}

/*
 * Analyze the redundancy of bits inside the image.
 * The parameter reductions indicates the intended reductions.
 * The function returns the possible reductions.
 */
static png_uint_32 /* PRIVATE */
opng_analyze_bits(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_bytepp row_ptr;
   png_bytep component_ptr;
   png_uint_32 height, width;
   int bit_depth, color_type, byte_depth, channels, sample_size, offset_alpha;
#ifdef PNG_bKGD_SUPPORTED
   png_color_16p background;
#endif
   png_uint_32 i, j;

   opng_debug(1, "in opng_analyze_bits");

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL);
   if (bit_depth < 8)
      return OPNG_REDUCE_NONE;  /* not applicable */
   if (color_type & PNG_COLOR_MASK_PALETTE)
      return OPNG_REDUCE_NONE;  /* let opng_reduce_palette() handle it */

   byte_depth = bit_depth / 8;
   channels = png_get_channels(png_ptr, info_ptr);
   sample_size = channels * byte_depth;
   offset_alpha = (channels - 1) * byte_depth;

   /* Select the applicable reductions. */
   reductions &= (OPNG_REDUCE_16_TO_8 |
      OPNG_REDUCE_RGB_TO_GRAY | OPNG_REDUCE_STRIP_ALPHA);
   if (bit_depth <= 8)
      reductions &= ~OPNG_REDUCE_16_TO_8;
   if (!(color_type & PNG_COLOR_MASK_COLOR))
      reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
   if (!(color_type & PNG_COLOR_MASK_ALPHA))
      reductions &= ~OPNG_REDUCE_STRIP_ALPHA;

   /* Check if the ancillary information allows these reductions. */
#ifdef PNG_bKGD_SUPPORTED
   if (png_get_bKGD(png_ptr, info_ptr, &background))
   {
      if (reductions & OPNG_REDUCE_16_TO_8)
      {
         if (background->red   % 257 != 0 ||
             background->green % 257 != 0 ||
             background->blue  % 257 != 0 ||
             background->gray  % 257 != 0)
            reductions &= ~OPNG_REDUCE_16_TO_8;
      }
      if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
      {
         if (background->red != background->green ||
             background->red != background->blue)
            reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
      }
   }
#endif

   /* Check for each possible reduction, row by row. */
   row_ptr = png_get_rows(png_ptr, info_ptr);
   for (i = 0; i < height; ++i, ++row_ptr)
   {
      if (reductions == OPNG_REDUCE_NONE)
         return OPNG_REDUCE_NONE;  /* no need to go any further */

      /* Check if it is possible to reduce the bit depth to 8. */
      if (reductions & OPNG_REDUCE_16_TO_8)
      {
         component_ptr = *row_ptr;
         for (j = 0; j < channels * width; ++j, component_ptr += 2)
         {
            if (component_ptr[0] != component_ptr[1])
            {
               reductions &= ~OPNG_REDUCE_16_TO_8;
               break;
            }
         }
      }

      if (bit_depth == 8)
      {
         /* Check if it is possible to reduce rgb --> gray. */
         if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
         {
            component_ptr = *row_ptr;
            for (j = 0; j < width; ++j, component_ptr += sample_size)
            {
               if (component_ptr[0] != component_ptr[1] ||
                   component_ptr[0] != component_ptr[2])
               {
                  reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
                  break;
               }
            }
         }

         /* Check if it is possible to strip the alpha channel. */
         if (reductions & OPNG_REDUCE_STRIP_ALPHA)
         {
            component_ptr = *row_ptr + offset_alpha;
            for (j = 0; j < width; ++j, component_ptr += sample_size)
            {
               if (component_ptr[0] != 255)
               {
                  reductions &= ~OPNG_REDUCE_STRIP_ALPHA;
                  break;
               }
            }
         }
      }
      else  /* bit_depth == 16 */
      {
         /* Check if it is possible to reduce rgb --> gray. */
         if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
         {
            component_ptr = *row_ptr;
            for (j = 0; j < width; ++j, component_ptr += sample_size)
            {
               if (component_ptr[0] != component_ptr[2] ||
                   component_ptr[0] != component_ptr[4] ||
                   component_ptr[1] != component_ptr[3] ||
                   component_ptr[1] != component_ptr[5])
               {
                  reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
                  break;
               }
            }
         }

         /* Check if it is possible to strip the alpha channel. */
         if (reductions & OPNG_REDUCE_STRIP_ALPHA)
         {
            component_ptr = *row_ptr + offset_alpha;
            for (j = 0; j < width; ++j, component_ptr += sample_size)
            {
               if (component_ptr[0] != 255 || component_ptr[1] != 255)
               {
                  reductions &= ~OPNG_REDUCE_STRIP_ALPHA;
                  break;
               }
            }
         }
      }
   }

   return reductions;
}

/*
 * Reduce the image type to a lower bit depth and color type,
 * by removing redundant bits.
 * Possible reductions: 16bpp to 8bpp; RGB to gray; strip alpha.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 * All reductions are performed in a single step.
 */
static png_uint_32 /* PRIVATE */
opng_reduce_bits(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_bytepp row_ptr;
   png_bytep src_ptr, dest_ptr;
   png_uint_32 width, height;
   int interlace_type, compression_type, filter_type;
   int src_bit_depth, dest_bit_depth;
   int src_byte_depth, dest_byte_depth;
   int src_color_type, dest_color_type;
   int src_channels, dest_channels;
   int src_sample_size, dest_sample_size;
   int tran_tbl[8];
   png_color_16p trans_color;
#ifdef PNG_bKGD_SUPPORTED
   png_color_16p background;
#endif
#ifdef PNG_sBIT_SUPPORTED
   png_color_8p sig_bits;
#endif
   png_uint_32 i, j;
   int k;

   opng_debug(1, "in opng_reduce_bits");

   /* See which reductions may be performed. */
   reductions = opng_analyze_bits(png_ptr, info_ptr, reductions);
   if (reductions == OPNG_REDUCE_NONE)
      return OPNG_REDUCE_NONE;  /* exit early */

   png_get_IHDR(png_ptr, info_ptr, &width, &height,
      &src_bit_depth, &src_color_type,
      &interlace_type, &compression_type, &filter_type);

   /* Compute the new image parameters bit_depth, color_type, etc. */
   OPNG_ASSERT(src_bit_depth >= 8);
   if (reductions & OPNG_REDUCE_16_TO_8)
   {
      OPNG_ASSERT(src_bit_depth == 16);
      dest_bit_depth = 8;
   }
   else
      dest_bit_depth = src_bit_depth;

   src_byte_depth = src_bit_depth / 8;
   dest_byte_depth = dest_bit_depth / 8;

   dest_color_type = src_color_type;
   if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
   {
      OPNG_ASSERT(src_color_type & PNG_COLOR_MASK_COLOR);
      dest_color_type &= ~PNG_COLOR_MASK_COLOR;
   }
   if (reductions & OPNG_REDUCE_STRIP_ALPHA)
   {
      OPNG_ASSERT(src_color_type & PNG_COLOR_MASK_ALPHA);
      dest_color_type &= ~PNG_COLOR_MASK_ALPHA;
   }

   src_channels = png_get_channels(png_ptr, info_ptr);
   dest_channels =
      ((dest_color_type & PNG_COLOR_MASK_COLOR) ? 3 : 1) +
      ((dest_color_type & PNG_COLOR_MASK_ALPHA) ? 1 : 0);

   src_sample_size = src_channels * src_byte_depth;
   dest_sample_size = dest_channels * dest_byte_depth;

   /* Pre-compute the intra-sample translation table. */
   for (k = 0; k < 4 * dest_byte_depth; ++k)
      tran_tbl[k] = k * src_bit_depth / dest_bit_depth;
   /* If rgb --> gray, shift the alpha component two positions to the left. */
   if ((reductions & OPNG_REDUCE_RGB_TO_GRAY) &&
       (dest_color_type & PNG_COLOR_MASK_ALPHA))
   {
      tran_tbl[dest_byte_depth] = tran_tbl[3 * dest_byte_depth];
      if (dest_byte_depth == 2)
         tran_tbl[dest_byte_depth + 1] = tran_tbl[3 * dest_byte_depth + 1];
   }

   /* Translate the samples to the new image type. */
   OPNG_ASSERT(src_sample_size > dest_sample_size);
   row_ptr = png_get_rows(png_ptr, info_ptr);
   for (i = 0; i < height; ++i, ++row_ptr)
   {
      src_ptr = dest_ptr = *row_ptr;
      for (j = 0; j < width; ++j)
      {
         for (k = 0; k < dest_sample_size; ++k)
            dest_ptr[k] = src_ptr[tran_tbl[k]];
         src_ptr += src_sample_size;
         dest_ptr += dest_sample_size;
      }
   }

   /* Update the ancillary information. */
   if (png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color))
   {
      if (reductions & OPNG_REDUCE_16_TO_8)
      {
         if (trans_color->red   % 257 == 0 &&
             trans_color->green % 257 == 0 &&
             trans_color->blue  % 257 == 0 &&
             trans_color->gray  % 257 == 0)
         {
            trans_color->red   &= 255;
            trans_color->green &= 255;
            trans_color->blue  &= 255;
            trans_color->gray  &= 255;
         }
         else
         {
            /* 16-bit tRNS in 8-bit samples: all pixels are 100% opaque. */
            png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
            png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
         }
      }
      if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
      {
         if (trans_color->red == trans_color->green ||
             trans_color->red == trans_color->blue)
            trans_color->gray = trans_color->red;
         else
         {
            /* Non-gray tRNS in grayscale image: all pixels are 100% opaque. */
            png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
            png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
         }
      }
   }
#ifdef PNG_bKGD_SUPPORTED
   if (png_get_bKGD(png_ptr, info_ptr, &background))
   {
      if (reductions & OPNG_REDUCE_16_TO_8)
      {
         background->red   &= 255;
         background->green &= 255;
         background->blue  &= 255;
         background->gray  &= 255;
      }
      if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
         background->gray = background->red;
   }
#endif
#ifdef PNG_sBIT_SUPPORTED
   if (png_get_sBIT(png_ptr, info_ptr, &sig_bits))
   {
      if (reductions & OPNG_REDUCE_16_TO_8)
      {
         if (sig_bits->red > 8)
            sig_bits->red = 8;
         if (sig_bits->green > 8)
            sig_bits->green = 8;
         if (sig_bits->blue > 8)
            sig_bits->blue = 8;
         if (sig_bits->gray > 8)
            sig_bits->gray = 8;
         if (sig_bits->alpha > 8)
            sig_bits->alpha = 8;
      }
      if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
      {
         png_byte max_sig_bits = sig_bits->red;
         if (max_sig_bits < sig_bits->green)
            max_sig_bits = sig_bits->green;
         if (max_sig_bits < sig_bits->blue)
            max_sig_bits = sig_bits->blue;
         sig_bits->gray = max_sig_bits;
      }
   }
#endif

   /* Update the image information. */
   png_set_IHDR(png_ptr, info_ptr, width, height,
      dest_bit_depth, dest_color_type,
      interlace_type, compression_type, filter_type);

   return reductions;
}

/*
 * Reduce the bit depth of a palette image to the lowest possible value.
 * The parameter reductions should contain OPNG_REDUCE_8_TO_4_2_1.
 * The function returns OPNG_REDUCE_8_TO_4_2_1 if successful.
 */
static png_uint_32 /* PRIVATE */
opng_reduce_palette_bits(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_bytepp row_ptr;
   png_bytep src_sample_ptr, dest_sample_ptr;
   png_uint_32 width, height;
   int color_type, interlace_type, compression_type, filter_type;
   int src_bit_depth, dest_bit_depth;
   unsigned int src_mask_init, src_mask, src_shift, dest_shift;
   unsigned int sample, dest_buf;
   png_colorp palette;
   int num_palette;
   png_uint_32 i, j;

   opng_debug(1, "in opng_reduce_palette_bits");

   /* Check if the reduction applies. */
   if (!(reductions & OPNG_REDUCE_8_TO_4_2_1))
      return OPNG_REDUCE_NONE;
   png_get_IHDR(png_ptr, info_ptr, &width, &height, &src_bit_depth,
      &color_type, &interlace_type, &compression_type, &filter_type);
   if (color_type != PNG_COLOR_TYPE_PALETTE)
      return OPNG_REDUCE_NONE;
   if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
      num_palette = 0;

   /* Find the smallest possible bit depth. */
   if (num_palette > 16)
      return OPNG_REDUCE_NONE;
   else if (num_palette > 4)  /* 5 .. 16 entries */
      dest_bit_depth = 4;
   else if (num_palette > 2)  /* 3 or 4 entries */
      dest_bit_depth = 2;
   else  /* 1 or 2 entries */
   {
      OPNG_ASSERT(num_palette > 0);
      dest_bit_depth = 1;
   }

   if (src_bit_depth <= dest_bit_depth)
   {
      OPNG_ASSERT(src_bit_depth == dest_bit_depth);
      return OPNG_REDUCE_NONE;
   }

   /* Iterate through all sample values. */
   row_ptr = png_get_rows(png_ptr, info_ptr);
   if (src_bit_depth == 8)
   {
      for (i = 0; i < height; ++i, ++row_ptr)
      {
         src_sample_ptr = dest_sample_ptr = *row_ptr;
         dest_shift = 8;
         dest_buf = 0;
         for (j = 0; j < width; ++j)
         {
            dest_shift -= dest_bit_depth;
            if (dest_shift > 0)
               dest_buf |= *src_sample_ptr << dest_shift;
            else
            {
               *dest_sample_ptr++ = (png_byte)(dest_buf | *src_sample_ptr);
               dest_shift = 8;
               dest_buf = 0;
            }
            ++src_sample_ptr;
         }
         if (dest_shift != 0)
            *dest_sample_ptr = (png_byte)dest_buf;
      }
   }
   else  /* src_bit_depth < 8 */
   {
      src_mask_init = (1 << (8 + src_bit_depth)) - (1 << 8);
      for (i = 0; i < height; ++i, ++row_ptr)
      {
         src_sample_ptr = dest_sample_ptr = *row_ptr;
         src_shift = dest_shift = 8;
         src_mask = src_mask_init;
         dest_buf = 0;
         for (j = 0; j < width; ++j)
         {
            src_shift -= src_bit_depth;
            src_mask >>= src_bit_depth;
            sample = (*src_sample_ptr & src_mask) >> src_shift;
            dest_shift -= dest_bit_depth;
            if (dest_shift > 0)
               dest_buf |= sample << dest_shift;
            else
            {
               *dest_sample_ptr++ = (png_byte)(dest_buf | sample);
               dest_shift = 8;
               dest_buf = 0;
            }
            if (src_shift == 0)
            {
               src_shift = 8;
               src_mask = src_mask_init;
               ++src_sample_ptr;
            }
         }
         if (dest_shift != 0)
            *dest_sample_ptr = (png_byte)dest_buf;
      }
   }

   /* Update the image information. */
   png_set_IHDR(png_ptr, info_ptr, width, height, dest_bit_depth,
      color_type, interlace_type, compression_type, filter_type);
   return OPNG_REDUCE_8_TO_4_2_1;
}

/*
 * Reduce the image type from grayscale(+alpha) or RGB(+alpha) to palette,
 * if possible.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
static png_uint_32 /* PRIVATE */
opng_reduce_to_palette(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_uint_32 result;
   png_row_info row_info;
   png_bytepp row_ptr;
   png_bytep sample_ptr, alpha_row;
   png_uint_32 height, width;
   int color_type, interlace_type, compression_type, filter_type;
   int src_bit_depth, dest_bit_depth, channels;
   png_color palette[256];
   png_byte trans_alpha[256];
   png_color_16p trans_color;
   int num_palette, num_trans, index;
   unsigned int gray, red, green, blue, alpha;
   unsigned int prev_gray, prev_red, prev_green, prev_blue, prev_alpha;
#ifdef PNG_bKGD_SUPPORTED
   png_color_16p background;
#endif
   png_uint_32 i, j;

   opng_debug(1, "in opng_reduce_to_palette");

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &src_bit_depth,
      &color_type, &interlace_type, &compression_type, &filter_type);
   if (src_bit_depth != 8)
      return OPNG_REDUCE_NONE;  /* nothing is done in this case */
   OPNG_ASSERT(!(color_type & PNG_COLOR_MASK_PALETTE));

   row_ptr = png_get_rows(png_ptr, info_ptr);
   channels = png_get_channels(png_ptr, info_ptr);
   alpha_row = (png_bytep)png_malloc(png_ptr, width);

   row_info.width = width;
   row_info.rowbytes = 0;  /* not used */
   row_info.color_type = (png_byte)color_type;
   row_info.bit_depth = (png_byte)src_bit_depth;
   row_info.channels = (png_byte)channels;
   row_info.pixel_depth = 0;  /* not used */

   /* Analyze the possibility of this reduction. */
   num_palette = num_trans = 0;
   trans_color = NULL;
   png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color);
   prev_gray = prev_red = prev_green = prev_blue = prev_alpha = 256;
   for (i = 0; i < height; ++i, ++row_ptr)
   {
      sample_ptr = *row_ptr;
      opng_get_alpha_row(&row_info, trans_color, *row_ptr, alpha_row);
      if (color_type & PNG_COLOR_MASK_COLOR)
      {
         for (j = 0; j < width; ++j, sample_ptr += channels)
         {
            red   = sample_ptr[0];
            green = sample_ptr[1];
            blue  = sample_ptr[2];
            alpha = alpha_row[j];
            /* Check the cache first. */
            if (red != prev_red || green != prev_green || blue != prev_blue ||
                alpha != prev_alpha)
            {
               prev_red   = red;
               prev_green = green;
               prev_blue  = blue;
               prev_alpha = alpha;
               if (opng_insert_palette_entry(palette, &num_palette,
                   trans_alpha, &num_trans, 256,
                   red, green, blue, alpha, &index) < 0)  /* overflow */
               {
                  OPNG_ASSERT(num_palette < 0);
                  i = height;  /* forced exit from outer loop */
                  break;
               }
            }
         }
      }
      else  /* grayscale */
      {
         for (j = 0; j < width; ++j, sample_ptr += channels)
         {
            gray  = sample_ptr[0];
            alpha = alpha_row[j];
            /* Check the cache first. */
            if (gray != prev_gray || alpha != prev_alpha)
            {
               prev_gray  = gray;
               prev_alpha = alpha;
               if (opng_insert_palette_entry(palette, &num_palette,
                   trans_alpha, &num_trans, 256,
                   gray, gray, gray, alpha, &index) < 0)  /* overflow */
               {
                  OPNG_ASSERT(num_palette < 0);
                  i = height;  /* forced exit from outer loop */
                  break;
               }
            }
         }
      }
   }
#ifdef PNG_bKGD_SUPPORTED
   if ((num_palette >= 0) && png_get_bKGD(png_ptr, info_ptr, &background))
   {
      /* bKGD has an alpha-agnostic palette entry. */
      if (color_type & PNG_COLOR_MASK_COLOR)
      {
         red   = background->red;
         green = background->green;
         blue  = background->blue;
      }
      else
         red = green = blue = background->gray;
      opng_insert_palette_entry(palette, &num_palette,
         trans_alpha, &num_trans, 256,
         red, green, blue, 256, &index);
      if (index >= 0)
         background->index = (png_byte)index;
   }
#endif

   /* Continue only if the uncompressed indexed image (pixels + PLTE + tRNS)
    * is smaller than the uncompressed RGB(A) image.
    * Casual overhead (headers, CRCs, etc.) is ignored.
    *
    * Compare:
    * num_pixels * (src_bit_depth * channels - dest_bit_depth) / 8
    * vs.
    * sizeof(PLTE) + sizeof(tRNS)
    */
   if (num_palette >= 0)
   {
      OPNG_ASSERT(num_palette > 0 && num_palette <= 256);
      OPNG_ASSERT(num_trans >= 0 && num_trans <= num_palette);
      if (num_palette <= 2)
         dest_bit_depth = 1;
      else if (num_palette <= 4)
         dest_bit_depth = 2;
      else if (num_palette <= 16)
         dest_bit_depth = 4;
      else
         dest_bit_depth = 8;
      /* Do the comparison in a way that does not cause overflow. */
      if (channels * 8 == dest_bit_depth ||
          (3 * num_palette + num_trans) * 8 / (channels * 8 - dest_bit_depth)
             / width / height >= 1)
         num_palette = -1;
   }

   if (num_palette < 0)  /* can't reduce */
   {
      png_free(png_ptr, alpha_row);
      return OPNG_REDUCE_NONE;
   }

   /* Reduce. */
   row_ptr = png_get_rows(png_ptr, info_ptr);
   index = -1;
   prev_red = prev_green = prev_blue = prev_alpha = (unsigned int)(-1);
   for (i = 0; i < height; ++i, ++row_ptr)
   {
      sample_ptr = *row_ptr;
      opng_get_alpha_row(&row_info, trans_color, *row_ptr, alpha_row);
      if (color_type & PNG_COLOR_MASK_COLOR)
      {
         for (j = 0; j < width; ++j, sample_ptr += channels)
         {
            red   = sample_ptr[0];
            green = sample_ptr[1];
            blue  = sample_ptr[2];
            alpha = alpha_row[j];
            /* Check the cache first. */
            if (red != prev_red || green != prev_green || blue != prev_blue ||
                alpha != prev_alpha)
            {
               prev_red   = red;
               prev_green = green;
               prev_blue  = blue;
               prev_alpha = alpha;
               if (opng_insert_palette_entry(palette, &num_palette,
                   trans_alpha, &num_trans, 256,
                   red, green, blue, alpha, &index) != 0)
                  index = -1;  /* this should not happen */
            }
            OPNG_ASSERT(index >= 0);
            (*row_ptr)[j] = (png_byte)index;
         }
      }
      else  /* grayscale */
      {
         for (j = 0; j < width; ++j, sample_ptr += channels)
         {
            gray  = sample_ptr[0];
            alpha = alpha_row[j];
            /* Check the cache first. */
            if (gray != prev_gray || alpha != prev_alpha)
            {
               prev_gray  = gray;
               prev_alpha = alpha;
               if (opng_insert_palette_entry(palette, &num_palette,
                   trans_alpha, &num_trans, 256,
                   gray, gray, gray, alpha, &index) != 0)
                  index = -1;  /* this should not happen */
            }
            OPNG_ASSERT(index >= 0);
            (*row_ptr)[j] = (png_byte)index;
         }
      }
   }

   /* Update the image information. */
   png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_PALETTE,
      interlace_type, compression_type, filter_type);
   png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
   if (num_trans > 0)
      png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans, NULL);
   /* bKGD (if present) is automatically updated. */

   png_free(png_ptr, alpha_row);

   result = OPNG_REDUCE_RGB_TO_PALETTE;
   if (reductions & OPNG_REDUCE_8_TO_4_2_1)
      result |= opng_reduce_palette_bits(png_ptr, info_ptr, reductions);
   return result;
}

/*
 * Analyze the usage of samples.
 * The output value usage_map[n] indicates whether the sample n
 * is used. The usage_map[] array must have 256 entries.
 * The function requires a valid bit depth between 1 and 8.
 */
static void /* PRIVATE */
opng_analyze_sample_usage(png_structp png_ptr, png_infop info_ptr,
   png_bytep usage_map)
{
   png_bytepp row_ptr;
   png_bytep sample_ptr;
   png_uint_32 width, height;
   int bit_depth, init_shift, init_mask, shift, mask;
#ifdef PNG_bKGD_SUPPORTED
   png_color_16p background;
#endif
   png_uint_32 i, j;

   opng_debug(1, "in opng_analyze_sample_usage");

   height = png_get_image_height(png_ptr, info_ptr);
   width = png_get_image_width(png_ptr, info_ptr);
   bit_depth = png_get_bit_depth(png_ptr, info_ptr);
   row_ptr = png_get_rows(png_ptr, info_ptr);

   /* Initialize the output entries with 0. */
   memset(usage_map, 0, 256);

   /* Iterate through all sample values. */
   if (bit_depth == 8)
   {
      for (i = 0; i < height; ++i, ++row_ptr)
      {
         for (j = 0, sample_ptr = *row_ptr; j < width; ++j, ++sample_ptr)
            usage_map[*sample_ptr] = 1;
      }
   }
   else
   {
      OPNG_ASSERT(bit_depth < 8);
      init_shift = 8 - bit_depth;
      init_mask = (1 << 8) - (1 << init_shift);
      for (i = 0; i < height; ++i, ++row_ptr)
      {
         for (j = 0, sample_ptr = *row_ptr; j < width; ++sample_ptr)
         {
            mask = init_mask;
            shift = init_shift;
            do
            {
               usage_map[(*sample_ptr & mask) >> shift] = 1;
               mask >>= bit_depth;
               shift -= bit_depth;
               ++j;
            } while (mask > 0 && j < width);
         }
      }
   }

#ifdef PNG_bKGD_SUPPORTED
   /* bKGD also counts as a used sample. */
   if (png_get_bKGD(png_ptr, info_ptr, &background))
      usage_map[background->index] = 1;
#endif
}

/*
 * Reduce the palette. (Only the fast method is implemented.)
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
static png_uint_32 /* PRIVATE */
opng_reduce_palette(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_uint_32 result;
   png_colorp palette;
   png_bytep trans_alpha;
   png_bytepp row_ptr;
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type, compression_type, filter_type;
   int num_palette, num_trans;
   int last_color_index, last_trans_index;
   png_byte crt_trans_value, last_trans_value;
   png_byte is_used[256];
   png_color_16 gray_trans;
   int is_gray;
#ifdef PNG_bKGD_SUPPORTED
   png_color_16p background;
#endif
#ifdef PNG_hIST_SUPPORTED
   png_uint_16p hist;
#endif
#ifdef PNG_sBIT_SUPPORTED
   png_color_8p sig_bits;
#endif
   png_uint_32 i, j;
   int k;

   opng_debug(1, "in opng_reduce_palette");

   result = OPNG_REDUCE_NONE;

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
      &color_type, &interlace_type, &compression_type, &filter_type);
   row_ptr = png_get_rows(png_ptr, info_ptr);
   if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
   {
      palette = NULL;
      num_palette = 0;
   }
   if (!png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, NULL))
   {
      trans_alpha = NULL;
      num_trans = 0;
   }
   else
      OPNG_ASSERT(trans_alpha != NULL && num_trans > 0);

   opng_analyze_sample_usage(png_ptr, info_ptr, is_used);
   /* Palette-to-gray does not work (yet) if the bit depth is below 8. */
   is_gray = (reductions & OPNG_REDUCE_PALETTE_TO_GRAY) && (bit_depth == 8);
   last_color_index = last_trans_index = -1;
   for (k = 0; k < 256; ++k)
   {
      if (!is_used[k])
         continue;
      last_color_index = k;
      if (k < num_trans && trans_alpha[k] < 255)
         last_trans_index = k;
      if (is_gray)
         if (palette[k].red != palette[k].green ||
             palette[k].red != palette[k].blue)
            is_gray = 0;
   }
   OPNG_ASSERT(last_color_index >= 0);
   OPNG_ASSERT(last_color_index >= last_trans_index);

   /* Check the integrity of PLTE and tRNS. */
   if (last_color_index >= num_palette)
   {
      png_warning(png_ptr, "Too few colors in PLTE");
      /* Fix the palette by adding blank entries at the end. */
      opng_realloc_PLTE(png_ptr, info_ptr, last_color_index + 1);
      png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
      OPNG_ASSERT(num_palette == last_color_index + 1);
      result |= OPNG_REDUCE_REPAIR;
   }
   if (num_trans > num_palette)
   {
      png_warning(png_ptr, "Too many alpha values in tRNS");
      /* Transparency will be fixed further below. */
      result |= OPNG_REDUCE_REPAIR;
   }

   /* Check if tRNS can be reduced to grayscale. */
   if (is_gray && last_trans_index >= 0)
   {
      gray_trans.gray = palette[last_trans_index].red;
      last_trans_value = trans_alpha[last_trans_index];
      for (k = 0; k <= last_color_index; ++k)
      {
         if (!is_used[k])
            continue;
         if (k <= last_trans_index)
         {
            crt_trans_value = trans_alpha[k];
            /* Cannot reduce if different colors have transparency. */
            if (crt_trans_value < 255 && palette[k].red != gray_trans.gray)
            {
               is_gray = 0;
               break;
            }
         }
         else
            crt_trans_value = 255;
         /* Cannot reduce if same color has multiple transparency levels. */
         if (palette[k].red == gray_trans.gray &&
             crt_trans_value != last_trans_value)
         {
            is_gray = 0;
            break;
         }
      }
   }

   /* Remove tRNS if it is entirely sterile. */
   if (num_trans > 0 && last_trans_index < 0)
   {
      num_trans = 0;
      png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
      png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
      result |= OPNG_REDUCE_PALETTE_FAST;
   }

   if (reductions & OPNG_REDUCE_PALETTE_FAST)
   {
      if (num_palette != last_color_index + 1)
      {
         /* Reduce PLTE. */
         /* hIST is reduced automatically. */
         opng_realloc_PLTE(png_ptr, info_ptr, last_color_index + 1);
         png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
         OPNG_ASSERT(num_palette == last_color_index + 1);
         result |= OPNG_REDUCE_PALETTE_FAST;
      }

      if (num_trans > 0 && num_trans != last_trans_index + 1)
      {
         /* Reduce tRNS. */
         opng_realloc_tRNS(png_ptr, info_ptr, last_trans_index + 1);
         png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, NULL);
         OPNG_ASSERT(num_trans == last_trans_index + 1);
         result |= OPNG_REDUCE_PALETTE_FAST;
      }
   }

   if (reductions & OPNG_REDUCE_8_TO_4_2_1)
   {
      result |= opng_reduce_palette_bits(png_ptr, info_ptr, reductions);
      /* Refresh the image information. */
      bit_depth = png_get_bit_depth(png_ptr, info_ptr);
   }
   if ((bit_depth < 8) || !is_gray)
      return result;

   /* Reduce palette --> grayscale. */
   for (i = 0; i < height; ++i)
   {
      for (j = 0; j < width; ++j)
         row_ptr[i][j] = palette[row_ptr[i][j]].red;
   }

   /* Update the ancillary information. */
   if (num_trans > 0)
      png_set_tRNS(png_ptr, info_ptr, NULL, 0, &gray_trans);
#ifdef PNG_bKGD_SUPPORTED
   if (png_get_bKGD(png_ptr, info_ptr, &background))
      background->gray = palette[background->index].red;
#endif
#ifdef PNG_hIST_SUPPORTED
   if (png_get_hIST(png_ptr, info_ptr, &hist))
   {
      png_free_data(png_ptr, info_ptr, PNG_FREE_HIST, -1);
      png_set_invalid(png_ptr, info_ptr, PNG_INFO_hIST);
   }
#endif
#ifdef PNG_sBIT_SUPPORTED
   if (png_get_sBIT(png_ptr, info_ptr, &sig_bits))
   {
      png_byte max_sig_bits = sig_bits->red;
      if (max_sig_bits < sig_bits->green)
         max_sig_bits = sig_bits->green;
      if (max_sig_bits < sig_bits->blue)
         max_sig_bits = sig_bits->blue;
      sig_bits->gray = max_sig_bits;
   }
#endif

   /* Update the image information. */
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      PNG_COLOR_TYPE_GRAY, interlace_type, compression_type, filter_type);
   png_free_data(png_ptr, info_ptr, PNG_FREE_PLTE, -1);
   png_set_invalid(png_ptr, info_ptr, PNG_INFO_PLTE);
   return OPNG_REDUCE_PALETTE_TO_GRAY;  /* ignore the former result */
}

/*
 * Reduce the image (bit depth + color type + palette) without
 * losing any information. The palette (if applicable) and the
 * image data must be present, e.g., by calling png_set_rows(),
 * or by loading IDAT.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
png_uint_32 PNGAPI
opng_reduce_image(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 reductions)
{
   png_uint_32 result;
   int color_type;

   opng_debug(1, "in opng_reduce_image_type");

   if (!opng_validate_image(png_ptr, info_ptr))
   {
      png_warning(png_ptr,
         "Image reduction requires the presence of all critical information");
      return OPNG_REDUCE_NONE;
   }

   color_type = png_get_color_type(png_ptr, info_ptr);

   /* The reductions below must be applied in this particular order. */

   /* Try to reduce the high bits and color/alpha channels. */
   result = opng_reduce_bits(png_ptr, info_ptr, reductions);

   /* Try to reduce the palette image. */
   if (color_type == PNG_COLOR_TYPE_PALETTE &&
       (reductions &
        (OPNG_REDUCE_PALETTE_TO_GRAY |
         OPNG_REDUCE_PALETTE_FAST |
         OPNG_REDUCE_8_TO_4_2_1)))
      result |= opng_reduce_palette(png_ptr, info_ptr, reductions);

   /* Try to reduce RGB to palette or grayscale to palette. */
   if (((color_type & ~PNG_COLOR_MASK_ALPHA) == PNG_COLOR_TYPE_GRAY &&
        (reductions & OPNG_REDUCE_GRAY_TO_PALETTE)) ||
       ((color_type & ~PNG_COLOR_MASK_ALPHA) == PNG_COLOR_TYPE_RGB &&
        (reductions & OPNG_REDUCE_RGB_TO_PALETTE)))
   {
      if (!(result & OPNG_REDUCE_PALETTE_TO_GRAY))
         result |= opng_reduce_to_palette(png_ptr, info_ptr, reductions);
   }

   return result;
}

#endif /* OPNG_IMAGE_REDUCTIONS_SUPPORTED */
