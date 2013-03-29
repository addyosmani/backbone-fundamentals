/**
 * minitiff.h
 * Minimal I/O interface to the Tagged Image File Format (TIFF).
 * Version 0.1, Release 2006-July-21.
 *
 * Copyright (C) 2006 Cosmin Truta.
 *
 * minitiff is open-source software, distributed under the zlib license.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.  If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/


#ifndef MINITIFF_H
#define MINITIFF_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * The minitiff data structure.
 **/
struct minitiff_info
{
    void (*error_handler)(const char *msg);
    void (*warning_handler)(const char *msg);
    int byte_order;
    size_t width, height;
    unsigned int bits_per_sample;
    unsigned int compression;
    unsigned int photometric;
    size_t strip_offsets_count;
    long *strip_offsets;
    unsigned int samples_per_pixel;
    unsigned int rows_per_strip;
};


/**
 * Constructor, validator and destructor.
 **/
void minitiff_init_info(struct minitiff_info *minitiff_ptr);
void minitiff_validate_info(struct minitiff_info *tiff_ptr);
void minitiff_destroy_info(struct minitiff_info *minitiff_ptr);


/**
 * Input functions.
 **/
void minitiff_read_info(struct minitiff_info *minitiff_ptr, FILE *fp);
void minitiff_read_row(struct minitiff_info *minitiff_ptr,
                       unsigned char *byte_ptr, size_t row_index,
                       FILE *fp);


/**
 * Output functions.
 **/
void minitiff_write_info(struct minitiff_info *minitiff_ptr, FILE *fp);
void minitiff_write_row(const struct minitiff_info *minitiff_ptr,
                        const unsigned char *byte_ptr, size_t row_index,
                        FILE *fp);


/**
 * Error reporting.
 **/
void minitiff_error(struct minitiff_info *minitiff_ptr, const char *msg);
void minitiff_warning(struct minitiff_info *minitiff_ptr, const char *msg);


/**
 * Global constants:
 * TIFF file signature.
 **/
extern const char minitiff_sig_m[4];
extern const char minitiff_sig_i[4];


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* MINITIFF_H */
