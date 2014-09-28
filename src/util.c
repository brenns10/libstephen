/***************************************************************************//**

  @file         common_include.c

  @author       Stephen Brennan

  @date         Created Sunday, 1 September 2013

  @brief        Contains definitions for the general purpose items.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <sys/time.h>         /* gettimeofday */
#include <stdio.h>            /* fprintf, fopen */
#include <stdlib.h>           /* malloc, exit */
#include <wchar.h>            /* wchar_t */
#include <stdbool.h>          /* bool */
#include <string.h>           /* strcmp */

#include "libstephen/base.h"  /* SMB_* */

static size_t mallocs;
unsigned int ERROR_VAR;
char *output_file = NULL;
struct timeval start = {.tv_sec = 0, .tv_usec = 0};

void smb_set_memory_log_location(char *location)
{
  output_file = location;
}

void smb___write_current_memory()
{
  struct timeval tv;
  double secs;
  FILE *out;

  if (output_file == NULL) return;

  out = fopen(output_file, "a");
  gettimeofday(&tv, NULL);

  if (start.tv_sec == 0 && start.tv_usec == 0) {
    start = tv;
  }

  secs = tv.tv_sec - start.tv_sec + (tv.tv_usec - start.tv_usec)/1000000.0;

  fprintf(out, "%f, %u\n", secs, mallocs);
  fclose(out);
}

void smb___helper_inc_malloc_counter(size_t number_of_mallocs)
{
  mallocs += number_of_mallocs;
  smb___write_current_memory();
}

size_t smb___helper_get_malloc_counter()
{
  return mallocs;
  return 0;
}

void smb___helper_dec_malloc_counter(size_t number_of_frees)
{
  mallocs -= number_of_frees;
  smb___write_current_memory();
}

/**
   @brief Utility function for macro smb_new().  Wrapper over malloc().

   Allocate a certain amount of memory.  If allocation fails, EXIT with an error
   message.  Account for the allocated bytes via the
   SMB_INCREMENT_MALLOC_COUNTER() macro.

   @parm amt The number of bytes to allocate.
   @returns The pointer to the allocated memory (guaranteed).
 */
void *smb___new(size_t amt)
{
  void *result = malloc(amt);
  if (!result) {
    fprintf(stderr, "smb_new: allocation error\n");
    exit(1);
  }
  SMB_INCREMENT_MALLOC_COUNTER(amt);
  return result;
}

/**
   @brief Utility function for macro smb_renew().  Wrapper over realloc().

   Reallocate a certain amount of memory.  You need to know the old size of the
   memory to keep proper accounting of the memory.

   @param ptr The memory to reallocate.
   @param newsize The new size of the memory.
   @param oldsize The old size of the memory.
   @returns The pointer to the new block.
 */
void *smb___renew(void *ptr, size_t newsize, size_t oldsize)
{
  void *result = realloc(ptr, newsize);
  if (!result) {
    fprintf(stderr, "smb_renew: allocation error\n");
    exit(1);
  }
  SMB_INCREMENT_MALLOC_COUNTER(newsize-oldsize);
  return result;
}

/**
   @brief Utility function for macro smb_free().  Wrapper over free().

   Free a certain amount of memory, and account for the memory in the allocation
   counter.

   @param ptr Memory to free.
   @param oldsize The size of the memory.
 */
void *smb___free(void *ptr, size_t oldsize)
{
  free(ptr);
  SMB_DECREMENT_MALLOC_COUNTER(oldsize);
}

/**
   @brief Read a line of input from the given file.

   Read a line of input into a dynamically allocated buffer.  The buffer is
   allocated by this function.  It will contain the line, WITHOUT newline or EOF
   characters.  The size of the buffer will not be the same size as the string.

   @param file The stream to read from, typically stdin.
   @param[out] alloc The number of characters allocated to the buffer.
   @returns A buffer containing the line.
 */
wchar_t *smb_read_linew(FILE *file, int *alloc)
{
  #define SMBRL_BUFSIZE 256
  int bufsize = SMBRL_BUFSIZE;
  int position = 0;
  wchar_t *buffer = smb_new(wchar_t, bufsize);
  wint_t wc;

  while (true) {
    // Read a character
    wc = fgetwc(file);

    // If we hit EOF, replace it with a null character and return.
    if (wc == WEOF || wc == L'\n') {
      buffer[position++] = L'\0';
      // Set the out parameter to the number of characters allocated.
      if (alloc)
        *alloc = bufsize;
      return buffer;
    } else {
      buffer[position++] = wc;
    }

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      buffer = smb_renew(wchar_t, buffer, bufsize+SMBRL_BUFSIZE, bufsize);
      bufsize += SMBRL_BUFSIZE;
    }
  }
}


char *smb_read_line(FILE *file, int *alloc)
{
  int bufsize = SMBRL_BUFSIZE;
  int position = 0;
  char *buffer = smb_new(char, bufsize);
  int c;

  while (true) {
    // Read a character
    c = fgetc(file);

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position++] = '\0';
      // Set the out parameter to the number of characters allocated.
      if (alloc)
        *alloc = bufsize;
      return buffer;
    } else {
      buffer[position++] = c;
    }

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      buffer = smb_renew(char, buffer, bufsize+SMBRL_BUFSIZE, bufsize);
      bufsize += SMBRL_BUFSIZE;
    }
  }
}

/**
   @brief Print a DATA, treating it as a `char *`.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_string(FILE *f, DATA d)
{
  fprintf(f, "\"%s\"", (char *)d.data_ptr);
}

/**
   @brief Print a DATA, treating it as an int.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_int(FILE *f, DATA d)
{
  fprintf(f, "%Ld", d.data_llint);
}

/**
   @brief Print a DATA, treating it as a float.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_float(FILE *f, DATA d)
{
  fprintf(f, "%Lf", d.data_dbl);
}

/**
   @brief Print a DATA, treating it as a pointer.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_pointer(FILE *f, DATA d)
{
  fprintf(f, "%p", d.data_ptr);
}

/**
   @brief Test whether two null terminated strings are equal.
   @param d1 First data.
   @param d2 Second data.
   @return An integer indicating whether the strings are equal, less than, or
   greater than.
 */
int data_compare_string(DATA d1, DATA d2)
{
  char *s1, *s2;
  s1 = (char *)d1.data_ptr;
  s2 = (char *)d2.data_ptr;
  return strcmp(s1, s2);
}

/**
   @brief Test whether two ints are equal.
   @param d1 First int.
   @param d2 Second int.
   @return An integer indicating whether the ints are equal, less than, or
   greater than.
 */
int data_compare_int(DATA d1, DATA d2)
{
  // Since the difference between two long long ints could be more than an int,
  // we need to store the difference and conditionally return.
  long long int diff = d1.data_llint - d2.data_llint;
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
   @brief Test whether two doubles are equal.

   This function compares two doubles stored in DATA.  However, it's NOT a smart
   comparison.  It really just tests whether the binary representations
   themselves are equal.  If you want a smart method for comparing floating
   point numbers, look elsewhere!

   @param d1 First double.
   @param d2 Second double.
   @return An integer indicating whether the doubles are equal, less than, or
   greater than.
 */
int data_compare_float(DATA d1, DATA d2)
{
  double diff = d1.data_llint - d2.data_llint;
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
   @brief Test whether two pointers are equal.  Does not order them.
   @param d1 First pointer.
   @param d2 Second pointer.
   @return 0 if equal, 1 if not.
 */
int data_compare_pointer(DATA d1, DATA d2)
{
  if (d1.data_ptr == d2.data_ptr) {
    return 0;
  } else {
    return 1;
  }
}
