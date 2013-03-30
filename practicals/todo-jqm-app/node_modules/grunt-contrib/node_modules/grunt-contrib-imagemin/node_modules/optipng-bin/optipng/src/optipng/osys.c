/*
 * osys.c
 * System extensions.
 *
 * Copyright (C) 2003-2012 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the attached LICENSE for more information.
 */

#include "osys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Auto-configuration.
 */
#if defined UNIX || defined __UNIX__ || defined __unix || defined __unix__ || \
    defined _POSIX_SOURCE || defined _POSIX_C_SOURCE || defined _XOPEN_SOURCE
#  define OSYS_UNIX
/* To be continued. */
#endif

#if defined WIN32 || defined _WIN32 || defined _WIN32_WCE || \
    defined __WIN32__ || defined __NT__
#  define OSYS_WIN32
#endif

#if defined WIN64 || defined _WIN64 || defined __WIN64__
#  define OSYS_WIN64
#endif

#if defined OSYS_WIN32 || defined OSYS_WIN64
#  define OSYS_WINDOWS
#endif

#if defined DOS || defined _DOS || defined __DOS__ || \
    defined MSDOS || defined _MSDOS || defined __MSDOS__
#  define OSYS_DOS
#endif

#if defined OS2 || defined OS_2 || defined __OS2__
#  define OSYS_OS2
#endif

#if defined OSYS_DOS || defined OSYS_OS2
#  define OSYS_DOSISH
#endif

#if defined __APPLE__
#  define OSYS_MACOS
#  if defined __MACH__
#    define OSYS_MACOSX
#    ifndef OSYS_UNIX
#      define OSYS_UNIX
#    endif
#  endif
#endif

#if defined __CYGWIN__ || defined __DJGPP__ || defined __EMX__
#  define OSYS_UNIXISH
   /* Strictly speaking, this is not entirely correct, but "it works". */
#  ifndef OSYS_UNIX
#    define OSYS_UNIX
#  endif
#endif

#if defined OSYS_UNIX || (!defined OSYS_WINDOWS && !defined OSYS_DOSISH)
#  include <unistd.h>
#  if defined _POSIX_VERSION || defined _XOPEN_VERSION
#    ifndef OSYS_UNIX
#      define OSYS_UNIX
#    endif
#  endif
#endif

#if defined OSYS_UNIX
#  define _BSD_SOURCE 1
#  include <strings.h>
#endif

#if defined OSYS_UNIX || defined OSYS_WINDOWS || defined OSYS_DOSISH
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  ifdef _MSC_VER
#    include <sys/utime.h>
#  else
#    include <utime.h>
#  endif
#endif

#if defined OSYS_WINDOWS || defined OSYS_DOSISH || defined OSYS_UNIXISH
#  include <io.h>
#endif

#if defined OSYS_WINDOWS
#  include <windows.h>
#endif

#if defined OSYS_DOSISH
#  include <process.h>
#endif


/*
 * More auto-configuration.
 */
#if (defined OSYS_WINDOWS || defined OSYS_DOSISH) && !defined OSYS_UNIXISH
#  define OSYS_PATH_PATHSEP '\\'
#  define OSYS_PATH_PATHSEP_STR "\\"
#  define OSYS_PATH_PATHSEP_ALL_STR "/\\"
#else
#  define OSYS_PATH_PATHSEP '/'
#  define OSYS_PATH_PATHSEP_STR "/"
#  if defined OSYS_UNIXISH
#    define OSYS_PATH_PATHSEP_ALL_STR "/\\"
#  elif defined OSYS_MACOSX
#    define OSYS_PATH_PATHSEP_ALL_STR "/:"
#  else  /* OSYS_UNIX and others */
#    define OSYS_PATH_PATHSEP_ALL_STR "/"
#  endif
#endif
#define OSYS_PATH_EXTSEP '.'
#define OSYS_PATH_EXTSEP_STR "."
/* TODO: Support more systems (e.g. OSYS_RISCOS). */

#if defined OSYS_WINDOWS || defined OSYS_DOSISH || defined OSYS_UNIXISH
#  define OSYS_PATH_DOS
#endif

#ifdef R_OK
#  define OSYS_TEST_READ R_OK
#else
#  define OSYS_TEST_READ 4
#endif
#ifdef W_OK
#  define OSYS_TEST_WRITE W_OK
#else
#  define OSYS_TEST_WRITE 2
#endif
#ifdef X_OK
#  define OSYS_TEST_EXEC X_OK
#else
#  define OSYS_TEST_EXEC 1
#endif
#ifdef F_OK
#  define OSYS_TEST_FILE F_OK
#else
#  define OSYS_TEST_FILE 0
#endif


/*
 * Utility macros.
 */
#define OSYS_UNUSED(x) ((void)(x))

#ifdef OSYS_PATH_DOS
#  define OSYS_PATH_IS_DRIVE_LETTER(ch) \
          (((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= 'a' && (ch) <= 'z'))
#endif

#ifdef OSYS_WINDOWS
#  if defined OSYS_WIN64
#    define OSYS_HAVE_STDIO__I64
#    define OSYS_WINDOWS_IS_WIN9X() 0
#  else
#    if (defined _MSC_VER && _MSC_VER >= 1400) || \
        (defined __MSVCRT_VERSION__ && __MSVCRT_VERSION__ >= 0x800)
#      define OSYS_HAVE_STDIO__I64
#    endif
#    define OSYS_WINDOWS_IS_WIN9X() (GetVersion() >= 0x80000000U)
#  endif
#endif


/*
 * Creates a new path name by changing the directory component of
 * a specified path name.
 */
char *
osys_path_chdir(char *buffer, size_t bufsize,
                const char *old_path, const char *new_dirname)
{
    const char *path, *ptr;
    size_t dirlen;

    /* Extract file name from old_path. */
    path = old_path;
#ifdef OSYS_PATH_DOS
    if (OSYS_PATH_IS_DRIVE_LETTER(path[0]) && path[1] == ':')
        path += 2;  /* skip drive name */
#endif
    for ( ; ; )
    {
        ptr = strpbrk(path, OSYS_PATH_PATHSEP_ALL_STR);
        if (ptr == NULL)
            break;
        path = ptr + 1;
    }

    /* Make sure the buffer is large enough. */
    dirlen = strlen(new_dirname);
    if (dirlen + strlen(path) + 2 >= bufsize)  /* overflow */
        return NULL;

    /* Copy the new directory name. Also append a slash if necessary. */
    if (dirlen > 0)
    {
        strcpy(buffer, new_dirname);
#ifdef OSYS_PATH_DOS
        if (dirlen == 2 && buffer[1] == ':' &&
            OSYS_PATH_IS_DRIVE_LETTER(buffer[0]))
        {
            /* Special case: do not append slash to "C:". */
        }
        else
#endif
        {
            if (strchr(OSYS_PATH_PATHSEP_ALL_STR, buffer[dirlen - 1]) == NULL)
                buffer[dirlen++] = OSYS_PATH_PATHSEP;
        }
    }

    /* Append the file name. */
    strcpy(buffer + dirlen, path);
    return buffer;
}

/*
 * Creates a new path name by changing the extension component of
 * a specified path name.
 */
char *
osys_path_chext(char *buffer, size_t bufsize,
                const char *old_path, const char *new_extname)
{
    size_t i, pos;

    if (new_extname[0] != OSYS_PATH_EXTSEP)  /* invalid argument */
        return NULL;
    for (i = 0, pos = (size_t)(-1); old_path[i] != 0; ++i)
    {
        if (i >= bufsize)  /* overflow */
            return NULL;
        if ((buffer[i] = old_path[i]) == OSYS_PATH_EXTSEP)
            pos = i;
    }
    if (i > pos)
        i = pos;  /* go back only if old_path has an extension */
    for ( ; ; ++i, ++new_extname)
    {
        if (i >= bufsize)  /* overflow */
            return NULL;
        if ((buffer[i] = *new_extname) == 0)
            return buffer;
    }
}

/*
 * Creates a backup path name.
 */
char *
osys_path_mkbak(char *buffer, size_t bufsize, const char *path)
{
    static const char bak_extname[] = OSYS_PATH_EXTSEP_STR "bak";

    if (strlen(path) + sizeof(bak_extname) > bufsize)
        return NULL;

#if defined OSYS_DOS

    return osys_path_chext(buffer, bufsize, path, bak_extname);

#else  /* OSYS_UNIX and others */

    strcpy(buffer, path);
    strcat(buffer, bak_extname);
    return buffer;

#endif
}

/*
 * Returns the current value of the file position indicator.
 */
osys_foffset_t
osys_ftello(FILE *stream)
{
#if defined OSYS_HAVE_STDIO__I64

    return (osys_foffset_t)_ftelli64(stream);

#elif defined OSYS_UNIX && (OSYS_FOFFSET_MAX > LONG_MAX)

    /* We don't know if off_t is sufficiently wide, we only know that
     * long isn't. We are trying just a little harder, in the absence
     * of an fopen64/ftell64 solution.
     */
    return (osys_foffset_t)ftello(stream);

#else  /* generic */

    return (osys_foffset_t)ftell(stream);

#endif
}

/*
 * Sets the file position indicator at the specified file offset.
 */
int
osys_fseeko(FILE *stream, osys_foffset_t offset, int whence)
{
#if defined OSYS_HAVE_STDIO__I64

    return _fseeki64(stream, (__int64)offset, whence);

#elif defined OSYS_UNIX

#if OSYS_FOFFSET_MAX > LONG_MAX
    /* We don't know if off_t is sufficiently wide, we only know that
     * long isn't. We are trying just a little harder, in the absence
     * of an fopen64/fseek64 solution.
     */
    return fseeko(stream, (off_t)offset, whence);
#else
    return fseek(stream, (long)offset, whence);
#endif

#else  /* generic */

    return (fseek(stream, (long)offset, whence) == 0) ? 0 : -1;

#endif
}

/*
 * Gets the size of the specified file stream.
 */
int
osys_fgetsize(FILE *stream, osys_fsize_t *size)
{
#if defined OSYS_WINDOWS

    HANDLE hFile;
    DWORD dwSizeLow, dwSizeHigh;

    hFile = (HANDLE)_get_osfhandle(_fileno(stream));
    dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
    if (GetLastError() != NO_ERROR)
        return -1;
    *size = (osys_fsize_t)dwSizeLow + ((osys_fsize_t)dwSizeHigh << 32);
    return 0;

#else  /* generic */

    osys_foffset_t offset;

    if (osys_fseeko(stream, 0, SEEK_END) != 0)
        return -1;
    offset = osys_ftello(stream);
    if (offset < 0)
        return -1;
    *size = (osys_fsize_t)offset;
    return 0;

#endif
}

/*
 * Reads a block of data from the specified file offset.
 */
size_t
osys_fread_at(FILE *stream, osys_foffset_t offset, int whence,
              void *block, size_t blocksize)
{
    fpos_t pos;
    size_t result;

    if (fgetpos(stream, &pos) != 0)
        return 0;
    if (osys_fseeko(stream, offset, whence) == 0)
        result = fread(block, 1, blocksize, stream);
    else
        result = 0;
    if (fsetpos(stream, &pos) != 0)
        result = 0;
    return result;
}

/*
 * Writes a block of data at the specified file offset.
 */
size_t
osys_fwrite_at(FILE *stream, osys_foffset_t offset, int whence,
               const void *block, size_t blocksize)
{
    fpos_t pos;
    size_t result;

    if (fgetpos(stream, &pos) != 0 || fflush(stream) != 0)
        return 0;
    if (osys_fseeko(stream, offset, whence) == 0)
        result = fwrite(block, 1, blocksize, stream);
    else
        result = 0;
    if (fflush(stream) != 0)
        result = 0;
    if (fsetpos(stream, &pos) != 0)
        result = 0;
    return result;
}

/*
 * Changes the name of a file path.
 */
int
osys_rename(const char *src_path, const char *dest_path, int clobber)
{
#if defined OSYS_WINDOWS

    DWORD dwFlags;

#if !defined OSYS_WIN64
    if (OSYS_WINDOWS_IS_WIN9X())
    {
        /* MoveFileEx is not available under Win9X; use MoveFile. */
        if (MoveFileA(src_path, dest_path))
            return 0;
        if (!clobber)
            return -1;
        DeleteFileA(dest_path);
        return MoveFileA(src_path, dest_path) ? 0 : -1;
    }
#endif

    dwFlags = clobber ? MOVEFILE_REPLACE_EXISTING : 0;
    return MoveFileExA(src_path, dest_path, dwFlags) ? 0 : -1;

#elif defined OSYS_UNIX

    if (!clobber)
    {
        if (access(dest_path, OSYS_TEST_FILE) >= 0)
            return -1;
    }
    return rename(src_path, dest_path);

#else  /* generic */

    if (osys_test(dest_path, "e") == 0)
    {
        if (!clobber)
            return -1;
        osys_unlink(dest_path);
    }
    return rename(src_path, dest_path);

#endif
}

/*
 * Copies the attributes (access mode, time stamp, etc.) of the source
 * file path onto the destination file path.
 */
int
osys_copy_attr(const char *src_path, const char *dest_path)
{
#if defined OSYS_WINDOWS

    HANDLE hFile;
    FILETIME ftLastWrite;
    BOOL result;

    hFile = CreateFileA(src_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    result = GetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!result)
        return -1;

    hFile = CreateFileA(dest_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        (OSYS_WINDOWS_IS_WIN9X() ? 0 : FILE_FLAG_BACKUP_SEMANTICS), 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    result = SetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!result)
        return -1;

    /* TODO: Copy the access mode. */

    return 0;

#elif defined OSYS_UNIX || defined OSYS_DOSISH

    struct stat sbuf;
    int /* mode_t */ mode;

    if (stat(src_path, &sbuf) != 0)
        return -1;

    mode = (int)sbuf.st_mode;
    if (chmod(dest_path, mode) != 0)
        return -1;

#ifdef AT_FDCWD
    {
        struct timespec times[2];

        times[0] = sbuf.st_atim;
        times[1] = sbuf.st_mtim;
        if (utimensat(AT_FDCWD, dest_path, times, 0) != 0)
            return -1;
    }
#else  /* legacy utime */
    {
        struct utimbuf utbuf;

        utbuf.actime = sbuf.st_atime;
        utbuf.modtime = sbuf.st_mtime;
        if (utime(dest_path, &utbuf) != 0)
            return -1;
    }
#endif

    return 0;

#else  /* generic */

    OSYS_UNUSED(dest_path);
    OSYS_UNUSED(src_path);

    /* Always fail. */
    return -1;

#endif
}

/*
 * Creates a new directory.
 */
int
osys_create_dir(const char *dirname)
{
    size_t len;

    len = strlen(dirname);
    if (len == 0)  /* current directory */
        return 0;

#ifdef OSYS_PATH_DOS
    if (len == 2 && dirname[1] == ':' && OSYS_PATH_IS_DRIVE_LETTER(dirname[0]))
        return 0;
#endif

#if defined OSYS_WINDOWS

    {
        char *wildname;
        HANDLE hFind;
        WIN32_FIND_DATAA wfd;

        /* See if dirname exists: find files in (dirname + "\\*"). */
        if (len + 3 < len)  /* overflow */
            return -1;
        wildname = (char *)malloc(len + 3);
        if (wildname == NULL)  /* out of memory */
            return -1;
        strcpy(wildname, dirname);
        if (strchr(OSYS_PATH_PATHSEP_ALL_STR, wildname[len - 1]) == NULL)
            wildname[len++] = OSYS_PATH_PATHSEP;
        wildname[len++] = '*';
        wildname[len] = '\0';
        hFind = FindFirstFileA(wildname, &wfd);
        free(wildname);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);
            return 0;
        }

        /* There is no directory, so create one now. */
        return CreateDirectoryA(dirname, NULL) ? 0 : -1;
    }

#elif defined OSYS_UNIX || defined OSYS_DOSISH

    {
        struct stat sbuf;

        if (stat(dirname, &sbuf) == 0)
            return (sbuf.st_mode & S_IFDIR) ? 0 : -1;

        /* There is no directory, so create one now. */
#if defined OSYS_UNIX
        return mkdir(dirname, 0777);
#else
        return mkdir(dirname);
#endif
    }

#else  /* generic */

    OSYS_UNUSED(dirname);

    /* Always fail. */
    return -1;

#endif
}

/*
 * Determines if the accessibility of the specified path satisfies
 * the specified access mode.
 */
int
osys_test(const char *path, const char *mode)
{
    int faccess, freg;

    faccess = freg = 0;
    if (strchr(mode, 'f') != NULL)
        freg = 1;
    if (strchr(mode, 'r') != NULL)
        faccess |= OSYS_TEST_READ;
    if (strchr(mode, 'w') != NULL)
        faccess |= OSYS_TEST_WRITE;
    if (strchr(mode, 'x') != NULL)
        faccess |= OSYS_TEST_EXEC;
    if (faccess == 0 && freg == 0)
    {
        if (strchr(mode, 'e') == NULL)
            return 0;
    }

#if defined OSYS_WINDOWS

    {
        DWORD attr;

        attr = GetFileAttributesA(path);
        if (attr == 0xffffffffU)
            return -1;
        if (freg && (attr & FILE_ATTRIBUTE_DIRECTORY))
            return -1;
        if ((faccess & OSYS_TEST_WRITE) && (attr & FILE_ATTRIBUTE_READONLY))
            return -1;
        return 0;
    }

#elif defined OSYS_UNIX || defined OSYS_DOSISH

    {
        struct stat sbuf;

        if (stat(path, &sbuf) != 0)
            return -1;
        if (freg && !(sbuf.st_mode & S_IFREG))
            return -1;
        if (faccess == 0)
            return 0;
        return access(path, faccess);
    }

#else  /* generic */

    {
        FILE *stream;

        if (faccess & OSYS_TEST_WRITE)
            stream = fopen(path, "r+b");
        else
            stream = fopen(path, "rb");
        if (stream == NULL)
            return -1;
        fclose(stream);
        return 0;
    }

#endif
}

/*
 * Determines if two accessible paths are equivalent, i.e. they
 * point to the same physical location.
 */
int
osys_test_eq(const char *path1, const char *path2)
{
#if defined OSYS_WINDOWS

    HANDLE hFile1, hFile2;
    BY_HANDLE_FILE_INFORMATION fileInfo1, fileInfo2;
    int result;

    hFile1 = CreateFileA(path1, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile1 == INVALID_HANDLE_VALUE)
        return -1;
    hFile2 = CreateFileA(path2, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile2 == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile1);
        return -1;
    }
    if (!GetFileInformationByHandle(hFile1, &fileInfo1) ||
        !GetFileInformationByHandle(hFile2, &fileInfo2))
    {
        /* Can't retrieve the file info. */
        result = -1;
    }
    else
    if (fileInfo1.nFileIndexLow == fileInfo2.nFileIndexLow &&
        fileInfo1.nFileIndexHigh == fileInfo2.nFileIndexHigh &&
        fileInfo1.dwVolumeSerialNumber == fileInfo2.dwVolumeSerialNumber)
    {
        /* The two paths have the same ID on the same volume. */
        result = 1;
    }
    else
    {
        /* The two paths have different IDs or sit on different volumes. */
        result = 0;
    }
    CloseHandle(hFile1);
    CloseHandle(hFile2);
    return result;

#elif defined OSYS_UNIX || defined OSYS_DOSISH

    struct stat sbuf1, sbuf2;

    if (stat(path1, &sbuf1) != 0 || stat(path2, &sbuf2) != 0)
    {
        /* Can't stat the paths. */
        return -1;
    }
    if (sbuf1.st_dev == sbuf2.st_dev && sbuf1.st_ino == sbuf2.st_ino)
    {
        /* The two paths have the same device and inode numbers. */
        /* The inode numbers are reliable only if they're not 0. */
        return (sbuf1.st_ino != 0) ? 1 : -1;
    }
    else
    {
        /* The two paths have different device or inode numbers. */
        return 0;
    }

#else  /* generic */

    OSYS_UNUSED(path1);
    OSYS_UNUSED(path2);

    /* Always unknown. */
    return -1;

#endif
}

/*
 * Removes a directory entry.
 */
int
osys_unlink(const char *path)
{
#if defined OSYS_WINDOWS

    return DeleteFileA(path) ? 0 : -1;

#elif defined OSYS_OS_UNIX || defined OSYS_OS_DOSISH

    return unlink(path);

#else  /* generic */

    return remove(path);

#endif
}

/*
 * Prints an error message to stderr and terminates the program
 * execution immediately, exiting with code 70 (EX_SOFTWARE).
 */
void
osys_terminate(void)
{
    static const char *msg =
        "The execution of this program has been terminated abnormally.\n";
    fputs(msg, stderr);
    exit(70);  /* EX_SOFTWARE */
}
