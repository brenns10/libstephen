/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Run tests on the libstephen library.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>

#include "libstephen/base.h"
#include "libstephen/log.h"
#include "tests.h"

/**
   Main test function
 */
int main(int argc, char ** argv)
{
  (void) argc; // unused
  (void) argv; // unused
  sl_set_level(NULL, LEVEL_INFO);
  linked_list_test();
  array_list_test();
  hash_table_test();
  bit_field_test();
  iter_test();
  list_test();
  args_test();
  charbuf_test();
  string_test();
  parse_test();
  lex_test();
  codegen_test();
  pike_test();
  log_test();
  // return args_test_main(argc, argv);
}
