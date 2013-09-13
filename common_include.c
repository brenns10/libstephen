/*******************************************************************************

    File:         common_include.c
    Auhtor:       Stephen Brennan
    Date Created: 9/1/2013
    Description:  Contains macros and useful things for general purpose
                  programming.

*******************************************************************************/
#include "common_include.h"

static size_t mallocs;

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
