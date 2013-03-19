/*
 * pngxio.c - libpng extension: I/O state query.
 *
 * Copyright (C) 2003-2011 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 *
 * NOTE:
 * The functionality provided in this module has "graduated", and is now
 * part of libpng.  The original code, retrofitted as a back-port, has
 * limitations: it is thread-unsafe, and it only allows one png_ptr object
 * for reading and one for writing.
 *
 * CAUTION:
 * libpng-1.4.5 is the earliest version whose I/O state implementation
 * can be used reliably.
 */

#include "pngxutil.h"

#define PNGX_INTERNAL
#include "pngxpriv.h"


#if PNG_LIBPNG_VER < 10405


static png_structp _pngxio_read_ptr;
static png_structp _pngxio_write_ptr;

static png_rw_ptr _pngxio_read_fn;
static png_rw_ptr _pngxio_write_fn;

static int _pngxio_read_io_state;
static int _pngxio_write_io_state;

static png_byte _pngxio_read_crt_chunk_hdr[9];
static png_byte _pngxio_write_crt_chunk_hdr[9];

static unsigned int _pngxio_read_crt_chunk_hdr_len;
static unsigned int _pngxio_write_crt_chunk_hdr_len;

static png_uint_32 _pngxio_read_crt_len;
static png_uint_32 _pngxio_write_crt_len;

static const char *_pngxio_errmsg_invalid_argument =
   "[PNGXIO internal] invalid argument";


/* Update io_state and call the user-supplied read/write functions. */
void /* PRIVATE */
pngxio_read_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_rw_ptr io_data_fn;
   int *io_state_ptr;
   int io_state_op;
   png_byte *crt_chunk_hdr;
   unsigned int *crt_chunk_hdr_len_ptr;
   png_uint_32 *crt_len_ptr;

   if (png_ptr == _pngxio_read_ptr)
   {
      io_data_fn = _pngxio_read_fn;
      io_state_ptr = &_pngxio_read_io_state;
      io_state_op = PNGX_IO_READING;
      crt_chunk_hdr = _pngxio_read_crt_chunk_hdr;
      crt_chunk_hdr_len_ptr = &_pngxio_read_crt_chunk_hdr_len;
      crt_len_ptr = &_pngxio_read_crt_len;
   }
   else if (png_ptr == _pngxio_write_ptr)
   {
      io_data_fn = _pngxio_write_fn;
      io_state_ptr = &_pngxio_write_io_state;
      io_state_op = PNGX_IO_WRITING;
      crt_chunk_hdr = _pngxio_write_crt_chunk_hdr;
      crt_chunk_hdr_len_ptr = &_pngxio_write_crt_chunk_hdr_len;
      crt_len_ptr = &_pngxio_write_crt_len;
   }
   else
   {
      png_error(png_ptr, _pngxio_errmsg_invalid_argument);
      /* NOTREACHED */
      return;
   }

   switch (*io_state_ptr & PNGX_IO_MASK_LOC)
   {
   case PNGX_IO_SIGNATURE:
      /* libpng must serialize the signature in a single I/O session. */
      PNGX_ASSERT(length <= 8);
      io_data_fn(png_ptr, data, length);
      *io_state_ptr = io_state_op | PNGX_IO_CHUNK_HDR;
      *crt_chunk_hdr_len_ptr = 0;
      return;
   case PNGX_IO_CHUNK_HDR:
      /* libpng must serialize the chunk header in a single I/O session.
       * (This was done in libpng-1.2.30, but regressed in libpng-1.4.0,
       * so we cannot rely on it here.)
       */
      PNGX_ASSERT(length == 4 || length == 8);
      PNGX_ASSERT(length + *crt_chunk_hdr_len_ptr <= 8);
      if (io_state_op == PNGX_IO_READING)
      {
         if (*crt_chunk_hdr_len_ptr == 0)
            io_data_fn(png_ptr, crt_chunk_hdr, 8);
         memcpy(data, crt_chunk_hdr + *crt_chunk_hdr_len_ptr, length);
         *crt_chunk_hdr_len_ptr += length;
         if (*crt_chunk_hdr_len_ptr < 8)
            return;
         *crt_len_ptr = png_get_uint_32(crt_chunk_hdr);
         /* memcpy(png_ptr->chunk_name, crt_chunk_hdr + 4, 4); */
      }
      else  /* io_state_op == PNGX_IO_WRITING */
      {
         memcpy(crt_chunk_hdr + *crt_chunk_hdr_len_ptr, data, length);
         *crt_chunk_hdr_len_ptr += length;
         if (*crt_chunk_hdr_len_ptr < 8)
            return;
         *crt_len_ptr = png_get_uint_32(crt_chunk_hdr);
         /* memcpy(png_ptr->chunk_name, crt_chunk_hdr + 4, 4); */
         io_data_fn(png_ptr, crt_chunk_hdr, 8);
      }
      *crt_chunk_hdr_len_ptr = 0;
      *io_state_ptr = io_state_op | PNGX_IO_CHUNK_DATA;
      return;
   case PNGX_IO_CHUNK_DATA:
      /* libpng may serialize the chunk data in multiple I/O sessions. */
      if (length == 0)
         return;
      if (*crt_len_ptr > 0)
      {
         PNGX_ASSERT(length <= *crt_len_ptr);
         io_data_fn(png_ptr, data, length);
         *crt_len_ptr -= length;
         return;
      }
      *io_state_ptr = io_state_op | PNGX_IO_CHUNK_CRC;
      /* FALLTHROUGH */
   case PNGX_IO_CHUNK_CRC:
      /* libpng must serialize the chunk CRC in a single I/O session. */
      PNGX_ASSERT(length == 4);
      io_data_fn(png_ptr, data, 4);
      *io_state_ptr = io_state_op | PNGX_IO_CHUNK_HDR;
      return;
   }
}

/* Get png_ptr->io_state. */
png_uint_32 PNGAPI
pngx_get_io_state(png_structp png_ptr)
{
   png_uint_32 io_state;

   if (png_ptr == _pngxio_read_ptr)
      io_state = _pngxio_read_io_state;
   else if (png_ptr == _pngxio_write_ptr)
      io_state = _pngxio_write_io_state;
   else
   {
      png_error(png_ptr, _pngxio_errmsg_invalid_argument);
      /* NOTREACHED */
      io_state = PNGX_IO_NONE;
   }

   return io_state;
}

/* Get png_ptr->chunk_name. */
png_bytep PNGAPI
pngx_get_io_chunk_name(png_structp png_ptr)
{
   png_bytep chunk_name;

   if (png_ptr == _pngxio_read_ptr)
      chunk_name = _pngxio_read_crt_chunk_hdr + 4;
   else if (png_ptr == _pngxio_write_ptr)
      chunk_name = _pngxio_write_crt_chunk_hdr + 4;
   else
   {
      png_error(png_ptr, _pngxio_errmsg_invalid_argument);
      /* NOTREACHED */
      chunk_name = NULL;
   }

   return chunk_name;
}

/* Wrap png_set_read_fn. */
void PNGAPI
pngx_set_read_fn(png_structp png_ptr, png_voidp io_ptr,
   png_rw_ptr read_data_fn)
{
   _pngxio_read_ptr = png_ptr;
   _pngxio_write_ptr = NULL;
   _pngxio_read_fn = read_data_fn;
   png_set_read_fn(png_ptr, io_ptr, pngxio_read_write);
   _pngxio_read_io_state = PNGX_IO_READING | PNGX_IO_SIGNATURE;
}

/* Wrap png_set_write_fn. */
void PNGAPI
pngx_set_write_fn(png_structp png_ptr, png_voidp io_ptr,
   png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn)
{
   _pngxio_write_ptr = png_ptr;
   _pngxio_read_ptr = NULL;
   _pngxio_write_fn = write_data_fn;
   png_set_write_fn(png_ptr, io_ptr, pngxio_read_write, output_flush_fn);
   _pngxio_write_io_state = PNGX_IO_WRITING | PNGX_IO_SIGNATURE;
}

/* Wrap png_write_sig. */
void PNGAPI
pngx_write_sig(png_structp png_ptr)
{
#if PNG_LIBPNG_VER >= 10400
   png_write_sig(png_ptr);
#else
   /* png_write_sig is not exported from the earlier libpng versions. */
   static png_byte png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   pngxio_read_write(png_ptr, png_signature, 8);
   /* TODO: Take png_ptr->sig_bytes into account. */
#endif
}


#endif /* PNG_LIBPNG_VER < 10405 */
