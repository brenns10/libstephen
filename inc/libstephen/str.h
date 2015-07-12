/***************************************************************************//**

  @file         str.h

  @author       Stephen Brennan

  @date         Created Friday, 10 July 2015

  @brief        Libstephen string functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_STR_H
#define SMB_STR_H

#include <wchar.h>
#include <stdio.h>

#include "libstephen/ll.h"

/**
   @brief Read a file into a string and return a pointer to it.
   @param f The file to read.

   It is the caller's responsibility to free() the returned pointer!
 */
char *read_file(FILE *f);
/**
   @brief Read a file into a wide string and return a pointer to it.
   @param f The file to read.

   It is the caller's responsibility to free() the returned pointer!
 */
wchar_t *read_filew(FILE *f);

/**
   @brief Read line from file into string, and return pointer to it.
   @param file File to read line from.

   It is the caller's responsibility to free() the returned pointer!
 */
char *read_line(FILE *file);
/**
   @brief Read line from file into wide string, and return pointer to it.
   @param file File to read line from.

   It is the caller's responsibility to free() the returned pointer!
 */
wchar_t *read_linew(FILE *file);

/**
   @brief Return a list of lines from the given string.
   @param source The string to split into lines.
   @return A linked list containing char* to each line.

   The string is modified!  You should make a copy before doing this.  Also, it
   is the caller's responsibility to free the returned list.
 */
smb_ll *split_lines(char *source);
/**
   @brief Return a list of lines from the given wide string.
   @param source The string to split into lines.
   @return A linked list containing wchar_t* to each line.

   The string is modified!  You should make a copy before doing this.  Also, it
   is the caller's responsibility to free the returned list.
 */
smb_ll *split_linesw(wchar_t *source);

#endif // SMB_STR_H
