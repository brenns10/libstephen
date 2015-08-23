/***************************************************************************//**

  @file         libstephen/util.h

  @author       Stephen Brennan

  @date         Created Friday, 10 July 2015

  @brief        Utility functions (mostly string).

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_UTIL_H
#define SMB_UTIL_H

/**
   @brief Read a line of wide character input from the given file.

   Read a line of input into a dynamically allocated buffer.  The buffer is
   allocated by this function.  It will contain the line, WITHOUT newline or EOF
   characters.  The size of the buffer will not be the same size as the string.
   @param file The stream to read from, typically stdin.
   @param[out] status Status variable.
   @returns A buffer containing the line.
 */
wchar_t *smb_read_linew(FILE *file, smb_status *status);
/**
   @brief Read a line of character input from the given file.

   Read a line of input into a dynamically allocated buffer.  The buffer is
   allocated by this function.  It will contain the line, WITHOUT newline or EOF
   characters.  The size of the buffer will not be the same size as the string.
   @param file The stream to read from, typically stdin.
   @param[out] status Status variable.
   @returns A buffer containing the line.
 */
char *smb_read_line(FILE *file, smb_status *status);

#endif // SMB_UTIL_H
