
/* pngusr.h - private libpng configuration settings for OptiPNG
 *
 * Copyright (C) 2003-2010 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as libpng.
 */

#ifndef PNGUSR_H
#define PNGUSR_H

/* Track memory allocations under MS Visual C++. */
#if defined(_DEBUG) && defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <malloc.h>
#include <stdlib.h>
#endif

/* Remove the libpng features that are not needed by OptiPNG. */
#define PNG_NO_ASSEMBLER_CODE
#define PNG_NO_ERROR_NUMBERS
#define PNG_NO_FLOATING_POINT_SUPPORTED
#define PNG_NO_LEGACY_SUPPORTED
#define PNG_NO_MNG_FEATURES
#define PNG_NO_PROGRESSIVE_READ
#define PNG_NO_READ_TRANSFORMS
#define PNG_NO_SETJMP_SUPPORTED
#define PNG_NO_TIME_RFC1123
#define PNG_NO_USER_MEM
#define PNG_NO_USER_TRANSFORM_PTR
#define PNG_NO_WRITE_TRANSFORMS
#define PNG_NO_ZALLOC_ZERO

#endif	/* PNGUSR_H */
