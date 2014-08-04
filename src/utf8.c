/***************************************************************************//**

  @file         utf8.c

  @author       Stephen Brennan

  @date         Created Tuesday,  8 July 2014

  @brief        Functions for converting between UTF-8 and UCS-4.

  @copyright    Copyright (c) 2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <stdio.h>
#include <wchar.h>

#include "libstephen/base.h"
#include "libstephen/util.h"

/*******************************************************************************

                                  UTF-8 Format

  UTF-8 takes a Unicode code point and turns it into a 1, 2, 3, or 4 byte
  sequence.  This table shows the code points and how they translate:

  Bits  Code point range      Bytes  Layout
  ----  ----------------      -----  ------
  7     U+0000  -- U+007F     1      0xxx xxxx
  11    U+0080  -- U+07FF     2      110x xxxx  10xx xxxx
  16    U+0800  -- U+FFFF     3      1110 xxxx  10xx xxxx  10xx xxxx
  21    U+10000 -- U+1FFFFF   4      1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx

*******************************************************************************/


#define SINGLE_MASK  0x80u // 1000 0000
#define SINGLE_VALUE 0x00u // 0xxx xxxx

#define DOUBLE_MASK  0xE0u // 1110 0000
#define DOUBLE_VALUE 0xC0u // 110x xxxx

#define TRIPLE_MASK  0xF0u // 1111 0000
#define TRIPLE_VALUE 0xE0u // 1110 xxxx

#define QUAD_MASK    0xF8u // 1111 1000
#define QUAD_VALUE   0xF0u // 1111 0xxx

#define CONT_BITS    6
#define CONT_MASK    0x3Fu // 0011 1111
#define CONT_VALUE   0x80  // 10xx xxxx

/**
   @brief A function supporting iteration through a UTF-8 string.

   Use this function to iterate through a UTF-8 string on a per-codepoint basis,
   rather than on a per-byte basis.  This is useful only if you're going to *do
   something* with the code points.  Otherwise, it is wise to act in an
   encoding-agnostic way (as UTF-8 allows) and act in a bytewise manner.

   This function uses a pointer to an iteration variable.  Initially, the
   variable is set to the index of the next code point in the string.  The
   funtion reads this character from the string, updates the iteration variable
   to hold the index of the next code point, and returns the character.

   @param _src The UTF-8 string to iterate through.
   @param _s A pointer to the iteration variable.
   @returns The character at `_src[*_s]`, with *_s updated.
   @retval WEOF on error
 */
wint_t utf8iter(const char *_src, int *_s)
{
  const unsigned char *src = (const char *) _src;
  int cont, s = *_s;
  wint_t dest;

  // Read the first byte in a sequence
  if ((src[s] & SINGLE_MASK) == SINGLE_VALUE) {
    dest = src[s++];
    *_s = s;
    return dest;
  } else if ((src[s] & DOUBLE_MASK) == DOUBLE_VALUE) {
    dest = (~DOUBLE_MASK) & src[s++];
    cont = 1;
  } else if ((src[s] & TRIPLE_MASK) == TRIPLE_VALUE) {
    dest = (~TRIPLE_MASK) & src[s++];
    cont = 2;
  } else if ((src[s] & QUAD_MASK) == QUAD_VALUE) {
    dest = (~QUAD_MASK) & src[s++];
    cont = 3;
  } else {
    PRINT_ERROR_LOC;
    fprintf(stderr, "error: bad initial byte(0x%2.2x)\n", src[s]);
    return WEOF;
  }

  // Read continuation bits:
  for ( ; cont > 0 && src[s] != '\0'; s++, cont--) {
    if ((src[s] & ~CONT_MASK) != CONT_VALUE) {
      PRINT_ERROR_LOC;
      fprintf(stderr, "error: bad continuation byte(0x%2.2x)\n", src[s]);
      *_s = s;
      return WEOF;
    }
    dest <<= CONT_BITS;
    dest |= src[s] & CONT_MASK;
  }
  *_s = s;

  if (cont != 0) {
    PRINT_ERROR_LOC;
    fprintf(stderr, "error: unexpected string termination while reading "
            "continuation bytes.\n");
    return WEOF;
  }
  return dest;
}

/**
   @brief Return the wide character represented at the given index in a string.
   @param src UTF-8 formatted string
   @param s Index of the *first byte* of a character
   @returns WEOF on any error, else the wide character at the given index.
 */
wint_t utf8char(const char *src, int s)
{
  return utf8iter(src, &s);
}


/**
   @brief Convert a UTF-8 char string to a UCS-4 wchar_t string.
   @param dest Destination buffer.
   @param _src Source buffer.
   @param n Number of characters allocated in destination buffer.
   @returns 0 if converted OK, <0 otherwise.
 */
int utf8toucs4(wchar_t *dest, const char *src, int n)
{
  int d = 0, s = 0;
  wint_t wc;
  for ( ; src[s] != '\0' && d < n-1; d++) {
    wc = utf8iter(src, &s);
    if (wc == WEOF) {
      PRINT_ERROR_LOC;
      fprintf(stderr, "error: UTF-8 character could not be converted\n");
      return -1;
    }
    dest[d] = (wchar_t) wc;
  }
  dest[d] = L'\0';
  return 0;
}
