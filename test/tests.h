/***************************************************************************//**

  @file         tests.h

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Header for all the tests.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

/**
   Run the linked list tests
 */
void linked_list_test();

/**
   Run the array list tests
 */
void array_list_test();

/**
   Run the hash table tests
 */
void hash_table_test();

/**
   Run the bit field tests
 */
void bit_field_test();

/**
   Run the iter tests
 */
void iter_test(void);

/**
   Run the list tests
 */
void list_test(void);

/**
   Run the args tests
 */
void args_test(void);

/**
   Run the cbuf test.
 */
void charbuf_test(void);

/**
   Run the log test (this one is visual, no unit tests).
 */
void log_test(void);

/**
   Run the string test.
 */
void string_test(void);

/**
   Tests from cky.
 */
void fsm_test(void);
void fsm_io_test(void);
void regex_test(void);
void regex_search_test(void);


/**
   Output statistics of main function args.
 */
int args_test_main(int argc, char **argv);
