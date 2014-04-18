/**

    @file    common_include.c

    @author  Stephen Brennan

    @date    Created: September 1, 2013

    @brief   Contains definitions for the general purpose items in libstephen.
*/

#include "libstephen.h"

static size_t mallocs;
unsigned int ERROR_VAR;

void smb___helper_inc_malloc_counter(size_t number_of_mallocs)
{
  mallocs += number_of_mallocs;
}

size_t smb___helper_get_malloc_counter()
{
  return mallocs;
  return 0;
}

void smb___helper_dec_malloc_counter(size_t number_of_frees)
{
  mallocs -= number_of_frees;
}
