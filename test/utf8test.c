/***************************************************************************//**

  @file         utf8test.c

  @author       Stephen Brennan

  @date         Created Wednesday,  9 July 2014

  @brief        Tests the utf8 functions.

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

#include "libstephen/util.h"
#include "libstephen/ut.h"
#include "tests.h"

int utf8_test_predetermined(void)
{
  #define BFSZ 5
  wchar_t result[BFSZ];

  // Code point U+1F602 -- Face with tears of joy
  // Code point in binary: 0001  1111 0110  0000 0010
  // 17 bits, so 4 bytes in UTF-8
  // UTF-8 template: 1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx
  // UTF-8 binary:   1111 0000  1001 1111  1001 1000  1000 0010
  // UTF-8 hex:         F    0     9    F     9    8     8    2
  char *face = "\xF0\x9F\x98\x82";
  utf8toucs4(result, face, BFSZ);
  TEST_ASSERT(wcscmp(result, L"\U0001F602") == 0);

  // Code point U+4EBA -- Chinese character ren ('man')
  // Code point in binary: 0100 1110 1011 1010
  // 15 bits, so 3 bytes in UTF-8
  // UTF-8 template: 1110 xxxx  10xx xxxx  10xx xxxx
  // UTF-8 binary:   1110 0100  1011 1010  1011 1010
  // UTF-8 hex:         E    4     B    A     B    A
  char *ren = "\xE4\xBA\xBA";
  utf8toucs4(result, ren, BFSZ);
  TEST_ASSERT(wcscmp(result, L"\u4EBA") == 0);

  // Code point U+0101 -- Latin small letter a with macron
  // Code point in binary: 0000 0001  0000 0001
  // 9 bits, so 2 bytes in UTF-8
  // UTF-8 template: 110x xxxx  10xx xxxx
  // UTF-8 binary:   1100 0100  1000 0001
  // UTF-8 hex:         C    4     8    1
  char *amac = "\xC4\x81";
  utf8toucs4(result, amac, BFSZ);
  TEST_ASSERT(wcscmp(result, L"\u0101") == 0);

  // Code point U+0061 -- Latin small letter a
  // Code point in binary: 0110 0001
  // 7 bits, so 1 byte in UTF-8
  // UTF-8 hex: 61
  char *a = "\x61";
  utf8toucs4(result, a, BFSZ);
  TEST_ASSERT(wcscmp(result, L"a") == 0);

  char *alltogether = "\xF0\x9F\x98\x82\xE4\xBA\xBA\xC4\x81\x61";
  utf8toucs4(result, alltogether, BFSZ);
  TEST_ASSERT(wcscmp(result, L"\U0001F602\u4EBA\u0101a") == 0);

  return 0;
}

void utf8_test(void)
{
  smb_ut_group *group = su_create_test_group("utf8");

  smb_ut_test *predetermined = su_create_test("predetermined",
                                              utf8_test_predetermined, 1);
  su_add_test(group, predetermined);

  su_run_group(group);
  su_delete_group(group);
}
