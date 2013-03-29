/*
 * cbitset.c
 * Plain old bitset data type.
 *
 * Copyright (C) 2001-2010 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the attached LICENSE for more information.
 */


#include "cbitset.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>


/*
 * Private helper macros: _bitset_MIN and _bitset_MAX.
 */
#define _bitset_MIN(a, b) \
    ((a) < (b) ? (a) : (b))
#define _bitset_MAX(a, b) \
    ((a) > (b) ? (a) : (b))


/*
 * Private helper macro: _bitset_PTR_SKIP_PRED.
 *
 * Skips the given pointer past the elements that satisfy the given predicate.
 * E.g., _bitset_PTR_SKIP_PRED(str, isspace) skips the leading whitespace.
 */
#define _bitset_PTR_SKIP_PRED(ptr, predicate) \
    { while (predicate(*(ptr))) ++(ptr); }


/*
 * Counts the number of elements in a bitset.
 */
unsigned int bitset_count(bitset_t set)
{
    unsigned int result;

    /* Apply Wegner's method. */
    result = 0;
    while (set != BITSET_EMPTY)
    {
        set &= (set - 1);
        ++result;
    }
    return result;
}


/*
 * Finds the first element in a bitset.
 */
int bitset_find_first(bitset_t set)
{
    int i;

    for (i = 0; i <= BITSET_ELT_MAX; ++i)
    {
        if (bitset_test(set, i))
            return i;
    }
    return -1;
}


/*
 * Finds the next element in a bitset.
 */
int bitset_find_next(bitset_t set, int elt)
{
    int i;

    for (i = _bitset_MAX(elt, -1) + 1; i <= BITSET_ELT_MAX; ++i)
    {
        if (bitset_test(set, i))
            return i;
    }
    return -1;
}


/*
 * Finds the last element in a bitset.
 */
int bitset_find_last(bitset_t set)
{
    int i;

    for (i = BITSET_ELT_MAX; i >= 0; --i)
    {
        if (bitset_test(set, i))
            return i;
    }
    return -1;
}


/*
 * Finds the previous element in a bitset.
 */
int bitset_find_prev(bitset_t set, int elt)
{
    int i;

    for (i = _bitset_MIN(elt, BITSET_ELT_MAX + 1) - 1; i >= 0; --i)
    {
        if (bitset_test(set, i))
            return i;
    }
    return -1;
}


/*
 * Converts a string to a bitset.
 */
bitset_t string_to_bitset(const char *str, size_t *end_idx)
{
    bitset_t result;
    const char *ptr;
    int out_of_range;

    ptr = str;
    _bitset_PTR_SKIP_PRED(ptr, isspace);
    if (*ptr != '0' && *ptr != '1')
    {
        /* Invalid input. */
        if (end_idx != NULL)
            *end_idx = 0;
#ifdef EINVAL
        errno = EINVAL;
#endif
        return BITSET_EMPTY;
    }

    result = BITSET_EMPTY;
    out_of_range = 0;
    for ( ; ; ++ptr)
    {
        if (*ptr == '0' || *ptr == '1')
        {
            result = (result << 1) | (*ptr - '0');
            if (bitset_test(result, BITSET_ELT_MAX))
                out_of_range = 1;
        }
        else
        {
            if (end_idx != NULL)
                *end_idx = (size_t)(ptr - str);
            if (out_of_range)
            {
                bitset_set(&result, BITSET_ELT_MAX);
#ifdef ERANGE
                errno = ERANGE;
#endif
            }
            return result;
        }
    }
}


/*
 * Converts a bitset to a string.
 */
size_t bitset_to_string(char *sbuf, size_t sbuf_size, bitset_t set)
{
    size_t result;
    char *ptr;
    int i;

    for (i = BITSET_ELT_MAX; i > 0; --i)
    {
        if (bitset_test(set, i))
            break;
    }

    result = (size_t)i + 1;
    if (result >= sbuf_size)
    {
        /* Buffer too small. */
        return result;
    }

    ptr = sbuf;
    for ( ; i >= 0; --i)
        *ptr++ = (char)(bitset_test(set, i) ? '1' : '0');
    *ptr = (char)0;
    return result;
}


/*
 * Converts a rangeset string to a bitset.
 */
bitset_t rangeset_string_to_bitset(const char *str, size_t *end_idx)
{
    bitset_t result;
    const char *ptr;
    int state;
    int num, num1, num2;
    int out_of_range;

    result = BITSET_EMPTY;
    ptr = str;
    state = 0;
    out_of_range = 0;
    num1 = num2 = -1;

    for ( ; ; )
    {
        _bitset_PTR_SKIP_PRED(ptr, isspace);
        switch (state)
        {
        case 0:  /* "" */
        case 2:  /* "N-" */
            /* Expecting number; go to next state. */
            if (*ptr >= '0' && *ptr <= '9')
            {
                num = 0;
                do
                {
                    num = 10 * num + (*ptr - '0');
                    if (num > BITSET_ELT_MAX)
                    {
                        out_of_range = 1;
                        num = BITSET_ELT_MAX;
                    }
                    ++ptr;
                } while (*ptr >= '0' && *ptr <= '9');
                if (state == 0)
                    num1 = num;
                num2 = num;
                ++state;
                continue;
            }
            break;
        case 1:  /* "N" */
            /* Expecting range operator; go to next state. */
            if (*ptr == '-')
            {
                ++ptr;
                num2 = BITSET_ELT_MAX;
                ++state;
                continue;
            }
            break;
        }

        if (state > 0)  /* "N", "N-" or "N-N" */
        {
            /* Store the partial result; go to state 0. */
            state = 0;
            if (num2 > BITSET_ELT_MAX)
            {
                out_of_range = 1;
                num2 = BITSET_ELT_MAX;
            }
            if (num1 <= num2)
                bitset_set_range(&result, num1, num2);
            else
                out_of_range = 1;
        }

        if (*ptr == ',' || *ptr == ';')
        {
            /* Separator: continue the loop. */
            ++ptr;
            continue;
        }
        else
        {
            /* Unexpected character or end of string: break the loop. */
            break;
        }
    }

    if (num1 == -1)
    {
        /* There were no partial results. */
        if (end_idx != NULL)
            *end_idx = 0;
        /* No EINVAL here: the empty set is a valid input. */
        return BITSET_EMPTY;
    }
    if (end_idx != NULL)
        *end_idx = (size_t)(ptr - str);
#ifdef ERANGE
    if (out_of_range)
        errno = ERANGE;
#endif
    return result;
}


/*
 * Converts a bitset to a rangeset string.
 */
size_t bitset_to_rangeset_string(char *sbuf, size_t sbuf_size, bitset_t set);
/* not implemented */
