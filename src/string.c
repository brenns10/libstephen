/***************************************************************************//**

  @file         string.c

  @author       Stephen Brennan

  @date         Created Friday, 10 July 2015

  @brief        Libstephen string utilities!

  @copyright    Copyright (c) 2015-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <wchar.h>

#include "libstephen/ll.h"
#include "libstephen/cb.h"

char *read_file(FILE *f)
{
  cbuf cb;
  int c;
  cb_init(&cb, 1024);

  while ((c = fgetc(f)) != EOF) {
    cb_append(&cb, c);
  }

  cb_trim(&cb);
  return cb.buf;
}

wchar_t *read_filew(FILE *f)
{
  wcbuf wcb;
  wint_t wc;
  wcb_init(&wcb, 1024);

  while ((wc = fgetwc(f)) != WEOF) {
    wcb_append(&wcb, wc);
  }

  wcb_trim(&wcb);
  return wcb.buf;
}

char *read_line(FILE *file)
{
  cbuf cb;
  int c;
  cb_init(&cb, 256);

  while ((c = fgetc(file)) != EOF && c != '\n') {
    cb_append(&cb, c);
  }

  cb_trim(&cb);
  return cb.buf;
}

wchar_t *read_linew(FILE *file)
{
  wcbuf wcb;
  wint_t wc;
  wcb_init(&wcb, 256);

  while ((wc = fgetwc(file)) != WEOF && wc != L'\n') {
    wcb_append(&wcb, wc);
  }

  wcb_trim(&wcb);
  return wcb.buf;
}

smb_ll *split_lines(char *source)
{
  char *start;
  smb_ll *list;
  DATA d;

  /*
    We go through `source` looking for every newline, replace it with NUL, and
    add the beginnig of the line to the list.
   */
  start = source;
  list = ll_create();
  while (*source != '\0') {
    if (*source == '\n') {
      // Add line to list.
      d.data_ptr = start;
      ll_append(list, d);
      // Null-terminate the line.
      *source = '\0';
      // Next string starts at the next character.
      start = source + 1;
    }
    source++;
  }
  if (start != source) {
    d.data_ptr = start;
    ll_append(list, d);
  }
  return list;
}

smb_ll *split_linesw(wchar_t *source)
{
  wchar_t *start;
  smb_ll *list;
  DATA d;

  /*
    We go through `source` looking for every newline, replace it with NUL, and
    add the beginnig of the line to the list.
   */
  start = source;
  list = ll_create();
  while (*source != L'\0') {
    if (*source == L'\n') {
      // Add line to list.
      d.data_ptr = start;
      ll_append(list, d);
      // Null-terminate the line.
      *source = L'\0';
      // Next string starts at the next character.
      start = source + 1;
    }
    source++;
  }
  if (start != source) {
    d.data_ptr = start;
    ll_append(list, d);
  }
  return list;
}
