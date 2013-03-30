/*
 * pngxmem.c - libpng extension: memory allocation utilities.
 *
 * Copyright (C) 2003-2011 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 */

#include "pngxutil.h"


#ifdef PNG_INFO_IMAGE_SUPPORTED


png_bytepp PNGAPI
pngx_malloc_rows(png_structp png_ptr, png_infop info_ptr, int filler)
{
   return pngx_malloc_rows_extended(png_ptr, info_ptr, 0, filler);
}

png_bytepp PNGAPI
pngx_malloc_rows_extended(png_structp png_ptr, png_infop info_ptr,
   pngx_alloc_size_t min_row_size, int filler)
{
   pngx_alloc_size_t row_size;
   png_bytep row;
   png_bytepp rows;
   png_uint_32 height, i;

   /* Calculate the row size. */
   row_size = png_get_rowbytes(png_ptr, info_ptr);
   if (row_size == 0)
      return NULL;
   if (row_size < min_row_size)
      row_size = min_row_size;

   /* Deallocate the currently-existing rows. */
   png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);

   /* Allocate memory for the row index. */
   height = png_get_image_height(png_ptr, info_ptr);
   if (height > (pngx_alloc_size_t)(-1) / sizeof(png_bytep))
   {
      png_warning(png_ptr, "Image height is too large");
      return NULL;
   }
   rows = (png_bytepp)png_malloc(png_ptr,
      (pngx_alloc_size_t)(height * sizeof(png_bytep)));
   if (rows == NULL)
      return NULL;

   /* Allocate memory for each row. */
   for (i = 0; i < height; ++i)
   {
      row = (png_bytep)png_malloc(png_ptr, row_size);
      if (row == NULL)  /* out of memory? */
      {
         /* Release the memory allocated up to the point of failure. */
         while (i > 0)
            png_free(png_ptr, rows[--i]);
         png_free(png_ptr, rows);
         return NULL;
      }
      if (filler >= 0)
         memset(row, filler, row_size);
      rows[i] = row;
   }

   /* Set the row pointers. */
   png_set_rows(png_ptr, info_ptr, rows);
   return rows;
}

#if 0  /* not necessary */
void PNGAPI
pngx_free_rows(png_structp png_ptr, png_infop info_ptr)
{
   png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);
}
#endif


#endif /* PNG_INFO_IMAGE_SUPPORTED */
