/*
 * optim.c
 * The main PNG optimization engine.
 *
 * Copyright (C) 2001-2012 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the attached LICENSE for more information.
 */

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optipng.h"
#include "proginfo.h"

#include "cbitset.h"
#include "opngreduc.h"
#include "osys.h"
#include "png.h"
#include "pngxtern.h"
#include "pngxutil.h"
#include "zlib.h"


/*
 * User exception setup.
 */
#include "cexcept.h"
define_exception_type(const char *);
struct exception_context the_exception_context[1];


/*
 * Optimization tables and presets
 */
static const char *compr_level_presets[OPNG_OPTIM_LEVEL_MAX + 1] =
    { "", "", "9", "9", "9", "9", "1-9", "1-9" };
static const char *mem_level_presets[OPNG_OPTIM_LEVEL_MAX + 1] =
    { "", "", "8", "8-9", "8", "8-9", "8", "8-9" };
static const char *strategy_presets[OPNG_OPTIM_LEVEL_MAX + 1] =
    { "", "", "0-", "0-", "0-", "0-", "0-", "0-" };
static const char *filter_presets[OPNG_OPTIM_LEVEL_MAX + 1] =
    { "", "", "0,5", "0,5", "0-", "0-", "0-", "0-" };
static const int filter_table[OPNG_FILTER_MAX + 1] =
{
    PNG_FILTER_NONE  /* 0 */,
    PNG_FILTER_SUB   /* 1 */,
    PNG_FILTER_UP    /* 2 */,
    PNG_FILTER_AVG   /* 3 */,
    PNG_FILTER_PAETH /* 4 */,
    PNG_ALL_FILTERS  /* 5 */
};

/*
 * Status flags
 */
enum
{
    INPUT_IS_PNG_FILE           = 0x0001,
    INPUT_HAS_PNG_DATASTREAM    = 0x0002,
    INPUT_HAS_PNG_SIGNATURE     = 0x0004,
    INPUT_HAS_DIGITAL_SIGNATURE = 0x0008,
    INPUT_HAS_MULTIPLE_IMAGES   = 0x0010,
    INPUT_HAS_APNG              = 0x0020,
    INPUT_HAS_STRIPPED_DATA     = 0x0040,
    INPUT_HAS_JUNK              = 0x0080,
    INPUT_HAS_ERRORS            = 0x0100,
    OUTPUT_NEEDS_NEW_FILE       = 0x1000,
    OUTPUT_NEEDS_NEW_IDAT       = 0x2000,
    OUTPUT_HAS_ERRORS           = 0x4000
};

/*
 * The chunks handled by OptiPNG
 */
static const png_byte sig_PLTE[4] = { 0x50, 0x4c, 0x54, 0x45 };
static const png_byte sig_tRNS[4] = { 0x74, 0x52, 0x4e, 0x53 };
static const png_byte sig_IDAT[4] = { 0x49, 0x44, 0x41, 0x54 };
static const png_byte sig_IEND[4] = { 0x49, 0x45, 0x4e, 0x44 };
static const png_byte sig_bKGD[4] = { 0x62, 0x4b, 0x47, 0x44 };
static const png_byte sig_hIST[4] = { 0x68, 0x49, 0x53, 0x54 };
static const png_byte sig_sBIT[4] = { 0x73, 0x42, 0x49, 0x54 };
static const png_byte sig_dSIG[4] = { 0x64, 0x53, 0x49, 0x47 };
static const png_byte sig_acTL[4] = { 0x61, 0x63, 0x54, 0x4c };
static const png_byte sig_fcTL[4] = { 0x66, 0x63, 0x54, 0x4c };
static const png_byte sig_fdAT[4] = { 0x66, 0x64, 0x41, 0x54 };

/*
 * The optimization engine.
 * Since the engine is not multithreaded, there isn't much to put in here...
 */
static struct opng_engine_struct
{
    int started;
} engine;

/*
 * The optimization process
 */
static struct opng_process_struct
{
    unsigned int status;
    int num_iterations;
    osys_foffset_t in_datastream_offset;
    osys_fsize_t in_file_size, out_file_size;
    osys_fsize_t in_idat_size, out_idat_size;
    osys_fsize_t best_idat_size, max_idat_size;
    png_uint_32 in_plte_trns_size, out_plte_trns_size;
    png_uint_32 reductions;
    bitset_t compr_level_set, mem_level_set, strategy_set, filter_set;
    int best_compr_level, best_mem_level, best_strategy, best_filter;
} process;

/*
 * The optimization process limits
 */
static const osys_fsize_t idat_size_max = PNG_UINT_31_MAX;
static const char *idat_size_max_string = "2GB";

/*
 * The optimization process summary
 */
static struct opng_summary_struct
{
    unsigned int file_count;
    unsigned int err_count;
    unsigned int fix_count;
    unsigned int snip_count;
} summary;

/*
 * The optimized image
 */
static struct opng_image_struct
{
    png_uint_32 width;             /* IHDR */
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int compression_type;
    int filter_type;
    int interlace_type;
    png_bytepp row_pointers;       /* IDAT */
    png_colorp palette;            /* PLTE */
    int num_palette;
    png_color_16p background_ptr;  /* bKGD */
    png_color_16 background;
    png_uint_16p hist;             /* hIST */
    png_color_8p sig_bit_ptr;      /* sBIT */
    png_color_8 sig_bit;
    png_bytep trans_alpha;         /* tRNS */
    int num_trans;
    png_color_16p trans_color_ptr;
    png_color_16 trans_color;
    png_unknown_chunkp unknowns;   /* everything else */
    int num_unknowns;
} image;

/*
 * The user options
 */
static struct opng_options options;


/*
 * The user interface
 */
static void (*usr_printf)(const char *fmt, ...);
static void (*usr_print_cntrl)(int cntrl_code);
static void (*usr_progress)(unsigned long num, unsigned long denom);
static void (*usr_panic)(const char *msg);


/*
 * More global variables, for quick access and bonus style points
 */
static png_structp read_ptr;
static png_infop read_info_ptr;
static png_structp write_ptr;
static png_infop write_info_ptr;


/*
 * Internal debugging tool
 */
#define OPNG_ENSURE(cond, msg) \
    { if (!(cond)) usr_panic(msg); }  /* strong check, no #ifdef's */


/*
 * Size ratio display
 */
static void
opng_print_fsize_ratio(osys_fsize_t num, osys_fsize_t denom, int force_percent)
{
    /* (1) num/denom = 0/0                  ==> print "??%"
     * (2) num/denom = INFINITY             ==> print "INFTY%"
     * (3) 0 <= num/denom < 99.995%         ==> use the percent format "99.99%"
     *     if force_percent:
     * (4)    0.995 <= num/denom < INFINITY ==> use the percent format "999%"
     *     else:
     * (5)    0.995 <= num/denom < 99.995   ==> use the factor format "9.99x"
     * (6)    99.5 <= num/denom < INFINITY  ==> use the factor format "999x"
     *     end if
     */

    osys_fsize_t integer_part, remainder;
    unsigned int fractional_part, scale;
    double scaled_ratio;

    /* (1,2): num/denom = 0/0 or num/denom = INFINITY */
    if (denom == 0)
    {
        usr_printf(num == 0 ? "??%%" : "INFTY%%");
        return;
    }

    /* (3): 0 <= num/denom < 99.995% */
    /* num/denom < 99.995% <==> denom/(denom-num) < 20000 */
    if (num < denom && denom / (denom - num) < 20000)
    {
        scale = 10000;
        scaled_ratio = ((double)num * (double)scale) / (double)denom;
        fractional_part = (unsigned int)(scaled_ratio + 0.5);
        /* Adjust the scaled result in the event of a roundoff error. */
        /* Such error may occur only if the numerator is extremely large. */
        if (fractional_part >= scale)
            fractional_part = scale - 1;
        usr_printf("%u.%02u%%", fractional_part / 100, fractional_part % 100);
        return;
    }

    /* Extract the integer part out of the fraction for the remaining cases. */
    integer_part = num / denom;
    remainder = num % denom;
    scale = 100;
    scaled_ratio = ((double)remainder * (double)scale) / (double)denom;
    fractional_part = (unsigned int)(scaled_ratio + 0.5);
    if (fractional_part >= scale)
    {
        fractional_part = 0;
        ++integer_part;
    }

    /* (4): 0.995 <= num/denom < INFINITY */
    if (force_percent)
    {
        usr_printf("%" OSYS_FSIZE_PRIu "%02u%%",
                   integer_part, fractional_part);
        return;
    }

    /* (5): 0.995 <= num/denom < 99.995 */
    if (integer_part < 100)
    {
        usr_printf("%" OSYS_FSIZE_PRIu ".%02ux",
                   integer_part, fractional_part);
        return;
    }

    /* (6): 99.5 <= num/denom < INFINITY */
    /* Round to the nearest integer. */
    /* Recalculate the integer part, for corner cases like 123.999. */
    integer_part = num / denom;
    if (remainder > (denom - 1) / 2)
        ++integer_part;
    usr_printf("%" OSYS_FSIZE_PRIu "x", integer_part);
}

/*
 * Size change display
 */
static void
opng_print_fsize_difference(osys_fsize_t init_size, osys_fsize_t final_size,
                            int show_ratio)
{
    osys_fsize_t difference;
    int sign;

    if (init_size <= final_size)
    {
        sign = 0;
        difference = final_size - init_size;
    }
    else
    {
        sign = 1;
        difference = init_size - final_size;
    }

    if (difference == 0)
    {
        usr_printf("no change");
        return;
    }
    if (difference == 1)
        usr_printf("1 byte");
    else
        usr_printf("%" OSYS_FSIZE_PRIu " bytes", difference);
    if (show_ratio && init_size > 0)
    {
        usr_printf(" = ");
        opng_print_fsize_ratio(difference, init_size, 0);
    }
    usr_printf(sign == 0 ? " increase" : " decrease");
}

/*
 * Image info display
 */
static void
opng_print_image_info(int show_dim, int show_depth, int show_type,
                      int show_interlaced)
{
    static const int type_channels[8] = {1, 0, 3, 1, 2, 0, 4, 0};
    int channels, printed;

    printed = 0;
    if (show_dim)
    {
        printed = 1;
        usr_printf("%lux%lu pixels",
                   (unsigned long)image.width, (unsigned long)image.height);
    }
    if (show_depth)
    {
        if (printed)
            usr_printf(", ");
        printed = 1;
        channels = type_channels[image.color_type & 7];
        if (channels != 1)
            usr_printf("%dx%d bits/pixel", channels, image.bit_depth);
        else if (image.bit_depth != 1)
            usr_printf("%d bits/pixel", image.bit_depth);
        else
            usr_printf("1 bit/pixel");
    }
    if (show_type)
    {
        if (printed)
            usr_printf(", ");
        printed = 1;
        if (image.color_type & PNG_COLOR_MASK_PALETTE)
        {
            if (image.num_palette == 1)
                usr_printf("1 color");
            else
                usr_printf("%d colors", image.num_palette);
            if (image.num_trans > 0)
                usr_printf(" (%d transparent)", image.num_trans);
            usr_printf(" in palette");
        }
        else
        {
            usr_printf((image.color_type & PNG_COLOR_MASK_COLOR) ?
                       "RGB" : "grayscale");
            if (image.color_type & PNG_COLOR_MASK_ALPHA)
                usr_printf("+alpha");
            else if (image.trans_color_ptr != NULL)
                usr_printf("+transparency");
        }
    }
    if (show_interlaced)
    {
        if (image.interlace_type != PNG_INTERLACE_NONE)
        {
            if (printed)
                usr_printf(", ");
            usr_printf("interlaced");
        }
    }
}

/*
 * Warning display
 */
static void
opng_print_warning(const char *msg)
{
    usr_print_cntrl('\v');  /* VT: new paragraph */
    usr_printf("Warning: %s\n", msg);
}

/*
 * Error display
 */
static void
opng_print_error(const char *msg)
{
    usr_print_cntrl('\v');  /* VT: new paragraph */
    usr_printf("Error: %s\n", msg);
}

/*
 * Warning handler
 */
static void
opng_warning(png_structp png_ptr, png_const_charp msg)
{
    /* Error in input or output file; processing may continue. */
    /* Recovery requires (re)compression of IDAT. */
    if (png_ptr == read_ptr)
        process.status |= (INPUT_HAS_ERRORS | OUTPUT_NEEDS_NEW_IDAT);
    opng_print_warning(msg);
}

/*
 * Error handler
 */
static void
opng_error(png_structp png_ptr, png_const_charp msg)
{
    /* Error in input or output file; processing must stop. */
    /* Recovery requires (re)compression of IDAT. */
    if (png_ptr == read_ptr)
        process.status |= (INPUT_HAS_ERRORS | OUTPUT_NEEDS_NEW_IDAT);
    Throw msg;
}

/*
 * Memory deallocator
 */
static void
opng_free(void *ptr)
{
    /* This deallocator must be compatible with libpng's memory allocation
     * routines, png_malloc() and png_free().
     * If those routines change, this one must be changed accordingly.
     */
    free(ptr);
}

/*
 * IDAT size checker
 */
static void
opng_check_idat_size(osys_fsize_t size)
{
    if (size > idat_size_max)
        Throw "IDAT sizes larger than the maximum chunk size "
              "are currently unsupported";
}

/*
 * Chunk handler
 */
static void
opng_set_keep_unknown_chunk(png_structp png_ptr,
                            int keep, png_bytep chunk_type)
{
    png_byte chunk_name[5];

    /* Call png_set_keep_unknown_chunks() once per each chunk type only. */
    memcpy(chunk_name, chunk_type, 4);
    chunk_name[4] = 0;
    if (!png_handle_as_unknown(png_ptr, chunk_name))
        png_set_keep_unknown_chunks(png_ptr, keep, chunk_name, 1);
}

/*
 * Chunk categorization
 */
static int
opng_is_image_chunk(png_bytep chunk_type)
{
    if ((chunk_type[0] & 0x20) == 0)
        return 1;
    /* Although tRNS is listed as ancillary in the PNG specification, it stores
     * alpha samples, which is critical information. For example, tRNS cannot
     * be generally ignored when rendering animations.
     * Operations claimed to be lossless must treat tRNS as a critical chunk.
     */
    if (memcmp(chunk_type, sig_tRNS, 4) == 0)
        return 1;
    return 0;
}

/*
 * Chunk categorization
 */
static int
opng_is_apng_chunk(png_bytep chunk_type)
{
    if (memcmp(chunk_type, sig_acTL, 4) == 0 ||
        memcmp(chunk_type, sig_fcTL, 4) == 0 ||
        memcmp(chunk_type, sig_fdAT, 4) == 0)
        return 1;
    return 0;
}

/*
 * Chunk filter
 */
static int
opng_allow_chunk(png_bytep chunk_type)
{
    /* Always allow critical chunks and tRNS. */
    if (opng_is_image_chunk(chunk_type))
        return 1;
    /* Block all the other chunks if requested. */
    if (options.strip_all)
        return 0;
    /* Always block the digital signature chunks. */
    if (memcmp(chunk_type, sig_dSIG, 4) == 0)
        return 0;
    /* Block the APNG chunks when snipping. */
    if (options.snip && opng_is_apng_chunk(chunk_type))
        return 0;
    /* Allow all the other chunks. */
    return 1;
}

/*
 * Chunk handler
 */
static void
opng_handle_chunk(png_structp png_ptr, png_bytep chunk_type)
{
    int keep;

    if (opng_is_image_chunk(chunk_type))
        return;

    if (options.strip_all)
    {
        process.status |= INPUT_HAS_STRIPPED_DATA | INPUT_HAS_JUNK;
        opng_set_keep_unknown_chunk(png_ptr,
                                    PNG_HANDLE_CHUNK_NEVER, chunk_type);
        return;
    }

    /* Let libpng handle bKGD, hIST and sBIT. */
    if (memcmp(chunk_type, sig_bKGD, 4) == 0 ||
        memcmp(chunk_type, sig_hIST, 4) == 0 ||
        memcmp(chunk_type, sig_sBIT, 4) == 0)
        return;

    /* Everything else is handled as unknown by libpng. */
    keep = PNG_HANDLE_CHUNK_ALWAYS;
    if (memcmp(chunk_type, sig_dSIG, 4) == 0)  /* digital signature? */
        process.status |= INPUT_HAS_DIGITAL_SIGNATURE;
    else if (opng_is_apng_chunk(chunk_type))  /* APNG? */
    {
        process.status |= INPUT_HAS_APNG;
        if (memcmp(chunk_type, sig_fdAT, 4) == 0)
            process.status |= INPUT_HAS_MULTIPLE_IMAGES;
        if (options.snip)
        {
            process.status |= INPUT_HAS_JUNK;
            keep = PNG_HANDLE_CHUNK_NEVER;
        }
    }
    opng_set_keep_unknown_chunk(png_ptr, keep, chunk_type);
}

/*
 * Initialization for input handler
 */
static void
opng_init_read_data(void)
{
    /* The relevant process data members are set to zero,
     * and nothing else needs to be done at this moment.
     */
}

/*
 * Initialization for output handler
 */
static void
opng_init_write_data(void)
{
    process.out_file_size = 0;
    process.out_plte_trns_size = 0;
    process.out_idat_size = 0;
}

/*
 * Input handler
 */
static void
opng_read_data(png_structp png_ptr, png_bytep data, size_t length)
{
    FILE *stream = (FILE *)png_get_io_ptr(png_ptr);
    int io_state = pngx_get_io_state(png_ptr);
    int io_state_loc = io_state & PNGX_IO_MASK_LOC;
    png_bytep chunk_sig;

    /* Read the data. */
    if (fread(data, 1, length, stream) != length)
        png_error(png_ptr,
            "Can't read the input file or unexpected end of file");

    if (process.in_file_size == 0)  /* first piece of PNG data */
    {
        OPNG_ENSURE(length == 8, "PNG I/O must start with the first 8 bytes");
        process.in_datastream_offset = osys_ftello(stream) - 8;
        process.status |= INPUT_HAS_PNG_DATASTREAM;
        if (io_state_loc == PNGX_IO_SIGNATURE)
            process.status |= INPUT_HAS_PNG_SIGNATURE;
        if (process.in_datastream_offset == 0)
            process.status |= INPUT_IS_PNG_FILE;
        else if (process.in_datastream_offset < 0)
            png_error(png_ptr,
                "Can't get the file-position indicator in input file");
        process.in_file_size = (osys_fsize_t)process.in_datastream_offset;
    }
    process.in_file_size += length;

    /* Handle the OptiPNG-specific events. */
    OPNG_ENSURE((io_state & PNGX_IO_READING) && (io_state_loc != 0),
                "Incorrect info in png_ptr->io_state");
    if (io_state_loc == PNGX_IO_CHUNK_HDR)
    {
        /* In libpng 1.4.x and later, the chunk length and the chunk name
         * are serialized in a single operation. This is also ensured by
         * the opngio add-on for libpng 1.2.x and earlier.
         */
        OPNG_ENSURE(length == 8, "Reading chunk header, expecting 8 bytes");
        chunk_sig = data + 4;

        if (memcmp(chunk_sig, sig_IDAT, 4) == 0)
        {
            OPNG_ENSURE(png_ptr == read_ptr, "Incorrect I/O handler setup");
            if (png_get_rows(read_ptr, read_info_ptr) == NULL)  /* 1st IDAT */
            {
                OPNG_ENSURE(process.in_idat_size == 0,
                            "Found IDAT with no rows");
                /* Allocate the rows here, bypassing libpng.
                 * This allows to initialize the contents and perform recovery
                 * in case of a premature EOF.
                 */
                if (png_get_image_height(read_ptr, read_info_ptr) == 0)
                    return;  /* premature IDAT; an error will occur later */
                OPNG_ENSURE(pngx_malloc_rows(read_ptr,
                                             read_info_ptr, 0) != NULL,
                            "Failed allocation of image rows; "
                            "unsafe libpng allocator");
                png_data_freer(read_ptr, read_info_ptr,
                               PNG_USER_WILL_FREE_DATA, PNG_FREE_ROWS);
            }
            else
            {
                /* There is split IDAT overhead. Join IDATs. */
                process.status |= INPUT_HAS_JUNK;
            }
            process.in_idat_size += png_get_uint_32(data);
        }
        else if (memcmp(chunk_sig, sig_PLTE, 4) == 0 ||
                 memcmp(chunk_sig, sig_tRNS, 4) == 0)
        {
            /* Add the chunk overhead (header + CRC) to the data size. */
            process.in_plte_trns_size += png_get_uint_32(data) + 12;
        }
        else
            opng_handle_chunk(png_ptr, chunk_sig);
    }
    else if (io_state_loc == PNGX_IO_CHUNK_CRC)
    {
        OPNG_ENSURE(length == 4, "Reading chunk CRC, expecting 4 bytes");
    }
}

/*
 * Output handler
 */
static void
opng_write_data(png_structp png_ptr, png_bytep data, size_t length)
{
    static int allow_crt_chunk;
    static int crt_chunk_is_idat;
    static osys_foffset_t crt_idat_offset;
    static osys_fsize_t crt_idat_size; 
    static png_uint_32 crt_idat_crc;
    FILE *stream = (FILE *)png_get_io_ptr(png_ptr);
    int io_state = pngx_get_io_state(png_ptr);
    int io_state_loc = io_state & PNGX_IO_MASK_LOC;
    png_bytep chunk_sig;
    png_byte buf[4];

    OPNG_ENSURE((io_state & PNGX_IO_WRITING) && (io_state_loc != 0),
                "Incorrect info in png_ptr->io_state");

    /* Handle the OptiPNG-specific events. */
    if (io_state_loc == PNGX_IO_CHUNK_HDR)
    {
        OPNG_ENSURE(length == 8, "Writing chunk header, expecting 8 bytes");
        chunk_sig = data + 4;
        allow_crt_chunk = opng_allow_chunk(chunk_sig);
        if (memcmp(chunk_sig, sig_IDAT, 4) == 0)
        {
            crt_chunk_is_idat = 1;
            process.out_idat_size += png_get_uint_32(data);
            /* Abandon the trial if IDAT is bigger than the maximum allowed. */
            if (stream == NULL)
            {
                if (process.out_idat_size > process.max_idat_size)
                    Throw NULL;  /* early interruption, not an error */
            }
        }
        else  /* not IDAT */
        {
            crt_chunk_is_idat = 0;
            if (memcmp(chunk_sig, sig_PLTE, 4) == 0 ||
                memcmp(chunk_sig, sig_tRNS, 4) == 0)
            {
                /* Add the chunk overhead (header + CRC) to the data size. */
                process.out_plte_trns_size += png_get_uint_32(data) + 12;
            }
        }
    }
    else if (io_state_loc == PNGX_IO_CHUNK_CRC)
    {
        OPNG_ENSURE(length == 4, "Writing chunk CRC, expecting 4 bytes");
    }

    /* Exit early if this is only a trial. */
    if (stream == NULL)
        return;

    /* Continue only if the current chunk type is allowed. */
    if (io_state_loc != PNGX_IO_SIGNATURE && !allow_crt_chunk)
        return;

    /* Here comes an elaborate way of writing the data, in which all IDATs
     * are joined into a single chunk.
     * Normally, the user-supplied I/O routines are not so complicated.
     */
    switch (io_state_loc)
    {
        case PNGX_IO_CHUNK_HDR:
        {
            if (crt_chunk_is_idat)
            {
                if (crt_idat_offset == 0)
                {
                    /* This is the header of the first IDAT. */
                    crt_idat_offset = osys_ftello(stream);
                    /* Try guessing the size of the final (joined) IDAT. */
                    if (process.best_idat_size > 0)
                    {
                        /* The guess is expected to be right. */
                        crt_idat_size = process.best_idat_size;
                    }
                    else
                    {
                        /* The guess could be wrong.
                         * The size of the final IDAT will be revised.
                         */
                        crt_idat_size = length;
                    }
                    png_save_uint_32(data, (png_uint_32)crt_idat_size);
                    /* Start computing the CRC of the final IDAT. */
                    crt_idat_crc = crc32(0, sig_IDAT, 4);
                }
                else
                {
                    /* This is not the first IDAT. Do not write its header. */
                    return;
                }
            }
            else
            {
                if (crt_idat_offset != 0)
                {
                    /* This is the header of the first chunk after IDAT.
                     * Finalize IDAT before resuming the normal operation.
                     */
                    png_save_uint_32(buf, crt_idat_crc);
                    if (fwrite(buf, 1, 4, stream) != 4)
                        io_state = 0;  /* error */
                    process.out_file_size += 4;
                    if (process.out_idat_size != crt_idat_size)
                    {
                        /* The IDAT size has not been guessed correctly.
                         * It must be updated in a non-streamable way.
                         */
                        OPNG_ENSURE(process.best_idat_size == 0,
                                    "Wrong guess of the output IDAT size");
                        opng_check_idat_size(process.out_idat_size);
                        png_save_uint_32(buf,
                                         (png_uint_32)process.out_idat_size);
                        if (osys_fwrite_at(stream, crt_idat_offset, SEEK_SET,
                                           buf, 4) != 4)
                            io_state = 0;  /* error */
                    }
                    if (io_state == 0)
                        png_error(png_ptr, "Can't finalize IDAT");
                    crt_idat_offset = 0;
                }
            }
            break;
        }
        case PNGX_IO_CHUNK_DATA:
        {
            if (crt_chunk_is_idat)
                crt_idat_crc = crc32(crt_idat_crc, data, length);
            break;
        }
        case PNGX_IO_CHUNK_CRC:
        {
            if (crt_chunk_is_idat)
                return;  /* defer writing until the first non-IDAT occurs */
            break;
        }
    }

    /* Write the data. */
    if (fwrite(data, 1, length, stream) != length)
        png_error(png_ptr, "Can't write the output file");
    process.out_file_size += length;
}

/*
 * Image info initialization
 */
static void
opng_clear_image_info(void)
{
    memset(&image, 0, sizeof(image));
}

/*
 * Image info transfer
 */
static void
opng_load_image_info(png_structp png_ptr, png_infop info_ptr, int load_meta)
{
    memset(&image, 0, sizeof(image));

    png_get_IHDR(png_ptr, info_ptr,
        &image.width, &image.height, &image.bit_depth, &image.color_type,
        &image.interlace_type, &image.compression_type, &image.filter_type);
    image.row_pointers = png_get_rows(png_ptr, info_ptr);
    png_get_PLTE(png_ptr, info_ptr, &image.palette, &image.num_palette);
    /* Transparency is not considered metadata, although tRNS is ancillary.
     * See the comment in opng_is_image_chunk() above.
     */
    if (png_get_tRNS(png_ptr, info_ptr,
        &image.trans_alpha, &image.num_trans, &image.trans_color_ptr))
    {
        /* Double copying (pointer + value) is necessary here
         * due to an inconsistency in the libpng design.
         */
        if (image.trans_color_ptr != NULL)
        {
            image.trans_color = *image.trans_color_ptr;
            image.trans_color_ptr = &image.trans_color;
        }
    }

    if (!load_meta)
        return;

    if (png_get_bKGD(png_ptr, info_ptr, &image.background_ptr))
    {
        /* Same problem as in tRNS. */
        image.background = *image.background_ptr;
        image.background_ptr = &image.background;
    }
    png_get_hIST(png_ptr, info_ptr, &image.hist);
    if (png_get_sBIT(png_ptr, info_ptr, &image.sig_bit_ptr))
    {
        /* Same problem as in tRNS. */
        image.sig_bit = *image.sig_bit_ptr;
        image.sig_bit_ptr = &image.sig_bit;
    }
    image.num_unknowns =
        png_get_unknown_chunks(png_ptr, info_ptr, &image.unknowns);
}

/*
 * Image info transfer
 */
static void
opng_store_image_info(png_structp png_ptr, png_infop info_ptr, int store_meta)
{
    OPNG_ENSURE(image.row_pointers != NULL, "No info in image");

    png_set_IHDR(png_ptr, info_ptr,
        image.width, image.height, image.bit_depth, image.color_type,
        image.interlace_type, image.compression_type, image.filter_type);
    png_set_rows(write_ptr, write_info_ptr, image.row_pointers);
    if (image.palette != NULL)
        png_set_PLTE(png_ptr, info_ptr, image.palette, image.num_palette);
    /* Transparency is not considered metadata, although tRNS is ancillary.
     * See the comment in opng_is_image_chunk() above.
     */
    if (image.trans_alpha != NULL || image.trans_color_ptr != NULL)
        png_set_tRNS(png_ptr, info_ptr,
            image.trans_alpha, image.num_trans, image.trans_color_ptr);

    if (!store_meta)
        return;

    if (image.background_ptr != NULL)
        png_set_bKGD(png_ptr, info_ptr, image.background_ptr);
    if (image.hist != NULL)
        png_set_hIST(png_ptr, info_ptr, image.hist);
    if (image.sig_bit_ptr != NULL)
        png_set_sBIT(png_ptr, info_ptr, image.sig_bit_ptr);
    if (image.num_unknowns != 0)
    {
        int i;
        png_set_unknown_chunks(png_ptr, info_ptr,
            image.unknowns, image.num_unknowns);
        /* Is this really necessary? Should it not be implemented in libpng? */
        for (i = 0; i < image.num_unknowns; ++i)
            png_set_unknown_chunk_location(png_ptr, info_ptr,
                i, image.unknowns[i].location);
    }
}

/*
 * Image info destruction
 */
static void
opng_destroy_image_info(void)
{
    png_uint_32 i;
    int j;

    if (image.row_pointers == NULL)
        return;  /* nothing to clean up */

    for (i = 0; i < image.height; ++i)
        opng_free(image.row_pointers[i]);
    opng_free(image.row_pointers);
    opng_free(image.palette);
    opng_free(image.trans_alpha);
    opng_free(image.hist);
    for (j = 0; j < image.num_unknowns; ++j)
        opng_free(image.unknowns[j].data);
    opng_free(image.unknowns);
    /* DO NOT deallocate background_ptr, sig_bit_ptr, trans_color_ptr.
     * See the comments regarding double copying inside opng_load_image_info().
     */

    /* Clear the space here and do not worry about double-deallocation issues
     * that might arise later on.
     */
    memset(&image, 0, sizeof(image));
}

/*
 * Image file reading
 */
static void
opng_read_file(FILE *infile)
{
    const char *fmt_name;
    int num_img;
    png_uint_32 reductions;
    const char * volatile err_msg;  /* volatile is required by cexcept */

    Try
    {
        read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
            opng_error, opng_warning);
        read_info_ptr = png_create_info_struct(read_ptr);
        if (read_info_ptr == NULL)
            Throw "Out of memory";

        png_set_keep_unknown_chunks(read_ptr,
                                    PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);

        /* Read the input image file. */
        opng_init_read_data();
        pngx_set_read_fn(read_ptr, infile, opng_read_data);
        fmt_name = NULL;
        num_img = pngx_read_image(read_ptr, read_info_ptr, &fmt_name, NULL);
        if (num_img <= 0)
            Throw "Unrecognized image file format";
        if (num_img > 1)
            process.status |= INPUT_HAS_MULTIPLE_IMAGES;
        if ((process.status & INPUT_IS_PNG_FILE) &&
            (process.status & INPUT_HAS_MULTIPLE_IMAGES))
        {
            /* pngxtern can't distinguish between APNG and proper PNG. */
            fmt_name = (process.status & INPUT_HAS_PNG_SIGNATURE) ?
                       "APNG" : "APNG datastream";
        }
        OPNG_ENSURE(fmt_name != NULL, "No format name from pngxtern");

        if (process.in_file_size == 0)
        {
            if (osys_fgetsize(infile, &process.in_file_size) < 0)
            {
                opng_print_warning("Can't get the correct file size");
                process.in_file_size = 0;
            }
        }

        err_msg = NULL;  /* everything is ok */
    }
    Catch (err_msg)
    {
        /* If the critical info has been loaded, treat all errors as warnings.
         * This enables a more advanced data recovery.
         */
        if (opng_validate_image(read_ptr, read_info_ptr))
        {
           png_warning(read_ptr, err_msg);
           err_msg = NULL;
        }
    }

    Try
    {
        if (err_msg != NULL)
            Throw err_msg;

        /* Display format and image information. */
        if (strcmp(fmt_name, "PNG") != 0)
        {
            usr_printf("Importing %s", fmt_name);
            if (process.status & INPUT_HAS_MULTIPLE_IMAGES)
            {
                if (!(process.status & INPUT_IS_PNG_FILE))
                    usr_printf(" (multi-image or animation)");
                if (options.snip)
                    usr_printf("; snipping...");
            }
            usr_printf("\n");
        }
        opng_load_image_info(read_ptr, read_info_ptr, 1);
        opng_print_image_info(1, 1, 1, 1);
        usr_printf("\n");

        /* Choose the applicable image reductions. */
        reductions = OPNG_REDUCE_ALL & ~OPNG_REDUCE_METADATA;
        if (options.nb)
            reductions &= ~OPNG_REDUCE_BIT_DEPTH;
        if (options.nc)
            reductions &= ~OPNG_REDUCE_COLOR_TYPE;
        if (options.np)
            reductions &= ~OPNG_REDUCE_PALETTE;
        if (options.nz && (process.status & INPUT_HAS_PNG_DATASTREAM))
        {
            /* Do not reduce files with PNG datastreams under -nz. */
            reductions = OPNG_REDUCE_NONE;
        }
        if (process.status & INPUT_HAS_DIGITAL_SIGNATURE)
        {
            /* Do not reduce signed files. */
            reductions = OPNG_REDUCE_NONE;
        }
        if ((process.status & INPUT_IS_PNG_FILE) &&
            (process.status & INPUT_HAS_MULTIPLE_IMAGES) &&
            (reductions != OPNG_REDUCE_NONE) && !options.snip)
        {
            usr_printf(
                "Can't reliably reduce APNG file; disabling reductions.\n"
                "(Did you want to -snip and optimize the first frame?)\n");
            reductions = OPNG_REDUCE_NONE;
        }

        /* Try to reduce the image. */
        process.reductions =
            opng_reduce_image(read_ptr, read_info_ptr, reductions);

        /* If the image is reduced, enforce full compression. */
        if (process.reductions != OPNG_REDUCE_NONE)
        {
            opng_load_image_info(read_ptr, read_info_ptr, 1);
            usr_printf("Reducing image to ");
            opng_print_image_info(0, 1, 1, 0);
            usr_printf("\n");
        }

        /* Change the interlace type if required. */
        if (options.interlace >= 0 &&
            image.interlace_type != options.interlace)
        {
            image.interlace_type = options.interlace;
            /* A change in interlacing requires IDAT recoding. */
            process.status |= OUTPUT_NEEDS_NEW_IDAT;
        }
    }
    Catch (err_msg)
    {
        /* Do the cleanup, then rethrow the exception. */
        png_data_freer(read_ptr, read_info_ptr,
                       PNG_DESTROY_WILL_FREE_DATA, PNG_FREE_ALL);
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        Throw err_msg;
    }

    /* Destroy the libpng structures, but leave the enclosed data intact
     * to allow further processing.
     */
    png_data_freer(read_ptr, read_info_ptr,
                   PNG_USER_WILL_FREE_DATA, PNG_FREE_ALL);
    png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
}

/*
 * PNG file writing
 *
 * If the output file is NULL, PNG encoding is still done,
 * but no file is written.
 */
static void
opng_write_file(FILE *outfile,
                int compression_level, int memory_level,
                int compression_strategy, int filter)
{
    const char * volatile err_msg;  /* volatile is required by cexcept */

    OPNG_ENSURE(
        compression_level >= OPNG_COMPR_LEVEL_MIN &&
        compression_level <= OPNG_COMPR_LEVEL_MAX &&
        memory_level >= OPNG_MEM_LEVEL_MIN &&
        memory_level <= OPNG_MEM_LEVEL_MAX &&
        compression_strategy >= OPNG_STRATEGY_MIN &&
        compression_strategy <= OPNG_STRATEGY_MAX &&
        filter >= OPNG_FILTER_MIN &&
        filter <= OPNG_FILTER_MAX,
        "Invalid encoding parameters");

    Try
    {
        write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
            NULL, opng_error, opng_warning);
        write_info_ptr = png_create_info_struct(write_ptr);
        if (write_info_ptr == NULL)
            Throw "Out of memory";

        png_set_compression_level(write_ptr, compression_level);
        png_set_compression_mem_level(write_ptr, memory_level);
        png_set_compression_strategy(write_ptr, compression_strategy);
        png_set_filter(write_ptr, PNG_FILTER_TYPE_BASE, filter_table[filter]);
        if (compression_strategy != Z_HUFFMAN_ONLY &&
            compression_strategy != Z_RLE)
        {
            if (options.window_bits > 0)
                png_set_compression_window_bits(write_ptr,
                                                options.window_bits);
        }
        else
        {
#ifdef WBITS_8_OK
            png_set_compression_window_bits(write_ptr, 8);
#else
            png_set_compression_window_bits(write_ptr, 9);
#endif
        }
        png_set_keep_unknown_chunks(write_ptr,
                                    PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);
        opng_store_image_info(write_ptr, write_info_ptr, (outfile != NULL));

        /* Write the PNG stream. */
        opng_init_write_data();
        pngx_set_write_fn(write_ptr, outfile, opng_write_data, NULL);
        png_write_png(write_ptr, write_info_ptr, 0, NULL);

        err_msg = NULL;  /* everything is ok */
    }
    Catch (err_msg)
    {
        /* Set IDAT size to invalid. */
        process.out_idat_size = idat_size_max + 1;
    }

    /* Destroy the libpng structures. */
    png_destroy_write_struct(&write_ptr, &write_info_ptr);

    if (err_msg != NULL)
        Throw err_msg;
}

/*
 * PNG file copying
 */
static void
opng_copy_file(FILE *infile, FILE *outfile)
{
    volatile png_bytep buf;  /* volatile is required by cexcept */
    const png_uint_32 buf_size_incr = 0x1000;
    png_uint_32 buf_size, length;
    png_byte chunk_hdr[8];
    const char * volatile err_msg;

    write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, opng_error, opng_warning);
    if (write_ptr == NULL)
        Throw "Out of memory";
    opng_init_write_data();
    pngx_set_write_fn(write_ptr, outfile, opng_write_data, NULL);

    Try
    {
        buf = NULL;
        buf_size = 0;

        /* Write the signature in the output file. */
        pngx_write_sig(write_ptr);

        /* Copy all chunks until IEND. */
        /* Error checking is done only at a very basic level. */
        do
        {
            if (fread(chunk_hdr, 8, 1, infile) != 1)  /* length + name */
                Throw "Read error";
            length = png_get_uint_32(chunk_hdr);
            if (length > PNG_UINT_31_MAX)
            {
                if (buf == NULL && length == 0x89504e47UL)  /* "\x89PNG" */
                    continue;  /* skip the signature */
                Throw "Data error";
            }
            if (length + 4 > buf_size)
            {
                png_free(write_ptr, buf);
                buf_size = (((length + 4) + (buf_size_incr - 1))
                            / buf_size_incr) * buf_size_incr;
                buf = (png_bytep)png_malloc(write_ptr, buf_size);
                /* Do not use realloc() here, it's slower. */
            }
            if (fread(buf, length + 4, 1, infile) != 1)  /* data + crc */
                Throw "Read error";
            png_write_chunk(write_ptr, chunk_hdr + 4, buf, length);
        } while (memcmp(chunk_hdr + 4, sig_IEND, 4) != 0);

        err_msg = NULL;  /* everything is ok */
    }
    Catch (err_msg)
    {
    }

    png_free(write_ptr, buf);
    png_destroy_write_struct(&write_ptr, NULL);

    if (err_msg != NULL)
        Throw err_msg;
}

/*
 * Iteration initialization
 */
static void
opng_init_iteration(bitset_t cmdline_set, bitset_t mask_set,
                    const char *preset, bitset_t *output_set)
{
    bitset_t preset_set;

    *output_set = cmdline_set & mask_set;
    if (*output_set == BITSET_EMPTY && cmdline_set != BITSET_EMPTY)
        Throw "Iteration parameter(s) out of range";
    if (*output_set == BITSET_EMPTY || options.optim_level >= 0)
    {
        preset_set = rangeset_string_to_bitset(preset, NULL);
        *output_set |= preset_set & mask_set;
    }
}

/*
 * Iteration initialization
 */
static void
opng_init_iterations(void)
{
    bitset_t compr_level_set, mem_level_set, strategy_set, filter_set;
    int preset_index;
    int t1, t2;

    /* Set the IDAT size limit. The trials that pass this limit will be
     * abandoned, as there will be no need to wait until their completion.
     * This limit may further decrease as iterations go on.
     */
    if ((process.status & OUTPUT_NEEDS_NEW_IDAT) || options.full)
        process.max_idat_size = idat_size_max;
    else
    {
        OPNG_ENSURE(process.in_idat_size > 0, "No IDAT in input");
        /* Add the input PLTE and tRNS sizes to the initial max IDAT size,
         * to account for the changes that may occur during reduction.
         * This incurs a negligible overhead on processing only: the final
         * IDAT size will not be affected, because a precise check will be
         * performed at the end, inside opng_finish_iterations().
         */
        process.max_idat_size =
            process.in_idat_size + process.in_plte_trns_size;
    }

    /* Get preset_index from options.optim_level, but leave the latter intact,
     * because the effect of "optipng -o2 -z... -f..." is slightly different
     * than the effect of "optipng -z... -f..." (without "-o").
     */
    preset_index = options.optim_level;
    if (preset_index < 0)
        preset_index = OPNG_OPTIM_LEVEL_DEFAULT;
    else if (preset_index > OPNG_OPTIM_LEVEL_MAX)
        preset_index = OPNG_OPTIM_LEVEL_MAX;

    /* Initialize the iteration sets.
     * Combine the user-defined values with the optimization presets.
     */
    opng_init_iteration(options.compr_level_set, OPNG_COMPR_LEVEL_SET_MASK,
        compr_level_presets[preset_index], &compr_level_set);
    opng_init_iteration(options.mem_level_set, OPNG_MEM_LEVEL_SET_MASK,
        mem_level_presets[preset_index], &mem_level_set);
    opng_init_iteration(options.strategy_set, OPNG_STRATEGY_SET_MASK,
        strategy_presets[preset_index], &strategy_set);
    opng_init_iteration(options.filter_set, OPNG_FILTER_SET_MASK,
        filter_presets[preset_index], &filter_set);

    /* Replace the empty sets with the libpng's "best guess" heuristics. */
    if (compr_level_set == BITSET_EMPTY)
        bitset_set(&compr_level_set, Z_BEST_COMPRESSION);  /* -zc9 */
    if (mem_level_set == BITSET_EMPTY)
        bitset_set(&mem_level_set, 8);
    if (image.bit_depth < 8 || image.palette != NULL)
    {
        if (strategy_set == BITSET_EMPTY)
            bitset_set(&strategy_set, Z_DEFAULT_STRATEGY);  /* -zs0 */
        if (filter_set == BITSET_EMPTY)
            bitset_set(&filter_set, 0);  /* -f0 */
    }
    else
    {
        if (strategy_set == BITSET_EMPTY)
            bitset_set(&strategy_set, Z_FILTERED);  /* -zs1 */
        if (filter_set == BITSET_EMPTY)
            bitset_set(&filter_set, 5);  /* -f0 */
    }

    /* Store the results into process. */
    process.compr_level_set = compr_level_set;
    process.mem_level_set   = mem_level_set;
    process.strategy_set    = strategy_set;
    process.filter_set      = filter_set;
    t1 = bitset_count(compr_level_set) *
         bitset_count(strategy_set & ~((1 << Z_HUFFMAN_ONLY) | (1 << Z_RLE)));
    t2 = bitset_count(strategy_set &  ((1 << Z_HUFFMAN_ONLY) | (1 << Z_RLE)));
    process.num_iterations =
        (t1 + t2) * bitset_count(mem_level_set) * bitset_count(filter_set);
    OPNG_ENSURE(process.num_iterations > 0, "Invalid iteration parameters");
}

/*
 * Iteration
 */
static void
opng_iterate(void)
{
    bitset_t compr_level_set, mem_level_set, strategy_set, filter_set;
    bitset_t saved_compr_level_set;
    int compr_level, mem_level, strategy, filter;
    int counter;
    int line_reused;

    OPNG_ENSURE(process.num_iterations > 0, "Iterations not initialized");
    if ((process.num_iterations == 1) &&
        (process.status & OUTPUT_NEEDS_NEW_IDAT))
    {
       /* We already know this combination will be selected.
        * Do not waste time running it twice.
        */
       process.best_idat_size = 0;
       process.best_compr_level = bitset_find_first(process.compr_level_set);
       process.best_mem_level   = bitset_find_first(process.mem_level_set);
       process.best_strategy    = bitset_find_first(process.strategy_set);
       process.best_filter      = bitset_find_first(process.filter_set);
       return;
    }

    /* Prepare for the big iteration. */
    compr_level_set = process.compr_level_set;
    mem_level_set   = process.mem_level_set;
    strategy_set    = process.strategy_set;
    filter_set      = process.filter_set;
    process.best_idat_size   = idat_size_max + 1;
    process.best_compr_level = -1;
    process.best_mem_level   = -1;
    process.best_strategy    = -1;
    process.best_filter      = -1;

    /* Iterate through the "hyper-rectangle" (zc, zm, zs, f). */
    usr_printf("\nTrying:\n");
    line_reused = 0;
    counter = 0;
    for (filter = OPNG_FILTER_MIN;
         filter <= OPNG_FILTER_MAX; ++filter)
    {
       if (bitset_test(filter_set, filter))
       {
          for (strategy = OPNG_STRATEGY_MIN;
               strategy <= OPNG_STRATEGY_MAX; ++strategy)
          {
             if (bitset_test(strategy_set, strategy))
             {
                /* The compression level has no significance under
                 * Z_HUFFMAN_ONLY or Z_RLE.
                 */
                saved_compr_level_set = compr_level_set;
                if (strategy == Z_HUFFMAN_ONLY)
                {
                   compr_level_set = BITSET_EMPTY;
                   bitset_set(&compr_level_set, 1);  /* use deflate_fast */
                }
                else if (strategy == Z_RLE)
                {
                   compr_level_set = BITSET_EMPTY;
                   bitset_set(&compr_level_set, 9);  /* use deflate_slow */
                }
                for (compr_level = OPNG_COMPR_LEVEL_MAX;
                     compr_level >= OPNG_COMPR_LEVEL_MIN; --compr_level)
                {
                   if (bitset_test(compr_level_set, compr_level))
                   {
                      for (mem_level = OPNG_MEM_LEVEL_MAX;
                           mem_level >= OPNG_MEM_LEVEL_MIN; --mem_level)
                      {
                         if (bitset_test(mem_level_set, mem_level))
                         {
                            usr_printf(
                               "  zc = %d  zm = %d  zs = %d  f = %d",
                               compr_level, mem_level, strategy, filter);
                            usr_progress(counter, process.num_iterations);
                            ++counter;
                            opng_write_file(NULL,
                               compr_level, mem_level, strategy, filter);
                            if (process.out_idat_size > idat_size_max)
                            {
                               if (options.verbose)
                               {
                                  usr_printf("\t\tIDAT too big\n");
                                  line_reused = 0;
                               }
                               else
                               {
                                  usr_print_cntrl('\r');  /* CR: reset line */
                                  line_reused = 1;
                               }
                               continue;
                            }
                            usr_printf("\t\tIDAT size = %" OSYS_FSIZE_PRIu
                                       "\n",
                                       process.out_idat_size);
                            line_reused = 0;
                            if (process.best_idat_size < process.out_idat_size)
                               continue;
                            if (process.best_idat_size == process.out_idat_size
                                && process.best_strategy >= Z_HUFFMAN_ONLY)
                               continue;  /* it's neither smaller nor faster */
                            process.best_compr_level = compr_level;
                            process.best_mem_level   = mem_level;
                            process.best_strategy    = strategy;
                            process.best_filter      = filter;
                            process.best_idat_size   = process.out_idat_size;
                            if (!options.full)
                               process.max_idat_size = process.out_idat_size;
                         }
                      }
                   }
                }
                compr_level_set = saved_compr_level_set;
             }
          }
       }
    }
    if (line_reused)
        usr_print_cntrl(-31);  /* Minus N: erase N chars from start of line */

    OPNG_ENSURE(counter == process.num_iterations,
                "Inconsistent iteration counter");
    usr_progress(counter, process.num_iterations);
}

/*
 * Iteration finalization
 */
static void
opng_finish_iterations(void)
{
    if (process.best_idat_size + process.out_plte_trns_size <
        process.in_idat_size + process.in_plte_trns_size)
        process.status |= OUTPUT_NEEDS_NEW_IDAT;
    if (process.status & OUTPUT_NEEDS_NEW_IDAT)
    {
        if (process.best_idat_size <= idat_size_max)
        {
            usr_printf("\nSelecting parameters:\n");
            usr_printf("  zc = %d  zm = %d  zs = %d  f = %d",
                       process.best_compr_level, process.best_mem_level,
                       process.best_strategy, process.best_filter);
            if (process.best_idat_size > 0)
            {
                /* At least one trial has been run. */
                usr_printf("\t\tIDAT size = %" OSYS_FSIZE_PRIu,
                           process.best_idat_size);
            }
            usr_printf("\n");
        }
        else
        {
            /* The compressed image data is larger than the maximum allowed. */
            usr_printf("  zc = *  zm = *  zs = *  f = *\t\tIDAT size > %s\n",
                       idat_size_max_string);
        }
    }
}

/*
 * Image file optimization
 */
static void
opng_optimize_impl(const char *infile_name)
{
    static FILE *infile, *outfile;         /* static or volatile is required */
    static const char *infile_name_local;                      /* by cexcept */
    static const char *outfile_name, *bakfile_name;
    static int new_outfile, has_backup;
    char name_buf[FILENAME_MAX], tmp_buf[FILENAME_MAX];
    const char * volatile err_msg;

    memset(&process, 0, sizeof(process));
    if (options.force)
        process.status |= OUTPUT_NEEDS_NEW_IDAT;

    err_msg = NULL;  /* prepare for error handling */

    infile_name_local = infile_name;
    if ((infile = fopen(infile_name_local, "rb")) == NULL)
        Throw "Can't open the input file";
    Try
    {
        opng_read_file(infile);
    }
    Catch (err_msg)
    {
        OPNG_ENSURE(err_msg != NULL, "Mysterious error in opng_read_file");
    }
    fclose(infile);  /* finally */
    if (err_msg != NULL)
        Throw err_msg;  /* rethrow */

    /* Check the error flag. This must be the first check. */
    if (process.status & INPUT_HAS_ERRORS)
    {
        usr_printf("Recoverable errors found in input.");
        if (options.fix)
        {
            usr_printf(" Fixing...\n");
            process.status |= OUTPUT_NEEDS_NEW_FILE;
        }
        else
        {
            usr_printf(" Rerun " PROGRAM_NAME " with -fix enabled.\n");
            Throw "Previous error(s) not fixed";
        }
    }

    /* Check the junk flag. */
    if (process.status & INPUT_HAS_JUNK)
        process.status |= OUTPUT_NEEDS_NEW_FILE;

    /* Check the PNG signature and datastream flags. */
    if (!(process.status & INPUT_HAS_PNG_SIGNATURE))
        process.status |= OUTPUT_NEEDS_NEW_FILE;
    if (process.status & INPUT_HAS_PNG_DATASTREAM)
    {
        if (options.nz && (process.status & OUTPUT_NEEDS_NEW_IDAT))
        {
            usr_printf(
                "IDAT recoding is necessary, but is disabled by the user.\n");
            Throw "Can't continue";
        }
    }
    else
        process.status |= OUTPUT_NEEDS_NEW_IDAT;

    /* Check the digital signature flag. */
    if (process.status & INPUT_HAS_DIGITAL_SIGNATURE)
    {
        usr_printf("Digital signature found in input.");
        if (options.force)
        {
            usr_printf(" Erasing...\n");
            process.status |= OUTPUT_NEEDS_NEW_FILE;
        }
        else
        {
            usr_printf(" Rerun " PROGRAM_NAME " with -force enabled.\n");
            Throw "Can't optimize digitally-signed files";
        }
    }

    /* Check the multi-image flag. */
    if (process.status & INPUT_HAS_MULTIPLE_IMAGES)
    {
        if (!options.snip && !(process.status & INPUT_IS_PNG_FILE))
        {
            usr_printf("Conversion to PNG requires snipping. "
                       "Rerun " PROGRAM_NAME " with -snip enabled.\n");
            Throw "Incompatible input format";
        }
    }
    if ((process.status & INPUT_HAS_APNG) && options.snip)
        process.status |= OUTPUT_NEEDS_NEW_FILE;

    /* Check the stripped-data flag. */
    if (process.status & INPUT_HAS_STRIPPED_DATA)
        usr_printf("Stripping metadata...\n");

    /* Initialize the output file name. */
    outfile_name = NULL;
    if (!(process.status & INPUT_IS_PNG_FILE))
    {
        if (osys_path_chext(name_buf, sizeof(name_buf),
                            infile_name_local, ".png") == NULL)
            Throw "Can't create the output file (name too long)";
        outfile_name = name_buf;
    }
    if (options.out_name != NULL)
        outfile_name = options.out_name;  /* override the old name */
    if (options.dir_name != NULL)
    {
        const char *tmp_name;
        if (outfile_name != NULL)
        {
            strcpy(tmp_buf, outfile_name);
            tmp_name = tmp_buf;
        }
        else
            tmp_name = infile_name_local;
        if (osys_path_chdir(name_buf, sizeof(name_buf), tmp_name,
                            options.dir_name) == NULL)
            Throw "Can't create the output file (name too long)";
        outfile_name = name_buf;
    }
    if (outfile_name == NULL)
    {
        outfile_name = infile_name_local;
        new_outfile = 0;
    }
    else
    {
        int test_eq = osys_test_eq(infile_name_local, outfile_name);
        if (test_eq >= 0)
            new_outfile = (test_eq == 0);
        else
        {
            /* We don't know if the two paths point to the same file.
             * Use a crude path name comparison.
             */
            new_outfile = (strcmp(infile_name_local, outfile_name) != 0);
        }
    }

    /* Initialize the backup file name. */
    bakfile_name = tmp_buf;
    if (new_outfile)
    {
        if (osys_path_mkbak(tmp_buf, sizeof(tmp_buf),
            outfile_name) == NULL)
            bakfile_name = NULL;
    }
    else
    {
        if (osys_path_mkbak(tmp_buf, sizeof(tmp_buf),
            infile_name_local) == NULL)
            bakfile_name = NULL;
    }
    /* Check the name even in simulation mode, to ensure a uniform behavior. */
    if (bakfile_name == NULL)
        Throw "Can't create backup file (name too long)";
    /* Check the backup file before engaging into lengthy trials. */
    if (!options.simulate && osys_test(outfile_name, "e") == 0)
    {
        if (new_outfile && !options.backup && !options.clobber)
        {
            usr_printf("The output file exists. "
                       "Rerun " PROGRAM_NAME " with -backup enabled\n");
            Throw "Can't overwrite the output file";
        }
        if (osys_test(outfile_name, "fw") != 0 ||
            (!options.clobber && osys_test(bakfile_name, "e") == 0))
            Throw "Can't back up the existing output file";
    }

    /* Display the input IDAT/file sizes. */
    if (process.status & INPUT_HAS_PNG_DATASTREAM)
        usr_printf("Input IDAT size = %" OSYS_FSIZE_PRIu " bytes\n",
                   process.in_idat_size);
    usr_printf("Input file size = %" OSYS_FSIZE_PRIu " bytes\n",
               process.in_file_size);

    /* Find the best parameters and see if it's worth recompressing. */
    if (!options.nz || (process.status & OUTPUT_NEEDS_NEW_IDAT))
    {
        opng_init_iterations();
        opng_iterate();
        opng_finish_iterations();
    }
    if (process.status & OUTPUT_NEEDS_NEW_IDAT)
    {
        process.status |= OUTPUT_NEEDS_NEW_FILE;
        opng_check_idat_size(process.best_idat_size);
    }

    /* Stop here? */
    if (!(process.status & OUTPUT_NEEDS_NEW_FILE))
    {
        usr_printf("\n%s is already optimized.\n", infile_name_local);
        if (!new_outfile)
            return;
    }
    if (options.simulate)
    {
        usr_printf("\nNo output: simulation mode.\n");
        return;
    }

    /* Make room for the output file. */
    if (new_outfile)
    {
        usr_printf("\nOutput file: %s\n", outfile_name);
        if (options.dir_name != NULL)
            osys_create_dir(options.dir_name);
        has_backup = 0;
        if (osys_test(outfile_name, "e") == 0)
        {
            if (osys_rename(outfile_name, bakfile_name, options.clobber) != 0)
                Throw "Can't back up the output file";
            has_backup = 1;
        }
    }
    else
    {
        if (osys_rename(infile_name_local, bakfile_name, options.clobber) != 0)
            Throw "Can't back up the input file";
        has_backup = 1;
    }

    outfile = fopen(outfile_name, "wb");
    Try
    {
        if (outfile == NULL)
            Throw "Can't open the output file";
        if (process.status & OUTPUT_NEEDS_NEW_IDAT)
        {
            /* Write a brand new PNG datastream to the output. */
            opng_write_file(outfile,
                process.best_compr_level, process.best_mem_level,
                process.best_strategy, process.best_filter);
        }
        else
        {
            /* Copy the input PNG datastream to the output. */
            infile =
                fopen((new_outfile ? infile_name_local : bakfile_name), "rb");
            if (infile == NULL)
                Throw "Can't reopen the input file";
            Try
            {
                if (process.in_datastream_offset > 0 &&
                    osys_fseeko(infile, process.in_datastream_offset,
                                 SEEK_SET) != 0)
                    Throw "Can't reposition the input file";
                process.best_idat_size = process.in_idat_size;
                opng_copy_file(infile, outfile);
            }
            Catch (err_msg)
            {
                OPNG_ENSURE(err_msg != NULL,
                            "Mysterious error in opng_copy_file");
            }
            fclose(infile);  /* finally */
            if (err_msg != NULL)
                Throw err_msg;  /* rethrow */
        }
    }
    Catch (err_msg)
    {
        if (outfile != NULL)
            fclose(outfile);
        /* Restore the original input file and rethrow the exception. */
        if (has_backup)
        {
            if (osys_rename(bakfile_name,
                            (new_outfile ? outfile_name : infile_name_local),
                            1) != 0)
                opng_print_warning(
                    "Can't recover the original file from backup");
        }
        else
        {
            OPNG_ENSURE(new_outfile,
                        "Overwrote input with no temporary backup");
            if (osys_unlink(outfile_name) != 0)
                opng_print_warning("Can't remove the broken output file");
        }
        Throw err_msg;  /* rethrow */
    }
    /* assert(err_msg == NULL); */
    fclose(outfile);

    /* Preserve file attributes (e.g. ownership, access rights, time stamps)
     * on request, if possible.
     */
    if (options.preserve)
        osys_copy_attr((new_outfile ? infile_name_local : bakfile_name),
                       outfile_name);

    /* Remove the backup file if it is not needed. */
    if (!new_outfile && !options.backup)
    {
        if (osys_unlink(bakfile_name) != 0)
            opng_print_warning("Can't remove the backup file");
    }

    /* Display the output IDAT/file sizes. */
    usr_printf("\nOutput IDAT size = %" OSYS_FSIZE_PRIu " bytes",
               process.out_idat_size);
    if (process.status & INPUT_HAS_PNG_DATASTREAM)
    {
        usr_printf(" (");
        opng_print_fsize_difference(process.in_idat_size,
                                    process.out_idat_size, 0);
        usr_printf(")");
    }
    usr_printf("\nOutput file size = %" OSYS_FSIZE_PRIu " bytes (",
               process.out_file_size);
    opng_print_fsize_difference(process.in_file_size,
                                process.out_file_size, 1);
    usr_printf(")\n");
}

/*
 * Engine initialization
 */
int
opng_initialize(const struct opng_options *init_options,
                const struct opng_ui *init_ui)
{
    /* Initialize and check the validity of the user interface. */
    usr_printf      = init_ui->printf_fn;
    usr_print_cntrl = init_ui->print_cntrl_fn;
    usr_progress    = init_ui->progress_fn;
    usr_panic       = init_ui->panic_fn;
    if (usr_printf == NULL      ||
        usr_print_cntrl == NULL ||
        usr_progress == NULL    ||
        usr_panic == NULL)
       return -1;

    /* Initialize and adjust the user options. */
    options = *init_options;
    if (options.optim_level == 0)
    {
        options.nb = options.nc = options.np = 1;
        options.nz = 1;
    }

    /* Start the engine. */
    memset(&summary, 0, sizeof(summary));
    engine.started = 1;
    return 0;
}

/*
 * Engine execution
 */
int
opng_optimize(const char *infile_name)
{
    const char *err_msg;
    volatile int result;  /* volatile not needed, but keeps compilers happy */

    OPNG_ENSURE(engine.started, "The OptiPNG engine is not running");

    usr_printf("** Processing: %s\n", infile_name);
    ++summary.file_count;
    opng_clear_image_info();
    Try
    {
        opng_optimize_impl(infile_name);
        if (process.status & INPUT_HAS_ERRORS)
        {
            ++summary.err_count;
            ++summary.fix_count;
        }
        if (process.status & INPUT_HAS_MULTIPLE_IMAGES)
        {
            if (options.snip)
                ++summary.snip_count;
        }
        result = 0;
    }
    Catch (err_msg)
    {
        ++summary.err_count;
        opng_print_error(err_msg);
        result = -1;
    }
    opng_destroy_image_info();
    usr_printf("\n");
    return result;
}

/*
 * Engine finalization
 */
int
opng_finalize(void)
{
    /* Print the status report. */
    if (options.verbose || summary.snip_count > 0 || summary.err_count > 0)
    {
        usr_printf("** Status report\n");
        usr_printf("%u file(s) have been processed.\n", summary.file_count);
        if (summary.snip_count > 0)
        {
            usr_printf("%u multi-image file(s) have been snipped.\n",
                       summary.snip_count);
        }
        if (summary.err_count > 0)
        {
            usr_printf("%u error(s) have been encountered.\n",
                       summary.err_count);
            if (summary.fix_count > 0)
                usr_printf("%u erroneous file(s) have been fixed.\n",
                           summary.fix_count);
        }
    }

    /* Stop the engine. */
    engine.started = 0;
    return 0;
}
