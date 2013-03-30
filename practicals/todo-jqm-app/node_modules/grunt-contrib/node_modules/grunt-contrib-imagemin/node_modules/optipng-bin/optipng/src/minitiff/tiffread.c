/**
 * tiffread.c
 * File input routines for minitiff.
 *
 * Copyright (C) 2006 Cosmin Truta.
 *
 * minitiff is open-source software, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in minitiff.h.
 **/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minitiff.h"
#include "tiffdef.h"


/**
 * Error messages.
 **/
static const char *
tiff_err_read        = "Error reading TIFF file";

static const char *
tiff_err_notiff      = "Not a TIFF file";

static const char *
tiff_err_invalid     = "Invalid TIFF file";

static const char *
tiff_err_unsupported = "Unsupported data format in TIFF file";

static const char *
tiff_err_compr       = "Unsupported compression in TIFF file";

static const char *
tiff_err_memory      = "Out of memory";

static const char *
tiff_warn_tag        = "Unrecognized tag(s) in TIFF file";

static const char *
tiff_warn_metadata   = "Unrecognized EXIF/IPTC/XMP metadata in TIFF file";

static const char *
tiff_warn_multiple   = "Selected first image from multi-image TIFF file";


/**
 * Utilities.
 **/
struct
minitiff_get_struct
{
    unsigned int  (*get_ushort)(const unsigned char *buf_ptr);
    unsigned long (*get_ulong) (const unsigned char *buf_ptr);
};

static unsigned int
get_ushort_m(const unsigned char *buf_ptr)
{
    return ((unsigned int)buf_ptr[0] << 8) +
           ((unsigned int)buf_ptr[1]);
}

static unsigned int
get_ushort_i(const unsigned char *buf_ptr)
{
    return ((unsigned int)buf_ptr[0])      +
           ((unsigned int)buf_ptr[1] << 8);
}

static unsigned long
get_ulong_m(const unsigned char *buf_ptr)
{
    return ((unsigned long)buf_ptr[0] << 24) +
           ((unsigned long)buf_ptr[1] << 16) +
           ((unsigned long)buf_ptr[2] << 8)  +
           ((unsigned long)buf_ptr[3]);
}

static unsigned long
get_ulong_i(const unsigned char *buf_ptr)
{
    return ((unsigned long)buf_ptr[0])       +
           ((unsigned long)buf_ptr[1] << 8)  +
           ((unsigned long)buf_ptr[2] << 16) +
           ((unsigned long)buf_ptr[3] << 24);
}

static unsigned long
get_ulong_value(struct minitiff_get_struct *get_ptr, int tag_type,
                const unsigned char *buf_ptr)
{
    switch (tag_type)
    {
    case 1:  /* byte */
        return (unsigned long)buf_ptr[0];
    case 3:  /* ushort */
        return (unsigned long)get_ptr->get_ushort(buf_ptr);
    case 4:  /* ulong */
        return get_ptr->get_ulong(buf_ptr);
    default:
        return (unsigned long)-1L;  /* error */
    }
}

static size_t
read_ulong_values(struct minitiff_get_struct *get_ptr, int tag_type,
                  unsigned long values[], size_t count, FILE *fp)
{
    unsigned char buf[4];
    size_t value_size;
    size_t i;

    if (tag_type == 1)  /* byte */
        value_size = 1;
    else if (tag_type == 3)  /* ushort */
        value_size = 2;
    else if (tag_type == 4)  /* ulong */
        value_size = 4;
    else
        return 0;  /* read nothing */

    for (i = 0; i < count; ++i)
    {
        if (fread(buf, value_size, 1, fp) != 1)
            break;
        values[i] = get_ulong_value(get_ptr, tag_type, buf);
    }

    return i;
}


/**
 * Read initializer.
 **/
void
minitiff_read_info(struct minitiff_info *tiff_ptr, FILE *fp)
{
    struct minitiff_get_struct getter;
    unsigned char buf[12];
    unsigned char *vbuf = buf + 8;
    long dir_offset;
    unsigned int dir_size, i;
    unsigned int tag_id, tag_type;
    size_t count;
    size_t bits_per_sample_count;
    unsigned int bits_per_sample_tag_type, strip_offsets_tag_type;
    long bits_per_sample_offset, strip_offsets_offset;
    int unknown_tag_found, unknown_metadata_found;

    /* Read the TIFF header. */
    if (fread(buf, 8, 1, fp) != 1)
        goto err_read;
    if (memcmp(buf, minitiff_sig_m, 4) == 0)
    {
        tiff_ptr->byte_order = 'M';
        getter.get_ushort = get_ushort_m;
        getter.get_ulong  = get_ulong_m;
    }
    else if (memcmp(buf, minitiff_sig_i, 4) == 0)
    {
        tiff_ptr->byte_order = 'I';
        getter.get_ushort = get_ushort_i;
        getter.get_ulong  = get_ulong_i;
    }
    else
    {
        minitiff_error(tiff_ptr, tiff_err_notiff);
        return;
    }
    bits_per_sample_count = 0;
    bits_per_sample_tag_type = strip_offsets_tag_type = 0;
    bits_per_sample_offset = strip_offsets_offset = 0;
    dir_offset = (long)getter.get_ulong(buf + 4);
    if (dir_offset < 8L)
        goto err_invalid;
    if (fseek(fp, dir_offset, SEEK_SET) != 0)
        goto err_read;

    /* Read the TIFF directory. */
    if (fread(buf, 2, 1, fp) != 1)
        goto err_read;
    dir_size = getter.get_ushort(buf);
    unknown_tag_found = unknown_metadata_found = 0;
    for (i = 0; i < dir_size; ++i)
    {
        if (fread(buf, 12, 1, fp) != 1)
            goto err_read;
        tag_id   = getter.get_ushort(buf);
        tag_type = getter.get_ushort(buf + 2);
        count    = (size_t)getter.get_ulong(buf + 4);
        if (count == 0)
            goto err_unsupported;
        switch (tag_id)
        {
        case TIFF_TAG_SUBFILE_TYPE:
            if (count != 1
             || (get_ulong_value(&getter, tag_type, vbuf) & ~(2UL)) != 0)
                goto err_unsupported;
            break;
        case TIFF_TAG_WIDTH:
            if (count != 1)
                goto err_unsupported;
            tiff_ptr->width = (size_t)get_ulong_value(&getter, tag_type, vbuf);
            break;
        case TIFF_TAG_HEIGHT:
            if (count != 1)
                goto err_unsupported;
            tiff_ptr->height = (size_t)get_ulong_value(&getter, tag_type, vbuf);
            break;
        case TIFF_TAG_BITS_PER_SAMPLE:
            if (count == 1)
                tiff_ptr->bits_per_sample =
                    (unsigned int)get_ulong_value(&getter, tag_type, vbuf);
            else
            {
                bits_per_sample_count    = count;
                bits_per_sample_tag_type = tag_type;
                bits_per_sample_offset   = (long)getter.get_ulong(vbuf);
            }
            break;
        case TIFF_TAG_COMPRESSION:
            if (count == 1 && get_ulong_value(&getter, tag_type, vbuf) == 1)
                tiff_ptr->compression = 1;
            else
                minitiff_error(tiff_ptr, tiff_err_compr);
            break;
        case TIFF_TAG_PHOTOMETRIC:
            if (count != 1)
                goto err_unsupported;
            tiff_ptr->photometric =
                (unsigned int)get_ulong_value(&getter, tag_type, vbuf);
            break;
        case TIFF_TAG_STRIP_OFFSETS:
            tiff_ptr->strip_offsets_count = count;
            if (count == 1)
            {
                if (tiff_ptr->strip_offsets != NULL)
                    goto err_invalid;
                tiff_ptr->strip_offsets = (long *)malloc(sizeof(long));
                if (tiff_ptr->strip_offsets == NULL)
                    goto err_memory;
                tiff_ptr->strip_offsets[0] =
                    (long)get_ulong_value(&getter, tag_type, vbuf);
            }
            else
            {
                strip_offsets_tag_type = tag_type;
                strip_offsets_offset   = (long)getter.get_ulong(vbuf);
            }
            break;
        case TIFF_TAG_ORIENTATION:
            if (count != 1 || get_ulong_value(&getter, tag_type, vbuf) != 1)
                minitiff_warning(tiff_ptr,
                    "Non-default TIFF image orientation");
            break;
        case TIFF_TAG_SAMPLES_PER_PIXEL:
            if (count != 1)
                goto err_unsupported;
            tiff_ptr->samples_per_pixel =
                (unsigned int)get_ulong_value(&getter, tag_type, vbuf);
            break;
        case TIFF_TAG_ROWS_PER_STRIP:
            if (count != 1)
                goto err_unsupported;
            tiff_ptr->rows_per_strip =
                (unsigned int)get_ulong_value(&getter, tag_type, vbuf);
            break;
        case TIFF_TAG_STRIP_BYTE_COUNTS:
            /* ignored for uncompressed images */
            break;
        case TIFF_TAG_PLANAR_CONFIGURATION:
        case TIFF_TAG_PREDICTOR:
            if (count != 1 || get_ulong_value(&getter, tag_type, vbuf) != 1)
                goto err_unsupported;
            break;
        case TIFF_TAG_DOCUMENT_NAME:
        case TIFF_TAG_IMAGE_DESCRIPTION:
        case TIFF_TAG_MAKE:
        case TIFF_TAG_MODEL:
        case TIFF_TAG_MIN_SAMPLE_VALUE:
        case TIFF_TAG_MAX_SAMPLE_VALUE:
        case TIFF_TAG_X_RESOLUTION:
        case TIFF_TAG_Y_RESOLUTION:
        case TIFF_TAG_X_POSITION:
        case TIFF_TAG_Y_POSITION:
        case TIFF_TAG_RESOLUTION_UNIT:
        case TIFF_TAG_PAGE_NAME:
        case TIFF_TAG_PAGE_NUMBER:
        case TIFF_TAG_SOFTWARE:
        case TIFF_TAG_DATE_TIME:
        case TIFF_TAG_ARTIST:
        case TIFF_TAG_HOST_COMPUTER:
            /* ignore */
            break;
        case TIFF_TAGEXT_XMP:
        case TIFF_TAGEXT_IPTC:
        case TIFF_TAGEXT_EXIF_IFD:
        case TIFF_TAGEXT_GPS_IFD:
        case TIFF_TAGEXT_INTEROPERABILITY_IFD:
        case TIFF_TAGEXT_PRINT_IM:
            if (!unknown_metadata_found)
            {
                unknown_metadata_found = 1;
                minitiff_warning(tiff_ptr, tiff_warn_metadata);
            }
            break;
        default:
            if (!unknown_tag_found)
            {
                unknown_tag_found = 1;
                minitiff_warning(tiff_ptr, tiff_warn_tag);
            }
            break;
        }
    }

    /* Is this the last TIFF directory? */
    if (fread(buf, 4, 1, fp) != 1)
        goto err_read;
    if (getter.get_ulong(buf) != 0)
        minitiff_warning(tiff_ptr, tiff_warn_multiple);

    /* Finish up the incomplete readings. */
    if (bits_per_sample_offset != 0)
    {
        unsigned long values[4];
        count = bits_per_sample_count;
        if (count != (size_t)tiff_ptr->samples_per_pixel)
            goto err_invalid;
        if (count > 4)
            goto err_unsupported;
        if (fseek(fp, bits_per_sample_offset, SEEK_SET) != 0)
            goto err_read;
        if (read_ulong_values(&getter, bits_per_sample_tag_type,
                              values, count, fp) != count)
            goto err_read;
        while (--count > 0)
            if (values[0] != values[count])
                goto err_unsupported;
        tiff_ptr->bits_per_sample = (unsigned int)values[0];
    }
    if (strip_offsets_offset != 0)
    {
        count = tiff_ptr->strip_offsets_count;
        if (count == 0 || count > tiff_ptr->height)
            goto err_invalid;
        tiff_ptr->strip_offsets = (long *)malloc(count * sizeof(long));
        if (tiff_ptr->strip_offsets == NULL)
            goto err_memory;
        if (fseek(fp, strip_offsets_offset, SEEK_SET) != 0)
            goto err_read;
        if (read_ulong_values(&getter, strip_offsets_tag_type,
                (unsigned long *)tiff_ptr->strip_offsets, count, fp) != count)
            goto err_read;
    }

    /* Terminate. */
    return;

    /* Quick and dirty goto labels. */
    err_read:
        minitiff_error(tiff_ptr, tiff_err_read);
    err_invalid:
        minitiff_error(tiff_ptr, tiff_err_invalid);
    err_unsupported:
        minitiff_error(tiff_ptr, tiff_err_unsupported);
    err_memory:
        minitiff_error(tiff_ptr, tiff_err_memory);
}


/**
 * Row reader.
 **/
void
minitiff_read_row(struct minitiff_info *tiff_ptr,
                  unsigned char *row_ptr, size_t row_index, FILE *fp)
{
    size_t row_size, strip_index;
    unsigned int bytes_per_sample, sample_max;
    long offset;
    size_t i;

    /* Do not do validation here. */
    /* Call minitiff_validate_info() before calling this function. */

    bytes_per_sample = (tiff_ptr->bits_per_sample + 7) / 8;
    row_size = tiff_ptr->width * tiff_ptr->samples_per_pixel
               * bytes_per_sample;

    /* Position the file pointer to the beginning of the row,
     * if that has not been done already.
     */
    strip_index = row_index / tiff_ptr->rows_per_strip;
    if (strip_index >= tiff_ptr->strip_offsets_count)
        goto err_invalid;
    offset = tiff_ptr->strip_offsets[strip_index]
             + row_size * (row_index % tiff_ptr->rows_per_strip);
    if (offset <= 0)
        goto err_invalid;
    if (ftell(fp) != offset)
        if (fseek(fp, offset, SEEK_SET) != 0)
            goto err_read;

    /* Read the row, and do all the necessary adjustments. */
    if (fread(row_ptr, row_size, 1, fp) != 1)
        goto err_read;
    if (tiff_ptr->photometric == 0)  /* white is zero */
    {
        if (bytes_per_sample > 1)
            goto err_unsupported;
        sample_max = (1 << tiff_ptr->bits_per_sample) - 1;
        for (i = 0; i < row_size; ++i)
            row_ptr[i] = (unsigned char)(sample_max - row_ptr[i]);
    }

    /* Terminate. */
    return;

    /* Quick and dirty goto labels. */
    err_read:
        minitiff_error(tiff_ptr, tiff_err_read);
    err_invalid:
        minitiff_error(tiff_ptr, tiff_err_invalid);
    err_unsupported:
        minitiff_error(tiff_ptr, tiff_err_unsupported);
}
