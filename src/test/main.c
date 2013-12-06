/*******************************************************************************

  File:         main.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  Runs tests on the library

*******************************************************************************/

#include <stdio.h>
#include "../libstephen.h"
#include "tests.h"

/**
   Main test function
 */
int main(int argc, char ** argv)
{

  int mallocs;

  linked_list_test();
  array_list_test();
  hash_table_test();

  if (mallocs = SMB_GET_MALLOC_COUNTER) {
    printf("#### MEMORY LEAK DETECTED!!  MALLOCS: %d. ####\n", mallocs);
    return 1;
  } else {
    return 0;
  }

  // return args_test_main(argc, argv);
}
