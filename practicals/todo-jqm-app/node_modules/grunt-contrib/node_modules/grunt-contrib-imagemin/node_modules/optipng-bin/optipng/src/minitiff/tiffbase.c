/**
 * tiffbase.c
 * General-purpose routines for minitiff.
 *
 * Copyright (C) 2006 Cosmin Truta.
 *
 * minitiff is open-source software, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in minitiff.h.
 **/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minitiff.h"


/**
 * Constructor.
 **/
void minitiff_init_info(struct minitiff_info *minitiff_ptr)
{
    memset(minitiff_ptr, 0, sizeof(*minitiff_ptr));
    minitiff_ptr->photometric = (unsigned int)(-1);
}


/**
 * Validator.
 **/
void minitiff_validate_info(struct minitiff_info *tiff_ptr)
{
    if (tiff_ptr->width == 0 || tiff_ptr->height == 0
     || tiff_ptr->bits_per_sample == 0 || tiff_ptr->samples_per_pixel == 0)
        minitiff_error(tiff_ptr, "Invalid TIFF dimensions");
    if (tiff_ptr->strip_offsets == NULL || tiff_ptr->rows_per_strip == 0)
        minitiff_error(tiff_ptr, "Invalid TIFF strip info");
    if (tiff_ptr->photometric >= 3)
        minitiff_error(tiff_ptr, "Unknown photometric interpretation");
}


/**
 * Destructor.
 **/
void minitiff_destroy_info(struct minitiff_info *minitiff_ptr)
{
    if (minitiff_ptr->strip_offsets != NULL)
        free(minitiff_ptr->strip_offsets);
}


/**
 * Error reporting.
 **/
static void minitiff_default_error_handler(const char *msg)
{
    fprintf(stderr, "** Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void minitiff_error(struct minitiff_info *minitiff_ptr, const char *msg)
{
    if (minitiff_ptr->error_handler != NULL)
        minitiff_ptr->error_handler(msg);
    else
        minitiff_default_error_handler(msg);
    abort();
}

static void minitiff_default_warning_handler(const char *msg)
{
    fprintf(stderr, "** Warning: %s\n", msg);
}

void minitiff_warning(struct minitiff_info *minitiff_ptr, const char *msg)
{
    if (minitiff_ptr->warning_handler != NULL)
        minitiff_ptr->warning_handler(msg);
    else
        minitiff_default_warning_handler(msg);
}


/**
 * Global constants:
 * TIFF file signature.
 **/
const char minitiff_sig_m[4] = { 0x4d, 0x4d, 0x00, 0x2a };  /* "MM\0*" */
const char minitiff_sig_i[4] = { 0x49, 0x49, 0x2a, 0x00 };  /* "II*\0" */
