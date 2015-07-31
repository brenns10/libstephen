/***************************************************************************//**

  @file         str.c

  @author       Stephen Brennan

  @date         Created Sunday, 29 June 2014

  @brief        Regex and FSM string functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <wchar.h>
#include <wctype.h>

#include "libstephen/ll.h"
#include "libstephen/fsm.h"
#include "str.h"

int hexit_val(wchar_t digit)
{
  if (iswdigit(digit)) {
    return digit - L'0';
  } else if (digit == L'a' || digit == L'b' || digit == L'c' || digit == L'd'
             || digit == L'e' || digit == L'f' || digit == L'A' || digit == L'B'
             || digit == L'C' || digit == L'D' || digit == L'E' || digit == L'F'){
    return 10 + towupper(digit) - L'A';
  }
  return -1;
}

wchar_t get_escape(const wchar_t **source)
{
  wchar_t value = 0;
  wchar_t specifier = **source;
  (*source)++;
  switch (specifier) {
  case L'a':
    return L'\a';
  case L'b':
    return L'\b';
  case L'f':
    return L'\f';
  case L'n':
    return L'\n';
  case L'r':
    return L'\r';
  case L't':
    return L'\t';
  case L'v':
    return L'\v';
  case L'\\':
    return L'\\';
  case L'x':
    value += 16 * hexit_val(**source);
    (*source)++;
    value += hexit_val(**source);
    (*source)++;
    return value;
  case L'u':
    value += 16 * 16 * 16 * hexit_val(**source);
    (*source)++;
    value += 16 * 16 * hexit_val(**source);
    (*source)++;
    value += 16 * hexit_val(**source);
    (*source)++;
    value += hexit_val(**source);
    (*source)++;
    return value;
  default:
    return specifier;
  }
}

int read_escape(const wchar_t *source, int len, wchar_t *out)
{
  wchar_t specifier;
  if (len < 2) {
    *out = WEOF;
    return len;
  }
  specifier = source[1];
  *out = 0;
  switch (specifier) {
  case L'a':
    *out = L'\a';
    return 2;
  case L'b':
    *out = L'\b';
    return 2;
  case L'e':
    *out = EPSILON;
    return 2;
  case L'f':
    *out = L'\f';
    return 2;
  case L'n':
    *out = L'\n';
    return 2;
  case L'r':
    *out = L'\r';
    return 2;
  case L't':
    *out = L'\t';
    return 2;
  case L'v':
    *out = L'\v';
    return 2;
  case L'\\':
    *out = L'\\';
    return 2;
  case L'x':
    if (len < 4) {
      *out = WEOF;
      return len;
    }
    *out += 16 * hexit_val(source[2]);
    *out += hexit_val(source[3]);
    return 4;
  case L'u':
    if (len < 6) {
      *out = WEOF;
      return len;
    }
    *out += 16 * 16 * 16 * hexit_val(source[2]);
    *out += 16 * 16 * hexit_val(source[3]);
    *out += 16 * hexit_val(source[4]);
    *out += hexit_val(source[5]);
    return 6;
  default:
    *out = specifier;
    return 2;
  }
}

wchar_t *escape_wchar(wchar_t input)
{
  static wchar_t buf[] = {L' ', L'\0'};
  switch (input) {
  case L'\a':
    return L"\\a";
  case L'\b':
    return L"\\b";
  case EPSILON:
    return L"\\e";
  case L'\f':
    return L"\\f";
  case L'\n':
    return L"\\n";
  case L'\r':
    return L"\\r";
  case L'\t':
    return L"\\t";
  case L'\v':
    return L"\\v";
  case L'\\':
    return L"\\\\";
  default:
    // Print other characters as verbatim
    buf[0] = input;
    return buf;
  }
}

int read_wchar(const wchar_t *source, int len, wchar_t *out)
{
  if (source[0] == L'\\') {
    return read_escape(source, len, out);
  } else {
    *out = source[0];
    return 1;
  }
}
