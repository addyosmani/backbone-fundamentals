/**
 * pnmout.c
 * PNM file output.
 *
 * Copyright (C) 2002-2008 Cosmin Truta.
 * This file is part of the pnmio library, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in pnmio.h.
 **/


#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include "pnmio.h"


/**
 * Validates a PNM header structure and writes it to a file stream.
 * Returns 1 on success, 0 on validation failure, or -1 on output failure.
 **/
int pnm_fput_header(const pnm_struct *pnm_ptr, FILE *stream)
{
    unsigned int format = pnm_ptr->format;
    unsigned int depth  = pnm_ptr->depth;
    unsigned int width  = pnm_ptr->width;
    unsigned int height = pnm_ptr->height;
    unsigned int maxval = pnm_ptr->maxval;
    int result;

    /* validate the info structure */
    if (!pnm_is_valid(pnm_ptr))
        return 0;

    /* write the PNM file header */
    switch (format)
    {
    case PNM_P1:
    case PNM_P4:
        result = fprintf(stream,
            "P%c\n%u %u\n", format + '0', width, height);
        break;
    case PNM_P2:
    case PNM_P3:
    case PNM_P5:
    case PNM_P6:
        result = fprintf(stream,
            "P%c\n%u %u\n%u\n", format + '0', width, height, maxval);
        break;
    case PNM_P7:
        result = fprintf(stream,
            "P7\nDEPTH %u\nWIDTH %u\nHEIGHT %u\nMAXVAL %u\nENDHDR\n",
            depth, width, height, maxval);
        break;
    default:
        errno = EINVAL;
        return 0;
    }

    /* check the result */
    return (result > 0) ? 1 : -1;
}


/**
 * Writes an array of PNM sample values to a file stream.
 * The values are written in the format specified by pnm_ptr->format.
 * The array length is pnm_ptr->depth * pnm_ptr->width * num_rows.
 * The validity check performed on the PNM structure is only partial.
 * Returns 1 on success, 0 on validation failure, or -1 on output failure.
 **/
int pnm_fput_values(const pnm_struct *pnm_ptr,
                    const unsigned int *sample_values,
                    unsigned int num_rows,
                    FILE *stream)
{
    unsigned int format = pnm_ptr->format;
    unsigned int depth  = pnm_ptr->depth;
    unsigned int width  = pnm_ptr->width;
    unsigned int maxval = pnm_ptr->maxval;
    size_t row_length = (size_t)depth * (size_t)width;
    size_t num_samples = num_rows * row_length;
    int ch, mask;
    size_t i, j;

    /* write the sample values */
    switch (format)
    {
    case PNM_P1:
        for (i = j = 0; i < num_samples; ++i)
        {
            if (putc(((sample_values[i] != 0) ? '0' : '1'), stream) == EOF)
                break;
            if (++j == row_length)
            {
                j = 0;
                if (putc('\n', stream) == EOF)
                    break;
            }
        }
        break;
    case PNM_P2:
    case PNM_P3:
        for (i = j = 0; i < num_samples; ++i)
        {
            if (++j == row_length)
                j = 0;
            if (fprintf(stream,
                    (j == 0) ? "%u\n" : "%u ", sample_values[i]) <= 0)
                break;
        }
        break;
    case PNM_P4:
        for (i = j = 0; i < num_samples; )
        {
            ch = 0;
            for (mask = 0x80; mask != 0; mask >>= 1)
            {
                if (sample_values[i++] == 0)
                    ch |= mask;
                if (++j == row_length)
                {
                    j = 0;
                    break;
                }
            }
            if (putc(ch, stream) == EOF)
                break;
        }
        break;
    case PNM_P5:
    case PNM_P6:
    case PNM_P7:
        if (maxval <= 0xffU)  /* 1 byte per sample */
        {
            for (i = 0; i < num_samples; ++i)
            {
                if (putc(sample_values[i] & 0xff, stream) == EOF)
                    break;
            }
        }
        else if (maxval <= 0xffffU)  /* 2 bytes per sample */
        {
            for (i = 0; i < num_samples; ++i)
            {
                if (putc((sample_values[i] >> 8) & 0xff, stream) == EOF ||
                        putc((sample_values[i]) & 0xff, stream) == EOF)
                    break;
            }
        }
#if PNM_UINT_BIT > 16
        else if (maxval <= 0xffffffffU)  /* 3 or 4 bytes per sample */
        {
            for (i = 0; i < num_samples; ++i)
            {
                if (maxval > 0xffffffU)
                    if (putc((sample_values[i] >> 24) & 0xff, stream) == EOF)
                        break;
                if (putc((sample_values[i] >> 16) & 0xff, stream) == EOF ||
                        putc((sample_values[i] >> 8) & 0xff, stream) == EOF ||
                        putc((sample_values[i]) & 0xff, stream) == EOF)
                    break;
            }
        }
#endif
        else  /* maxval > PNM_UINT_MAX */
        {
            errno = EINVAL;
            return 0;
        }
        break;
    default:
        errno = EINVAL;
        return 0;
    }

    /* check the result */
    return (i == num_samples) ? 1 : -1;
}


/**
 * Writes an array of sample bytes to a raw PNM file stream.
 * Multi-byte samples are stored in network order, as in the PNM stream.
 * The byte count is sample_size * pnm_ptr->depth * pnm_ptr->width * num_rows.
 * The validity check performed on the PNM structure is only partial.
 * Returns 1 on success, 0 on validation failure, or -1 on output failure.
 **/
int pnm_fput_bytes(const pnm_struct *pnm_ptr,
                   const unsigned char *sample_bytes,
                   size_t sample_size,
                   unsigned int num_rows,
                   FILE *stream)
{
    unsigned int format = pnm_ptr->format;
    unsigned int depth  = pnm_ptr->depth;
    unsigned int width  = pnm_ptr->width;
    unsigned int maxval = pnm_ptr->maxval;
    size_t row_length = (size_t)depth * (size_t)width;
    size_t num_samples = num_rows * row_length;
    size_t raw_sample_size;
    int ch, mask;
    size_t i, j;

    /* validate the given sample size */
    if (maxval <= 0xffU)
        raw_sample_size = 1;
    else if (maxval <= 0xffffU)
        raw_sample_size = 2;
#if PNM_UINT_BIT > 16
    else if (maxval <= 0xffffffU)
        raw_sample_size = 3;
    else if (maxval <= 0xffffffffU)
        raw_sample_size = 4;
#endif
    else  /* maxval > PNM_UINT_MAX */
        raw_sample_size = !sample_size;
    if (raw_sample_size != sample_size)
    {
        errno = EINVAL;
        return 0;
    }

    /* write the raw sample bytes */
    switch (format)
    {
    case PNM_P4:
        for (i = j = 0; i < num_samples; )
        {
            ch = 0;
            for (mask = 0x80; mask != 0; mask >>= 1)
            {
                if (sample_bytes[i++] == 0)
                    ch |= mask;
                if (++j == row_length)
                {
                    j = 0;
                    break;
                }
            }
            if (putc(ch, stream) == EOF)
                break;
        }
        break;
    case PNM_P5:
    case PNM_P6:
    case PNM_P7:
        i = fwrite(sample_bytes, sample_size, num_samples, stream);
        break;
    default:
        errno = EINVAL;
        return 0;
    }

    /* check the result */
    return (i == num_samples) ? 1 : -1;
}
