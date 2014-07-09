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

#include "libstephen.h"


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
   @brief Convert a UTF-8 char string to a UCS-4 wchar_t string.
   @param dest Destination buffer.
   @param _src Source buffer.
   @param n Number of characters allocated in destination buffer.
   @returns 0 if converted OK, <0 otherwise.
 */
int utf8toucs4(wchar_t *dest, const char *_src, int n)
{
  // Initially assume that dest can hold src.
  const unsigned char *src = (const unsigned char *) _src;
  int d = 0, s = 0, cont;

  for ( ; src[s] && d < n-1; d++) {
    // Read the first byte in a sequence
    if ((src[s] & SINGLE_MASK) == SINGLE_VALUE) {
      dest[d] = src[s++];
      continue;
    } else if ((src[s] & DOUBLE_MASK) == DOUBLE_VALUE) {
      dest[d] = (~DOUBLE_MASK) & src[s++];
      cont = 1;
    } else if ((src[s] & TRIPLE_MASK) == TRIPLE_VALUE) {
      dest[d] = (~TRIPLE_MASK) & src[s++];
      cont = 2;
    } else if ((src[s] & QUAD_MASK) == QUAD_VALUE) {
      dest[d] = (~QUAD_MASK) & src[s++];
      cont = 3;
    } else {
      PRINT_ERROR_LOC;
      fprintf(stderr, "error: bad initial byte(0x%2.2x): byte %d of \"%s\"\n",
              src[s], s, _src);
      return -1;
    }

    // Read continuation bits:
    for ( ; cont > 0 && src[s] != '\0'; s++, cont--) {
      if ((src[s] & ~CONT_MASK) != CONT_VALUE) {
        PRINT_ERROR_LOC;
        fprintf(stderr, "error: bad continuation byte(0x%2.2x): byte %d of"
                " \"%s\"\n", src[s], s, _src);
        return -2;
      }
      dest[d] <<= CONT_BITS;
      dest[d] |= src[s] & CONT_MASK;
    }

    if (cont != 0) {
      PRINT_ERROR_LOC;
      fprintf(stderr, "error: unexpected string termination while reading "
              "continuation bytes.\n");
      return -3;
    }
  }
  dest[d] = L'\0';
  return 0;
}
