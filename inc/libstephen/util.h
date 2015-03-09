/***************************************************************************//**

  @file         util.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Utilities

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_UTIL_H
#define LIBSTEPHEN_UTIL_H

#include <stdio.h>    /* FILE    */
#include <wchar.h>    /* wchar_t */

wchar_t *smb_read_linew(FILE *, int *);
char *smb_read_line(FILE *, int *);
void smb_set_memory_log_location(char *);
int utf8toucs4(wchar_t *dest, const char *src, int n);
wint_t utf8char(const char *src, int s);
wint_t utf8iter(const char *_src, int *_s);

#endif // LIBSTEPHEN_UTIL_H
