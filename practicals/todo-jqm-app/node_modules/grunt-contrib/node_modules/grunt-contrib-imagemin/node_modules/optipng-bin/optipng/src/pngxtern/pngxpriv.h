/*
 * pngxpriv.h - private helpers for pngxtern.
 *
 * Copyright (C) 2003-2011 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 */

#ifndef PNGXPRIV_H
#define PNGXPRIV_H

/* This header is not meant to be used outside pngxtern. */
#ifdef PNGX_INTERNAL

#include "png.h"
#include <stdio.h>

#if defined(PNGX_DEBUG) && (PNGX_DEBUG > 0)
#include <assert.h>
#define PNGX_ASSERT(cond) assert(cond)
#else
#define PNGX_ASSERT(cond) ((void)0)
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* BMP support */
int pngx_sig_is_bmp(png_bytep sig, size_t sig_size,
                    png_const_charpp fmt_name_ptr,
                    png_const_charpp fmt_long_name_ptr);
int pngx_read_bmp(png_structp png_ptr, png_infop info_ptr, FILE *stream);

/* GIF support */
int pngx_sig_is_gif(png_bytep sig, size_t sig_size,
                    png_const_charpp fmt_name_ptr,
                    png_const_charpp fmt_long_name_ptr);
int pngx_read_gif(png_structp png_ptr, png_infop info_ptr, FILE *stream);

/* JPEG support (well, not really...) */
int pngx_sig_is_jpeg(png_bytep sig, size_t sig_size,
                     png_const_charpp fmt_name_ptr,
                     png_const_charpp fmt_long_name_ptr);
int pngx_read_jpeg(png_structp png_ptr, png_infop info_ptr, FILE *stream);

/* PNM support */
int pngx_sig_is_pnm(png_bytep sig, size_t sig_size,
                    png_const_charpp fmt_name_ptr,
                    png_const_charpp fmt_long_name_ptr);
int pngx_read_pnm(png_structp png_ptr, png_infop info_ptr, FILE *stream);

/* TIFF support (partial) */
int pngx_sig_is_tiff(png_bytep sig, size_t sig_size,
                     png_const_charpp fmt_name_ptr,
                     png_const_charpp fmt_long_name_ptr);
int pngx_read_tiff(png_structp png_ptr, png_infop info_ptr, FILE *stream);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* PNGX_INTERNAL */

#endif /* PNGXPRIV_H */
