/*
 * cbitset.h
 * Plain old bitset data type.
 *
 * Copyright (C) 2001-2010 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the attached LICENSE for more information.
 */


#ifndef CBITSET_H
#define CBITSET_H


#include <limits.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/*
 * The bitset type.
 */
typedef unsigned int bitset_t;
/* TO DO: bitset8_t, bitset16_t, bitset32_t, ..., bitsetmax_t. */


/*
 * The size operator (not restricted to bitset_t).
 */
#define BITSIZEOF(object) (sizeof(object) * CHAR_BIT)


/*
 * Bitset constants.
 */
#define BITSET_EMPTY 0U
#define BITSET_FULL (~BITSET_EMPTY)


/*
 * Bitset limits.
 */
enum
{
    BITSET_ELT_MIN = 0,
    BITSET_ELT_MAX = (int)(BITSIZEOF(bitset_t) - 1)
};


/*
 * Direct bitset access methods.
 */
#ifdef __cplusplus

inline int bitset_test(bitset_t set, int elt)
{
    return (set & (1U << elt)) != 0;
}

inline void bitset_set(bitset_t *set, int elt)
{
    *set |= (1U << elt);
}

inline void bitset_reset(bitset_t *set, int elt)
{
    *set &= ~(1U << elt);
}

inline void bitset_flip(bitset_t *set, int elt)
{
    *set ^= (1U << elt);
}

inline void bitset_set_range(bitset_t *set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set |= (((1U << (stop_elt - start_elt) << 1) - 1) << start_elt);
}

inline void bitset_reset_range(bitset_t *set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set &= ~(((1U << (stop_elt - start_elt) << 1) - 1) << start_elt);
}

inline void bitset_flip_range(bitset_t *set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set ^= (((1U << (stop_elt - start_elt) << 1) - 1) << start_elt);
}

#else  /* !__cplusplus */

#define bitset_test(set, elt) \
    (((set) & (1U << (elt))) != 0)

#define bitset_set(set, elt) \
    (*(set) |= (1U << (elt)))

#define bitset_reset(set, elt) \
    (*(set) &= ~(1U << (elt)))

#define bitset_flip(set, elt) \
    (*(set) ^= (1U << (elt)))

#define bitset_set_range(set, start_elt, stop_elt) \
    (*(set) |= ((start_elt) <= (stop_elt)) \
        ? (((1U << ((stop_elt) - (start_elt)) << 1) - 1) << (start_elt)) \
        : 0U)

#define bitset_reset_range(set, start_elt, stop_elt) \
    (*(set) &= ((start_elt) <= (stop_elt)) \
        ? ~(((1U << ((stop_elt) - (start_elt)) << 1) - 1) << (start_elt)) \
        : ~0U)

#define bitset_flip_range(set, start_elt, stop_elt) \
    (*(set) ^= ((start_elt) <= (stop_elt)) \
        ? (((1U << ((stop_elt) - (start_elt)) << 1) - 1) << (start_elt)) \
        : 0U)

#endif  /* __cplusplus */


/*
 * Counts the number of elements in a bitset.
 *
 * The function returns the number of bits set to 1.
 */
unsigned int bitset_count(bitset_t set);


/*
 * Finds the first element in a bitset.
 *
 * The function returns the position of the first bit set to 1,
 * or -1 if all bits are set to 0.
 */
int bitset_find_first(bitset_t set);


/*
 * Finds the next element in a bitset.
 *
 * The function returns the position of the next bit set to 1,
 * or -1 if all the following bits are set to 0.
 */
int bitset_find_next(bitset_t set, int elt);


/*
 * Finds the last element in a bitset.
 *
 * The function returns the position of the last bit set to 1,
 * or -1 if all bits are set to 0.
 */
int bitset_find_last(bitset_t set);


/*
 * Finds the previous element in a bitset.
 *
 * The function returns the position of the previous bit set to 1,
 * or -1 if all the preceding bits are set to 0.
 */
int bitset_find_prev(bitset_t set, int elt);


/*
 * Converts a string to a bitset.
 *
 * The function skips the leading whitespace characters, parses
 * the sequence of '0' and '1' characters, and stops at the first
 * character that is not recognized.
 *
 * If end_idx is not null, the function sets *end_idx to point to
 * the character that stopped the scan.  If the input is invalid
 * and end_idx is not null, the function sets *end_idx to 0.
 *
 * The function returns the value of the converted bitset.
 * If the result of conversion cannot be represented in a bitset_t,
 * the function sets errno to ERANGE and returns the lower-order
 * bits from the result, with the highest-order bit set to 1.
 * If the input is invalid, the function sets errno to EINVAL and
 * returns BITSET_EMPTY.
 */
bitset_t string_to_bitset(const char *str, size_t *end_idx);


/*
 * Converts a bitset to a string.
 *
 * The function converts the bitset to a string representation, and
 * attempts to store it in sbuf, if sbuf is large enough.  Otherwise,
 * it leaves sbuf intact.
 *
 * The function returns the length of the string representation.
 */
size_t bitset_to_string(char *sbuf, size_t sbuf_size, bitset_t set);


/*
 * Converts a rangeset string to a bitset.
 *
 * A rangeset string is an arbitrary sequence of elements ("N") and
 * ranges ("M-N" or "M-"), separated by ',' or ';'.  Whitespace is
 * allowed around lexical elements, and is ignored.
 *
 * Here are a few examples, assuming BITSIZEOF(bitset_t) == 16:
 *  "0,3,5-7"  => 0000000011101001
 *  "0-3,5,7-" => 1111111110101111
 *  "8-,4"     => 1111111100010000
 *  ""         => 0000000000000000
 *  "8-4"      => 0000000000000000, errno = ERANGE
 *  "99"       => 1000000000000000, errno = ERANGE
 *  "invalid"  => 0000000000000000, errno = EINVAL
 *
 * If end_idx is not null, the function sets *end_idx to point to
 * the character that stopped the scan.  If the input is invalid and
 * end_idx is not null, the function sets *end_idx to 0.
 *
 * The function returns the value of the converted bitset.  If the
 * input contains non-representable elements or ranges (e.g. elements
 * larger than BITSET_ELT_MAX), the function sets errno to ERANGE.
 * If the input is invalid, the function sets errno to EINVAL and
 * returns BITSET_EMPTY.
 */
bitset_t rangeset_string_to_bitset(const char *str, size_t *end_idx);


/*
 * Converts a bitset to a rangeset string.
 *
 * The function converts the bitset to a rangeset string representation
 * and attempts to store it in sbuf, if sbuf is large enough.  Otherwise,
 * it leaves sbuf intact.
 *
 * The function returns the length of the rangeset string representation.
 */
size_t bitset_to_rangeset_string(char *sbuf, size_t sbuf_size, bitset_t set);


/*
 * TO DO:
 * wstring_to_bitset,
 * bitset_to_wstring,
 * rangeset_wstring_to_bitset,
 * bitset_to_rangeset_wstring.
 */


#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif  /* CBITSET_H */
