/***************************************************************************//**

  @file         str.c

  @author       Stephen Brennan

  @date         Created Sunday, 29 June 2014

  @brief        Common functions for working with strings.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <wchar.h>
#include <wctype.h>

#include "libstephen/ll.h"
#include "libstephen/fsm.h"
#include "str.h"

/**
   @brief Get the value of a hexadecimal digit.
   @param digit The digit
   @return The value of the digit in hexadecimal.
 */
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

/**
   @brief Get an escaped character from the string source.

   This function will advance the source pointer to after the escape sequence.
   It can get escapes `abfnrtv\xu`, which includes hexadecimal and unicode
   escapes.

   @brief source The source pointer
   @return The character that was escaped
 */
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

/**
   @brief Place the escaped wchar in source into out.

   Source should be a string "\..." containing an escape sequence.  It should
   include the backslash.  This function will read the escape sequence, convert
   it to a wchar_t, store that in out, and return the number of characters read.
   @param source The string escape sequence to translate.
   @param len The length of the string to read. (assumed >= 1)
   @param out Where to store the output character.
   @return The number of characters read.
   @exception Sets *out to WEOF if it would readpast len.
 */
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

/**
   @brief Return a string representation of a character, escaped if necessary.

   The returned string is statically allocated, but only valid until the next
   time fsm_print_char() is called.
   @param dest The file to print to.
   @param input The character to filter.
 */
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

/**
   @brief Read a single character from the string, accepting escape sequences.
   @param source The string to read from.
   @param len Number of characters in the string. (assumed >= 1)
   @param out Place to store the resulting character.
   @return Number of characters read from source.
   @exception Sets *out to WEOF if the read would go past len.
 */
int read_wchar(const wchar_t *source, int len, wchar_t *out)
{
  if (source[0] == L'\\') {
    return read_escape(source, len, out);
  } else {
    *out = source[0];
    return 1;
  }
}
