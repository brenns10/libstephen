/***************************************************************************//**

  @file         str.h

  @author       Stephen Brennan

  @date         Created Sunday, 29 June 2014

  @brief        Common functions for working with strings.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef SMB_CKY_STR_H
#define SMB_CKY_STR_H

#include <wchar.h>
#include "libstephen/ll.h"

int hexit_val(wchar_t digit);
wchar_t get_escape(const wchar_t **source);
int read_escape(const wchar_t *source, int len, wchar_t *out);
int read_wchar(const wchar_t *source, int len, wchar_t *out);
wchar_t *escape_wchar(wchar_t input);

#endif
