/***************************************************************************//**

  @file         listtest.c

  @author       Stephen Brennan

  @date         Friday,  6 March 2015

  @brief        Libstephen: Generic list tests.

*******************************************************************************/

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
