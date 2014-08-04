/***************************************************************************//**

  @file         itertest.c

  @author       Stephen Brennan

  @date         Created Monday,  4 August 2014

  @brief        Libstephen: Iterator Tests

  @copyright    Copyright (c) 2014, Stephen Brennan.
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

#include "libstephen/ut.h"

#include "libstephen/list.h"
#include "libstephen/ll.h"
#include "libstephen/al.h"

// Setup and tear down declarations.

/**
   @brief Return and iterator to test.
   @param n Number of elements to return.

   The elements should be integers of 100 * index.
 */
smb_iter (*get_iter)(int n);
/**
   @brief Clean up the iterator's underlying list.
 */
void (*cleanup)(void);
void *test_data;


/*******************************************************************************

                                     Tests

*******************************************************************************/

/**
   @brief Tests that empty iterators have no values.
 */
int iter_test_empty()
{
  smb_iter it = get_iter(0);

  TEST_ASLINE(! it.has_next(&it));

  it.destroy(&it);
  cleanup();
  return 0;
}

/**
   @brief Tests that iterators can destroy.
 */
int iter_test_destroy()
{
  smb_iter it = get_iter(10);
  it.destroy(&it);
  cleanup();
  return 0;
}

/**
   @brief Tests that iterators can free.
 */
int iter_test_delete()
{
  smb_iter *it = smb_new(smb_iter, 1);
  *it = get_iter(10);
  it->delete(it);
  cleanup();
  return 0;
}

/**
   @brief Tests that iterators have the correct amounts of values.
 */
int iter_test_count()
{
  #define MAX_TEST_COUNT 1000
  for (int i = 0; i < MAX_TEST_COUNT; i++) {
    smb_iter it = get_iter(i);
    int n = 0;
    while (it.has_next(&it)) {
      it.next(&it);
      n++;
    }
    it.destroy(&it);
    cleanup();
    TEST_ASLINE(i == n);
  }
  return 0;
}

/**
   @brief Tests that iterators return correct values in the correct order.
 */
int iter_test_values()
{
  #define TEST_COUNT 1000
  smb_iter it = get_iter(TEST_COUNT);
  DATA d;
  int i = 0;
  while (it.has_next(&it)) {
    d = it.next(&it);
    TEST_ASLINE(d.data_llint == 100 * i);
    i++;
  }
  it.destroy(&it);
  cleanup();
  return 0;
}


/*******************************************************************************

                                Setup/Tear Down

*******************************************************************************/

/**
   @brief Return an iterator for an array list.
   @param size Number of elements to add.
   @return An iterator.
 */
smb_iter get_al_iter(int size)
{
  test_data = al_create();

  for (int i = 0; i < size; i++) {
    DATA d = { .data_llint = 100 * i };
    al_append(test_data, d);
  }

  return al_get_iter(test_data);
}

/**
   @brief Clean up array list.
 */
void al_cleanup(void)
{
  al_delete(test_data);
}

/**
   @brief Return a linked list iterator.
   @param Number of elements in the list.
   @return An iterator.
 */
smb_iter get_ll_iter(int size)
{
  test_data = ll_create();

  for (int i = 0; i < size; i++) {
    DATA d = { .data_llint = 100 * i };
    ll_append(test_data, d);
  }

  return ll_get_iter(test_data);
}

/**
   @brief Clean up linked list.
 */
void ll_cleanup(void)
{
  ll_delete(test_data);
}

/*******************************************************************************

                                  Test Running

*******************************************************************************/

/**
   @brief Run the tests in this file with a specified description.

   This allows you to set the setup and tear down functions to different values
   and run the tests again with a different name.

   @param desc The description for this run through.
 */
void run_tests(char *desc)
{
  smb_ut_group *group = su_create_test_group(desc);

  smb_ut_test *empty = su_create_test("empty", iter_test_empty, 0, 1);
  su_add_test(group, empty);

  smb_ut_test *destroy = su_create_test("destroy", iter_test_destroy, 0, 1);
  su_add_test(group, destroy);

  smb_ut_test *delete = su_create_test("delete", iter_test_delete, 0, 1);
  su_add_test(group, delete);

  smb_ut_test *count = su_create_test("count", iter_test_count, 0, 1);
  su_add_test(group, count);

  smb_ut_test *values = su_create_test("values", iter_test_values, 0, 1);
  su_add_test(group, values);

  su_run_group(group);
  su_delete_group(group);
}

/**
   @brief Run the tests on iterators.
 */
void iter_test(void)
{
  get_iter = &get_al_iter;
  cleanup = &al_cleanup;
  run_tests("al_iter");

  get_iter = &get_ll_iter;
  cleanup = &ll_cleanup;
  run_tests("ll_iter");
}
