/**
 * pnmio.h
 * Simple I/O interface to the Portable Any Map (PNM) image file format.
 * Version 0.3, Release 2008-Jun-15.
 *
 * Copyright (C) 2002-2008 Cosmin Truta.
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
 *
 **/


#ifndef PNMIO_H
#define PNMIO_H

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * PNM format codes
 **/
enum
{
    PNM_P1 = 1,  /* plain PBM */
    PNM_P2 = 2,  /* plain PGM */
    PNM_P3 = 3,  /* plain PPM */
    PNM_P4 = 4,  /* raw PBM */
    PNM_P5 = 5,  /* raw PGM */
    PNM_P6 = 6,  /* raw PPM */
    PNM_P7 = 7   /* PAM (only partially implemented) */
};


/**
 * PNM info structure
 **/
typedef struct pnm_struct
{
    unsigned int format;
    unsigned int depth;
    unsigned int width;
    unsigned int height;
    unsigned int maxval;
} pnm_struct;


/**
 * PNM input functions
 **/
int pnm_fget_header(pnm_struct *pnm_ptr,
                    FILE *stream);
int pnm_fget_values(const pnm_struct *pnm_ptr,
                    unsigned int *sample_values,
                    unsigned int num_rows,
                    FILE *stream);
int pnm_fget_bytes(const pnm_struct *pnm_ptr,
                    unsigned char *sample_bytes,
                    size_t sample_size,
                    unsigned int num_rows,
                    FILE *stream);


/**
 * PNM output functions
 **/
int pnm_fput_header(const pnm_struct *pnm_ptr,
                    FILE *stream);
int pnm_fput_values(const pnm_struct *pnm_ptr,
                    const unsigned int *sample_values,
                    unsigned int num_rows,
                    FILE *stream);
int pnm_fput_bytes(const pnm_struct *pnm_ptr,
                    const unsigned char *sample_bytes,
                    size_t sample_size,
                    unsigned int num_rows,
                    FILE *stream);


/**
 * PNM utility functions
 **/
int pnm_is_valid(const pnm_struct *pnm_ptr);
size_t pnm_raw_sample_size(const pnm_struct *pnm_ptr);
size_t pnm_mem_size(const pnm_struct *pnm_ptr,
                    size_t sample_size,
                    unsigned int num_rows);


/**
 * PNM limits
 **/
#define PNM_UCHAR_BIT 8
#define PNM_UCHAR_MAX 0xffU
#if UINT_MAX < 0xffffffffUL
#define PNM_UINT_BIT 16
#define PNM_UINT_MAX 0xffffU
#else
#define PNM_UINT_BIT 32
#define PNM_UINT_MAX 0xffffffffU
#endif


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PNMIO_H */
