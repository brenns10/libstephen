/***************************************************************************//**

  @file         charbuf.c

  @author       Stephen Brennan

  @date         Saturday, 23 May 2015

  @brief        Character buffer data structure, for simpler string handling.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdarg.h>
#include <wchar.h>
#include <string.h>

#include "libstephen/base.h"
#include "libstephen/cb.h"

/*******************************************************************************

                                 cbuf Functions

*******************************************************************************/

void cb_init(cbuf *obj, int capacity)
{
  // Initialization logic
  obj->buf = smb_new(char, capacity);
  obj->buf[0] = '\0';
  obj->capacity = capacity;
  obj->length = 0;
}

cbuf *cb_create(int capacity)
{
  cbuf *obj = smb_new(cbuf, 1);
  cb_init(obj, capacity);
  return obj;
}

void cb_destroy(cbuf *obj)
{
  smb_free(obj->buf);
  obj->buf = NULL;
}

void cb_delete(cbuf *obj) {
  cb_destroy(obj);
  smb_free(obj);
}

static void cb_expand_to_fit(cbuf *obj, int minsize)
{
  int newcapacity = obj->capacity;
  while (newcapacity < minsize) {
    newcapacity *= 2;
  }
  if (newcapacity != obj->capacity) {
    obj->buf = smb_renew(char, obj->buf, newcapacity);
    obj->capacity = newcapacity;
  }
}

void cb_append(cbuf *obj, char *buf)
{
  int length = strlen(buf);
  cb_expand_to_fit(obj, obj->length + length + 1);
  strcpy(obj->buf + obj->length, buf);
  obj->length += length;
}

void cb_vprintf(cbuf *obj, char *format, va_list va)
{
  va_list v2;
  int length;
  va_copy(v2, va);

  // Find the length of the formatted string.
  length = vsnprintf(NULL, 0, format, va);

  // Make sure we have enough room for everything.
  cb_expand_to_fit(obj, obj->length + length + 1);

  // Put the formatted string into the buffer.
  vsnprintf(obj->buf + obj->length, length + 1, format, v2);
  va_end(v2);
}

void cb_printf(cbuf *obj, char *format, ...)
{
  va_list va;
  va_start(va, format);
  cb_vprintf(obj, format, va);
  va_end(va);  // Have to va_stop() it when you're done using it.
}

/*******************************************************************************

                                wcbuf Functions

*******************************************************************************/

void wcb_init(wcbuf *obj, int capacity)
{
  // Initialization logic
  obj->buf = smb_new(wchar_t, capacity);
  obj->buf[0] = L'\0';
  obj->capacity = capacity;
  obj->length = 0;
}

wcbuf *wcb_create(int capacity)
{
  wcbuf *obj = smb_new(wcbuf, 1);
  wcb_init(obj, capacity);
  return obj;
}

void wcb_destroy(wcbuf *obj)
{
  // Cleanup logic
  smb_free(obj->buf);
  obj->buf = NULL;
}

void wcb_delete(wcbuf *obj)
{
  wcb_destroy(obj);
  smb_free(obj);
}

static void wcb_expand_to_fit(wcbuf *obj, int minsize)
{
  int newcapacity = obj->capacity;
  while (newcapacity < minsize) {
    newcapacity *= 2;
  }
  if (newcapacity != obj->capacity) {
    obj->buf = smb_renew(wchar_t, obj->buf, newcapacity);
    obj->capacity = newcapacity;
  }
}

void wcb_append(wcbuf *obj, wchar_t *str)
{
  int length = wcslen(str);
  wcb_expand_to_fit(obj, obj->length + length + 1);
  wcscpy(obj->buf + obj->length, str);
  obj->length += length;
}

void wcb_vprintf(wcbuf *obj, wchar_t *format, va_list v1)
{
  va_list v2;
  char *mbformat, *mbout;
  size_t mbformat_len, mbout_len, wcout_len;

  // First, convert the wide format string to a multibyte one.
  mbformat_len = wcstombs(NULL, format, 0);
  mbformat = smb_new(char, mbformat_len + 1);
  wcstombs(mbformat, format, mbformat_len+1);

  // Then, use vsnprintf first to find out how many bytes of output to allocate.
  va_copy(v2, v1);
  mbout_len = vsnprintf(NULL, 0, mbformat, v1);
 
  // Now, actually allocate the memory and do the multibyte print.
  mbout = smb_new(char, mbout_len + 1);
  vsnprintf(mbout, mbout_len + 1, mbformat, v2);
  va_end(v2);

  // And now, get the number of wide characters this is.
  wcout_len = mbstowcs(NULL, mbout, 0);

  // Make sure we have room for everything.
  wcb_expand_to_fit(obj, obj->length + wcout_len + 1);

  // And, put the output in the buffer.
  mbstowcs(obj->buf + obj->length, mbout, wcout_len + 1);
  obj->length += wcout_len;

  // Free up allocated memory, and we're good to go.
  smb_free(mbformat);
  smb_free(mbout);
}

void wcb_printf(wcbuf *obj, wchar_t *format, ...)
{
  va_list va;
  va_start(va, format);
  wcb_vprintf(obj, format, va);
  va_end(va);
}
