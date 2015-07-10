/***************************************************************************//**

  @file         util.h

  @author       Stephen Brennan

  @date         Created Friday, 10 July 2015

  @brief        Utility functions (mostly string).

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_UTIL_H
#define SMB_UTIL_H

wchar_t *smb_read_linew(FILE *file, smb_status *status);
char *smb_read_line(FILE *file, smb_status *status);

#endif // SMB_UTIL_H
