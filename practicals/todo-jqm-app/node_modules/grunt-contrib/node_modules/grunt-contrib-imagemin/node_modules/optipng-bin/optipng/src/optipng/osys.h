/*
 * osys.h
 * System extensions.
 *
 * Copyright (C) 2003-2012 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the attached LICENSE for more information.
 */

#ifndef OSYS_H
#define OSYS_H

#include <stdio.h>


/*
 * File offset and size types:
 *
 * osys_foffset_t: the file offset type (a signed integer type)
 * OSYS_FOFFSET_MIN: the minimum value (less than 0)
 * OSYS_FOFFSET_MAX: the maximum value (greater than 0)
 * OSYS_FOFFSET_SCNd: macro for scanf format specifier ("%d"-like)
 * OSYS_FOFFSET_SCNx: macro for scanf format specifier ("%x"-like)
 * OSYS_FOFFSET_PRId: macro for printf format specifier ("%d"-like)
 * OSYS_FOFFSET_PRIx: macro for printf format specifier ("%x"-like)
 * OSYS_FOFFSET_PRIX: macro for printf format specifier ("%X"-like)
 *
 * osys_fsize_t: the file size type (an unsigned integer type)
 * OSYS_FSIZE_MAX: the maximum value
 * OSYS_FSIZE_SCNu: macro for scanf format specifier ("%u"-like)
 * OSYS_FSIZE_SCNx: macro for scanf format specifier ("%x"-like)
 * OSYS_FSIZE_PRIu: macro for printf format specifier ("%u"-like)
 * OSYS_FSIZE_PRIx: macro for printf format specifier ("%x"-like)
 * OSYS_FSIZE_PRIX: macro for printf format specifier ("%X"-like)
 */
#include <limits.h>
#if (LONG_MAX > 0x7fffffffL) || (LONG_MAX > INT_MAX)

typedef long osys_foffset_t;
#define OSYS_FOFFSET_MIN LONG_MIN
#define OSYS_FOFFSET_MAX LONG_MAX
#define OSYS_FOFFSET_SCNd "ld"
#define OSYS_FOFFSET_SCNx "lx"
#define OSYS_FOFFSET_PRId "ld"
#define OSYS_FOFFSET_PRIx "lx"
#define OSYS_FOFFSET_PRIX "lX"

typedef unsigned long osys_fsize_t;
#define OSYS_FSIZE_MAX ULONG_MAX
#define OSYS_FSIZE_SCNu "lu"
#define OSYS_FSIZE_SCNx "lx"
#define OSYS_FSIZE_PRIu "lu"
#define OSYS_FSIZE_PRIx "lx"
#define OSYS_FSIZE_PRIX "lX"

#elif defined _I64_MAX && (defined _WIN32 || defined __WIN32__)

typedef __int64 osys_foffset_t;
#define OSYS_FOFFSET_MIN _I64_MIN
#define OSYS_FOFFSET_MAX _I64_MAX
#define OSYS_FOFFSET_SCNd "I64d"
#define OSYS_FOFFSET_SCNx "I64x"
#define OSYS_FOFFSET_PRId "I64d"
#define OSYS_FOFFSET_PRIx "I64x"
#define OSYS_FOFFSET_PRIX "I64X"

typedef unsigned __int64 osys_fsize_t;
#define OSYS_FSIZE_MAX _UI64_MAX
#define OSYS_FSIZE_SCNu "I64u"
#define OSYS_FSIZE_SCNx "I64x"
#define OSYS_FSIZE_PRIu "I64u"
#define OSYS_FSIZE_PRIx "I64x"
#define OSYS_FSIZE_PRIX "I64X"

#else
#include <stdint.h>
#include <inttypes.h>

typedef int_least64_t osys_foffset_t;
#define OSYS_FOFFSET_MIN INT_LEAST64_MIN
#define OSYS_FOFFSET_MAX INT_LEAST64_MAX
#define OSYS_FOFFSET_SCNd SCNdLEAST64
#define OSYS_FOFFSET_SCNx SCNxLEAST64
#define OSYS_FOFFSET_PRId PRIdLEAST64
#define OSYS_FOFFSET_PRIx PRIxLEAST64
#define OSYS_FOFFSET_PRIX PRIXLEAST64

typedef uint_least64_t osys_fsize_t;
#define OSYS_FSIZE_MAX UINT_LEAST64_MAX
#define OSYS_FSIZE_SCNu SCNuLEAST64
#define OSYS_FSIZE_SCNx SCNxLEAST64
#define OSYS_FSIZE_PRIu PRIuLEAST64
#define OSYS_FSIZE_PRIx PRIxLEAST64
#define OSYS_FSIZE_PRIX PRIXLEAST64

#endif


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Creates a new path name by changing the directory component of
 * a specified path name.
 * The new directory name can be the empty string, indicating that
 * the resulting path has no directory. Otherwise, the directory
 * name should follow the conventions specific to the host operating
 * system, and may optionally be ended with the directory separator
 * (e.g. "/" on Unix).
 * On success, the function returns buffer.
 * On error, it returns NULL.
 */
char *
osys_path_chdir(char *buffer, size_t bufsize,
                const char *old_path, const char *new_dirname);

/*
 * Creates a new path name by changing the extension component of
 * a specified path name.
 * The new extension name can be the empty string, indicating that
 * the resulting path has no extension. Otherwise, the extension
 * name must begin with the extension separator (e.g. "." on Unix).
 * On success, the function returns buffer.
 * On error, it returns NULL.
 */
char *
osys_path_chext(char *buffer, size_t bufsize,
                const char *old_path, const char *new_extname);

/*
 * Creates a backup path name.
 * On success, the function returns buffer.
 * On error, it returns NULL.
 */
char *
osys_path_mkbak(char *buffer, size_t bufsize, const char *path);

/*
 * Returns the current value of the file position indicator.
 * On error, the function returns (osys_foffset_t)(-1).
 */
osys_foffset_t
osys_ftello(FILE *stream);

/*
 * Sets the file position indicator at the specified file offset.
 * On success, the function returns 0. On error, it returns -1.
 */
int
osys_fseeko(FILE *stream, osys_foffset_t offset, int whence);

/*
 * Gets the size of the specified file stream.
 * This function may change the file position indicator.
 * On success, the function returns 0. On error, it returns -1.
 */
int
osys_fgetsize(FILE *stream, osys_fsize_t *size);

/*
 * Reads a block of data from the specified file offset.
 * The file-position indicator is saved and restored after reading.
 * The file buffer is flushed before and after reading.
 * On success, the function returns the number of bytes read.
 * On error, it returns 0.
 */
size_t
osys_fread_at(FILE *stream, osys_foffset_t offset, int whence,
              void *block, size_t blocksize);

/*
 * Writes a block of data at the specified file offset.
 * The file-position indicator is saved and restored after writing.
 * The file buffer is flushed before and after writing.
 * On success, the function returns the number of bytes written.
 * On error, it returns 0.
 */
size_t
osys_fwrite_at(FILE *stream, osys_foffset_t offset, int whence,
               const void *block, size_t blocksize);

/*
 * Changes the name of a file path.
 * On success, the function returns 0.
 * On error, it returns -1.
 */
int
osys_rename(const char *src_path, const char *dest_path, int clobber);

/*
 * Copies the attributes (access mode, time stamp, etc.) of the source
 * file path onto the destination file path.
 * On success, the function returns 0.
 * On error, it returns -1.
 */
int
osys_copy_attr(const char *src_path, const char *dest_path);

/*
 * Creates a new directory.
 * If the directory is successfully created, or if it already exists,
 * the function returns 0.
 * Otherwise, it returns -1.
 */
int
osys_create_dir(const char *dirname);

/*
 * Determines if the accessibility of the specified path satisfies
 * the specified access mode. The access mode consists of one or more
 * characters that indicate the checks to be performed, as follows:
 *   'e': the path exists; it needs not be a regular file.
 *   'f': the path exists and is a regular file.
 *   'r': the path exists and read permission is granted.
 *   'w': the path exists and write permission is granted.
 *   'x': the path exists and execute permission is granted.
 * For example, to determine if a file can be opened for reading using
 * fopen(), use "fr" in the access mode.
 * If all checks succeed, the function returns 0.
 * Otherwise, it returns -1.
 */
int
osys_test(const char *path, const char *mode);

/*
 * Determines if two accessible paths are equivalent, i.e. they
 * point to the same physical location.
 * If the two paths are equivalent, the function returns 1.
 * If the two paths are not equivalent, the function returns 0.
 * If at least one path is not accessible or does not exist, or
 * if the check cannot be performed, the function returns -1.
 */
int
osys_test_eq(const char *path1, const char *path2);

/*
 * Removes a directory entry.
 * On success, the function returns 0.
 * On error, it returns -1.
 */
int
osys_unlink(const char *path);

/*
 * Prints an error message to stderr and terminates the program
 * execution immediately, exiting with code 70 (EX_SOFTWARE).
 * This function does not raise SIGABRT, and it does not generate
 * other files (like core dumps, where applicable).
 */
void
osys_terminate(void);


#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif  /* OSYS_H */
