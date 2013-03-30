/**
 * tiffdef.h
 * Macro definitions for minitiff (private).
 *
 * Copyright (C) 2006 Cosmin Truta.
 *
 * minitiff is open-source software, distributed under the zlib license.
 * For conditions of distribution and use, see copyright notice in minitiff.h.
 **/

#ifndef MINITIFF_DEF_H
#define MINITIFF_DEF_H

#define TIFF_TAG_SUBFILE_TYPE           0x00fe
#define TIFF_TAG_WIDTH                  0x0100
#define TIFF_TAG_HEIGHT                 0x0101
#define TIFF_TAG_BITS_PER_SAMPLE        0x0102
#define TIFF_TAG_COMPRESSION            0x0103
#define TIFF_TAG_PHOTOMETRIC            0x0106
#define TIFF_TAG_DOCUMENT_NAME          0x010d
#define TIFF_TAG_IMAGE_DESCRIPTION      0x010e
#define TIFF_TAG_MAKE                   0x010f
#define TIFF_TAG_MODEL                  0x0110
#define TIFF_TAG_STRIP_OFFSETS          0x0111
#define TIFF_TAG_ORIENTATION            0x0112
#define TIFF_TAG_SAMPLES_PER_PIXEL      0x0115
#define TIFF_TAG_ROWS_PER_STRIP         0x0116
#define TIFF_TAG_STRIP_BYTE_COUNTS      0x0117
#define TIFF_TAG_MIN_SAMPLE_VALUE       0x0118
#define TIFF_TAG_MAX_SAMPLE_VALUE       0x0119
#define TIFF_TAG_X_RESOLUTION           0x011a
#define TIFF_TAG_Y_RESOLUTION           0x011b
#define TIFF_TAG_PLANAR_CONFIGURATION   0x011c
#define TIFF_TAG_PAGE_NAME              0x011d
#define TIFF_TAG_X_POSITION             0x011e
#define TIFF_TAG_Y_POSITION             0x011f
#define TIFF_TAG_RESOLUTION_UNIT        0x0128
#define TIFF_TAG_PAGE_NUMBER            0x0129
#define TIFF_TAG_SOFTWARE               0x0131
#define TIFF_TAG_DATE_TIME              0x0132
#define TIFF_TAG_ARTIST                 0x013b
#define TIFF_TAG_HOST_COMPUTER          0x013c
#define TIFF_TAG_PREDICTOR              0x013d

/* TIFF extensions */
#define TIFF_TAGEXT_XMP                     0x02bc
#define TIFF_TAGEXT_IPTC                    0x83bb
#define TIFF_TAGEXT_EXIF_IFD                0x8769
#define TIFF_TAGEXT_GPS_IFD                 0x8825
#define TIFF_TAGEXT_INTEROPERABILITY_IFD    0xa005
#define TIFF_TAGEXT_PRINT_IM                0xc4a5

#endif  /* MINITIFF_DEF_H */
