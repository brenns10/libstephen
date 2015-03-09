/***************************************************************************//**

  @file         listtest.c

  @author       Stephen Brennan

  @date         Friday,  6 March 2015

  @brief        Libstephen: Generic list tests.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <string.h>

#include "libstephen/base.h"
#include "libstephen/list.h"
#include "libstephen/ll.h"
#include "libstephen/al.h"
#include "libstephen/ut.h"
#include "tests.h"

/*******************************************************************************

                                   Test Setup

*******************************************************************************/

smb_list (*get_list)(void);

/*******************************************************************************

                                     Tests

*******************************************************************************/

int test_append()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_list list = get_list();

  for (d.data_llint = 0; d.data_llint < 200; d.data_llint++) {
    // Put a small, 200 item load on it.  This tests appending on
    // empty and general appending.
    list.append(&list, d);
    TEST_ASSERT(list.length(&list) == d.data_llint + 1);

    // Test that the data is correct.
    for (int i = 0; i < list.length(&list); i++) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  list.delete(&list);
  return 0;
}

int test_prepend()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_list list = get_list();

  // Test prepend about 200 times...
  for (d.data_llint = 0; d.data_llint < 200; d.data_llint++) {
    list.prepend(&list, d);
    TEST_ASSERT(list.length(&list) == d.data_llint + 1);

    for (int i = 0; i < list.length(&list); i++) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == d.data_llint - i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  list.delete(&list);
  return 0;
}

int test_set()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_list list = get_list();
  const int length = 30;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    list.append(&list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(list.get(&list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is correct
  TEST_ASSERT(list.length(&list) == length);

  // Test set
  for (int i = 0; i < list.length(&list); i++) {
    d.data_llint = length - i;
    list.set(&list, i, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
    TEST_ASSERT(list.get(&list, i, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is still correct
  TEST_ASSERT(list.length(&list) == length);

  list.set(&list, list.length(&list), d, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);

  status = SMB_SUCCESS;
  list.get(&list, list.length(&list), &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);

  list.delete(&list);
  return 0;
}

int test_remove()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_list list = get_list();
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    list.append(&list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(list.get(&list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Remove first element
  list.remove(&list, 0, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(list.length(&list) == length - 1);
  TEST_ASSERT(list.get(&list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Remove middle element
  list.remove(&list, 10, &status); // list[10] == 11 before
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(list.length(&list) == length - 2);
  TEST_ASSERT(list.get(&list, 10, &status).data_llint == 12);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Remove last element
  list.remove(&list, list.length(&list) - 1, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(list.length(&list) == length - 3);

  // Remove invalid element
  list.remove(&list, list.length(&list), &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);
  status = SMB_SUCCESS;
  list.remove(&list, -1, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);
  status = SMB_SUCCESS;

  // Test all elements values
  int value = 1;
  for (int i = 0; i < length - 3; i++) {
    if (i == 10) value++;
    TEST_ASSERT(list.get(&list, i, &status).data_llint == value);
    TEST_ASSERT(status == SMB_SUCCESS);
    value++;
  }

  list.delete(&list);
  return 0;
}

int test_insert()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_list list = get_list();
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    list.append(&list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(list.get(&list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Here are the insertions for the test:
  d.data_llint = 100;
  list.insert(&list, 0, d);
  TEST_ASSERT(list.length(&list) == length + 1);

  d.data_llint = 101;
  list.insert(&list, 10, d);
  TEST_ASSERT(list.length(&list) == length + 2);

  d.data_llint = 102;
  list.insert(&list, list.length(&list), d);
  TEST_ASSERT(list.length(&list) == length + 3);


  d.data_llint = 101;
  list.insert(&list, -1, d);
  TEST_ASSERT(list.length(&list) == length + 4);

  d.data_llint = 102;
  list.insert(&list, list.length(&list) + 1, d);
  TEST_ASSERT(list.length(&list) == length + 5);

  int value = 0;

  for (int i = 0; i < list.length(&list); i++) {
    if (i == 0) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == 101);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == 1) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == 100);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == 11) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == 101);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == list.length(&list) - 2) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == 102);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == list.length(&list) - 1) {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == 102);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else {
      TEST_ASSERT(list.get(&list, i, &status).data_llint == value);
      TEST_ASSERT(status == SMB_SUCCESS);
      value++;
    }
  }

  list.delete(&list);
  return 0;
}

int test_front(void)
{
  smb_list list = get_list();
  smb_status status = SMB_SUCCESS;
  DATA d;
  const int length = 5;

  // Push test data to get 0, 1, 2, 3, 4
  for (d.data_llint = length-1; d.data_llint >= 0; d.data_llint--) {
    list.push_front(&list, d);
  }

  // Test that peek works correctly with data.
  TEST_ASSERT(list.peek_front(&list, &status).data_llint == 0);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Check that pop works correctly.
  for (d.data_llint = 0; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(list.pop_front(&list, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Check that peek and pop will fail correctly.
  list.peek_front(&list, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);
  status = SMB_SUCCESS;
  list.pop_front(&list, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);

  // Cleanup
  list.delete(&list);
  return 0;
}

int test_back(void)
{
  smb_list list = get_list();
  smb_status status = SMB_SUCCESS;
  DATA d;
  const int length = 5;

  // Push test data to get 0, 1, 2, 3, 4
  for (d.data_llint = 0; d.data_llint < length; d.data_llint++) {
    list.push_back(&list, d);
  }

  // Test that peek works correctly with data.
  TEST_ASSERT(list.peek_back(&list, &status).data_llint == 4);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Check that pop works correctly.
  for (d.data_llint = length-1; d.data_llint >= 0; d.data_llint--) {
    TEST_ASSERT(list.pop_back(&list, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Check that peek and pop will fail correctly.
  list.peek_back(&list, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);
  status = SMB_SUCCESS;
  list.pop_back(&list, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);

  // Cleanup
  list.delete(&list);
  return 0;
}

int test_index_of(void)
{
  smb_list list = get_list();
  smb_status status = SMB_SUCCESS;
  DATA d, d2;
  char *t1 = "I'm a string";
  char *t2;
  int strsize = strlen(t1) + 1;
  const int length = 20;

  // Make a copy of the test string so pointer comparison won't work.
  t2 = (char*)smb_new(char, strsize);
  strncpy(t2, t1, strsize);
  d.data_ptr = t1;

  // Check that the string won't be found in an empty list.
  TEST_ASSERT(list.index_of(&list, d, &data_compare_string) == -1);

  // Now add the copy to the list.
  d2.data_ptr = t2;
  list.append(&list, d2);

  // Now assert that the string will be found in the list.
  TEST_ASSERT(list.index_of(&list, d, &data_compare_string) == 0);

  smb_free(t2);
  list.pop_back(&list, &status);

  // Push test data to get 0, 1, 2, 3, 4, ..., 20
  for (d.data_llint = 0; d.data_llint < length; d.data_llint++) {
    list.push_back(&list, d);
  }

  // Check that it finds the data.
  for (d.data_llint = 0; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(list.index_of(&list, d, NULL) == d.data_llint);
  }

  list.delete(&list);
  return 0;
}

/*******************************************************************************

                                  Test Runner

*******************************************************************************/

/**
   @brief Run the tests in this file with a specified description.

   This allows you to set the setup and tear down functions to different values
   and run the tests again with a different name.

   @param desc The description for this run through.
 */
void run_list_tests(char *desc)
{
  smb_ut_group *group = su_create_test_group(desc);

  smb_ut_test *append = su_create_test("append", test_append);
  su_add_test(group, append);

  smb_ut_test *prepend = su_create_test("prepend", test_prepend);
  su_add_test(group, prepend);

  smb_ut_test *set = su_create_test("set", test_set);
  su_add_test(group, set);

  smb_ut_test *remove = su_create_test("remove", test_remove);
  su_add_test(group, remove);

  smb_ut_test *insert = su_create_test("insert", test_insert);
  su_add_test(group, insert);

  smb_ut_test *front = su_create_test("front", test_front);
  su_add_test(group, front);

  smb_ut_test *back = su_create_test("back", test_back);
  su_add_test(group, back);

  smb_ut_test *index_of = su_create_test("index_of", test_index_of);
  su_add_test(group, index_of);

  su_run_group(group);
  su_delete_group(group);
}

/**
   @brief Run the tests on lists.
 */
void list_test(void)
{
  get_list = &ll_create_list;
  run_list_tests("list_ll");

  get_list = &al_create_list;
  run_list_tests("list_al");
}
