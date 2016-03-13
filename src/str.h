/***************************************************************************//**

  @file         str.h

  @author       Stephen Brennan

  @date         Created Sunday, 29 June 2014

  @brief        Common functions for working with strings.

  @copyright    Copyright (c) 2015-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef SMB_CKY_STR_H
#define SMB_CKY_STR_H

#include <wchar.h>
#include "libstephen/ll.h"

/**
   @brief Get the value of a hexadecimal digit.
   @param digit The digit
   @return The value of the digit in hexadecimal.
 */
int hexit_val(wchar_t digit);
/**
   @brief Get an escaped character from the string source.

   This function will advance the source pointer to after the escape sequence.
   It can get escapes `abfnrtv\xu`, which includes hexadecimal and unicode
   escapes.
   @brief source The source pointer
   @return The character that was escaped
 */
wchar_t get_escape(const wchar_t **source);
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
int read_escape(const wchar_t *source, int len, wchar_t *out);
/**
   @brief Read a single character from the string, accepting escape sequences.
   @param source The string to read from.
   @param len Number of characters in the string. (assumed >= 1)
   @param out Place to store the resulting character.
   @return Number of characters read from source.
   @exception Sets *out to WEOF if the read would go past len.
 */
int read_wchar(const wchar_t *source, int len, wchar_t *out);
/**
   @brief Return a string representation of a character, escaped if necessary.

   The returned string is statically allocated, but only valid until the next
   time fsm_print_char() is called.
   @param input The character to filter.
   @returns A statically allocated, escaped version of the character.  Only
   valid until next call to escape_wchar().
 */
wchar_t *escape_wchar(wchar_t input);

#endif
