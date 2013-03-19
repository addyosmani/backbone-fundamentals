/**
 * pnmin.c
 * PNM file input.
 *
 * Copyright (C) 2002-2008 Cosmin Truta.
 * This file is part of the pnmio library, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in pnmio.h.
 **/


#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "pnmio.h"


/**
 * Checks if the character is a space: ' ', '\t', '\n' or '\r'.
 **/
#define pnm_is_space(ch) \
    ((ch) == ' ' || (ch) == '\t' || (ch) == '\n' || (ch) == '\r')


/**
 * Checks if the character is a digit: '0' .. '9'.
 **/
#define pnm_is_digit(ch) \
    ((ch) >= '0' && (ch) <= '9')


/**
 * Reads a character from a file stream.
 * Comment sequences starting with '#' are skipped until the end of line.
 * End of line sequences (LF, CR, CR+LF) are translated to '\n'.
 * Returns the character read, or EOF on input failure.
 **/
static int pnm_fget_char(FILE *stream)
{
    int ch = getc(stream);

    /* skip the comments */
    if (ch == '#')
    {
        do
        {
            ch = getc(stream);
        } while (ch != EOF && ch != '\n' && ch != '\r');
    }

    /* translate the line endings */
    if (ch == '\r')
    {
        ch = getc(stream);
        if (ch != '\n')
        {
            ungetc(ch, stream);
            ch = '\n';
        }
    }

    return ch;
}


/**
 * Reads (scans) an unsigned integer from a file stream.
 * Returns 1 on success, 0 on matching failure, or EOF on input failure.
 **/
static int pnm_fscan_uint(FILE *stream, unsigned int *value)
{
    int ch;
    unsigned int tmp;

    /* skip the leading whitespaces */
    do
    {
        ch = pnm_fget_char(stream);
    } while (pnm_is_space(ch));
    if (ch == EOF)  /* input failure */
        return EOF;
    if (!pnm_is_digit(ch))  /* matching failure */
    {
        ungetc(ch, stream);
        return 0;
    }

    /* read the value */
    *value = 0;
    do
    {
        tmp = *value * 10 + (ch - '0');
        if (tmp >= *value)
            *value = tmp;
        else  /* overflow */
        {
            *value = UINT_MAX;
            errno = ERANGE;
        }
        ch = getc(stream);
    } while (pnm_is_digit(ch));

    /* put back the trailing non-whitespace, if any */
    if (!pnm_is_space(ch))
        ungetc(ch, stream);

    return 1;
}


/**
 * Reads a PNM header structure from a file stream and validates it.
 * Returns 1 on success, 0 on validation failure,
 * or -1 on input or matching failure.
 * Reading PAM ("P7") headers is not currently implemented.
 **/
int pnm_fget_header(pnm_struct *pnm_ptr, FILE *stream)
{
    unsigned int format;
    int ch;

    /* clear the PNM info structure */
    memset(pnm_ptr, 0, sizeof(pnm_struct));

    /* read the PNM file signature */
    ch = getc(stream);
    if (ch == EOF)  /* input failure */
        return -1;
    /* any subsequent failure is a matching failure */
    if (ch != 'P')
        return -1;
    ch = getc(stream);
    if (ch < '1' || ch > '9')
        return -1;
    format = (unsigned int)(ch - '0');
    ch = pnm_fget_char(stream);  /* start using pnm_fget_char() */
    if (!pnm_is_space(ch))
        return -1;

    /* read the header */
    pnm_ptr->format = format;
    if (format >= PNM_P1 && format <= PNM_P6)  /* old-style PNM header */
    {
        pnm_ptr->depth = (format == PNM_P3 || format == PNM_P6) ? 3 : 1;
        if (pnm_fscan_uint(stream, &pnm_ptr->width) != 1 ||
                pnm_fscan_uint(stream, &pnm_ptr->height) != 1)
            return -1;
        if (format == PNM_P1 || format == PNM_P4)
            pnm_ptr->maxval = 1;
        else
        {
            if (pnm_fscan_uint(stream, &pnm_ptr->maxval) != 1)
                return -1;
        }
        return pnm_is_valid(pnm_ptr) ? 1 : 0;
    }
    else  /* TODO: if (format == PNM_P7) ... */
        return -1;
}


/**
 * Reads an array of PNM sample values from a file stream.
 * The values are expected to be in the format specified by pnm_ptr->format.
 * The array length is pnm_ptr->depth * pnm_ptr->width * num_rows.
 * The validity check performed on the PNM structure is only partial.
 * Returns 1 on success, 0 on validation failure,
 * or -1 on input or matching failure.
 * If reading is incomplete, the remaining sample values are set to 0.
 **/
int pnm_fget_values(const pnm_struct *pnm_ptr,
                    unsigned int *sample_values,
                    unsigned int num_rows,
                    FILE *stream)
{
    unsigned int format = pnm_ptr->format;
    unsigned int depth  = pnm_ptr->depth;
    unsigned int width  = pnm_ptr->width;
    unsigned int maxval = pnm_ptr->maxval;
    size_t row_length = (size_t)depth * (size_t)width;
    size_t num_samples = num_rows * row_length;
    int ch, ch8;
#if PNM_UINT_BIT > 16
    int ch16, ch24;
#endif
    int mask;
    size_t i, j;

    /* read the sample values */
    switch (format)
    {
    case PNM_P1:
        for (i = 0; i < num_samples; ++i)
        {
            do
            {
                ch = pnm_fget_char(stream);
            } while (pnm_is_space(ch));
            if (ch != '0' && ch != '1')
            {
                ungetc(ch, stream);
                break;
            }
            sample_values[i] = (ch == '0') ? 1 : 0;
        }
        break;
    case PNM_P2:
    case PNM_P3:
        for (i = 0; i < num_samples; ++i)
        {
            if (pnm_fscan_uint(stream, &sample_values[i]) != 1)
                break;
        }
        break;
    case PNM_P4:
        for (i = j = 0; i < num_samples; )
        {
            ch = getc(stream);
            if (ch == EOF)
                break;
            for (mask = 0x80; mask != 0; mask >>= 1)
            {
                sample_values[i++] = (ch & mask) ? 0 : 1;
                if (++j == row_length)
                {
                    j = 0;
                    break;
                }
            }
        }
        break;
    case PNM_P5:
    case PNM_P6:
    case PNM_P7:
        if (maxval <= 0xffU)  /* 1 byte per sample */
        {
            for (i = 0; i < num_samples; ++i)
            {
                ch = getc(stream);
                if (ch == EOF)
                    break;
                sample_values[i] = (unsigned int)ch;
            }
        }
        else if (maxval <= 0xffffU)  /* 2 bytes per sample */
        {
            for (i = 0; i < num_samples; ++i)
            {
                ch8 = getc(stream);
                ch = getc(stream);
                if (ch == EOF)
                    break;
                sample_values[i] = ((unsigned int)ch8 << 8) + (unsigned int)ch;
            }
        }
#if PNM_UINT_BIT > 16
        else if (maxval <= 0xffffffffU)  /* 3 or 4 bytes per sample */
        {
            ch24 = 0;
            for (i = 0; i < num_samples; ++i)
            {
                if (maxval > 0xffffffU)
                    ch24 = getc(stream);
                ch16 = getc(stream);
                ch8 = getc(stream);
                ch = getc(stream);
                if (ch == EOF)
                    break;
                sample_values[i] =
                    ((unsigned int)ch24 << 24) + ((unsigned int)ch16 << 16) +
                    ((unsigned int)ch8  <<  8) + ((unsigned int)ch);
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
    if (i < num_samples)
    {
        memset(sample_values + i, 0, (num_samples - i) * sizeof(unsigned int));
        return -1;
    }
    return 1;
}


/**
 * Reads an array of sample bytes from a raw PNM file stream.
 * Multi-byte samples are stored in network order, as in the PNM stream.
 * The byte count is sample_size * pnm_ptr->depth * pnm_ptr->width * num_rows.
 * The validity check performed on the PNM structure is only partial.
 * Returns 1 on success, 0 on validation failure, or -1 on input failure.
 * If reading is incomplete, the remaining sample bytes are set to 0.
 **/
int pnm_fget_bytes(const pnm_struct *pnm_ptr,
                   unsigned char *sample_bytes,
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

    /* read the raw sample bytes */
    switch (format)
    {
    case PNM_P4:
        for (i = j = 0; i < num_samples; )
        {
            ch = getc(stream);
            if (ch == EOF)
                break;
            for (mask = 0x80; mask != 0; mask >>= 1)
            {
                sample_bytes[i++] = (unsigned char)((ch & mask) ? 0 : 1);
                if (++j == row_length)
                {
                    j = 0;
                    break;
                }
            }
        }
        break;
    case PNM_P5:
    case PNM_P6:
    case PNM_P7:
        i = fread(sample_bytes, sample_size, num_samples, stream);
        break;
    default:
        errno = EINVAL;
        return 0;
    }

    /* check the result */
    if (i < num_samples)
    {
        memset(sample_bytes + i, 0, sample_size * num_samples - i);
        return -1;
    }
    return 1;
}
