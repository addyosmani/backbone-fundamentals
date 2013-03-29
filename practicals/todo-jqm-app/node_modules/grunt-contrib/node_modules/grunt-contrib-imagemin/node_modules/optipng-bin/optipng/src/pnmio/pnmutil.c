/**
 * pnmutil.c
 * PNM utilities.
 *
 * Copyright (C) 2002-2008 Cosmin Truta.
 * This file is part of the pnmio library, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in pnmio.h.
 **/


#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include "pnmio.h"


/**
 * Validates a PNM structure.
 * Returns 1 on success, 0 on failure.
 **/
int pnm_is_valid(const pnm_struct *pnm_ptr)
{
    unsigned int format = pnm_ptr->format;
    unsigned int depth  = pnm_ptr->depth;
    unsigned int width  = pnm_ptr->width;
    unsigned int height = pnm_ptr->height;
    unsigned int maxval = pnm_ptr->maxval;

    if (depth == 0 || width == 0 || height == 0 || maxval == 0)
        return 0;

#if PNM_UINT_MAX < UINT_MAX
    if (maxval > PNM_UINT_MAX)
        return 0;
#endif

    switch (format)
    {
    case PNM_P1:
    case PNM_P4:
        /* PBM */
        return (depth == 1 && maxval == 1) ? 1 : 0;
    case PNM_P2:
    case PNM_P5:
        /* PGM */
        return (depth == 1) ? 1 : 0;
    case PNM_P3:
    case PNM_P6:
        /* PPM */
        return (depth == 3) ? 1 : 0;
    case PNM_P7:
        /* PAM */
        return 1;
    default:
        return 0;
    }
}


/**
 * Calculates the size of a raw PNM sample, i.e. the smallest number of
 * bytes required to store a sample value between 0 and pnm_ptr->maxval.
 * The validity check performed on the PNM structure is only partial.
 * Returns the raw sample size on success, or 0 on validation failure.
 **/
size_t pnm_raw_sample_size(const pnm_struct *pnm_ptr)
{
    unsigned int maxval = pnm_ptr->maxval;

    if (maxval == 0)
        errno = EINVAL;  /* fall through */

    if (maxval <= 0xffU)
        return 1;
    else if (maxval <= 0xffffU)
        return 2;
#if PNM_UINT_BIT > 16
    else if (maxval <= 0xffffffU)
        return 3;
    else if (maxval <= 0xffffffffU)
        return 4;
#endif
    else  /* maxval > PNM_UINT_MAX */
    {
        errno = EINVAL;
        return 0;
    }
}


/**
 * Calculates the number of bytes occupied by an array of PNM samples.
 * The byte count is sample_size * pnm_ptr->depth * pnm_ptr->width * num_rows.
 * The validity check performed on the PNM structure is only partial.
 * Returns the array size on success, or 0 on validation failure.
 **/
size_t pnm_mem_size(const pnm_struct *pnm_ptr,
                    size_t sample_size, unsigned int num_rows)
{
    unsigned int depth = pnm_ptr->depth;
    unsigned int width = pnm_ptr->width;

    if (sample_size == 0 || depth == 0 || width == 0)
    {
        errno = EINVAL;
        return 0;
    }

    if (num_rows > (size_t)(-1) / sample_size / depth / width)
    {
        errno = ERANGE;
        return 0;
    }

    return sample_size * depth * width * num_rows;
}
