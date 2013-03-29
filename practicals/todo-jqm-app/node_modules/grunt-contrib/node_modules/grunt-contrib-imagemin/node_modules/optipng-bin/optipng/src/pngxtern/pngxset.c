/*
 * pngxset.c - libpng extension: additional image info storage.
 *
 * Copyright (C) 2003-2011 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 */

#include "pngxutil.h"


/*
 * NOTE:
 * There is a discrepancy between the parameter names used in
 * this module vs. the PNG specification.
 * The PNG specification uses the terms "compression method",
 * "filter method" and "interlace method", while this module
 * uses the terms, respectively, "compression type", "filter type"
 * and "interlace type", following the libpng naming conventions.
 */

void PNGAPI
pngx_set_compression_type(png_structp png_ptr, png_infop info_ptr,
                          int compression_type)
{
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type, filter_type;
   int old_compression_type;

   if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
       &color_type, &interlace_type, &old_compression_type, &filter_type))
      return;
   if (compression_type == old_compression_type)
      return;
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      color_type, interlace_type, compression_type, filter_type);
}

void PNGAPI
pngx_set_filter_type(png_structp png_ptr, png_infop info_ptr,
                     int filter_type)
{
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type, compression_type;
   int old_filter_type;

   if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
       &color_type, &interlace_type, &compression_type, &old_filter_type))
      return;
   if (filter_type == old_filter_type)
      return;
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      color_type, interlace_type, compression_type, filter_type);
}

void PNGAPI
pngx_set_interlace_type(png_structp png_ptr, png_infop info_ptr,
                        int interlace_type)
{
   png_uint_32 width, height;
   int bit_depth, color_type, compression_type, filter_type;
   int old_interlace_type;

   if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
       &color_type, &old_interlace_type, &compression_type, &filter_type))
      return;
   if (interlace_type == old_interlace_type)
      return;
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      color_type, interlace_type, compression_type, filter_type);
}
