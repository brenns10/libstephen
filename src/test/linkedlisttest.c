/*******************************************************************************

  File:         linkedlisttest.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  Test of the linked list

*******************************************************************************/

#include <stdio.h>
#include "tests.h"
#include "../libstephen.h"

// These tests use the smbunit framework!

int ll_test_create()
{
  DATA d;
  d.data_llint = 13;
  
  struct smb_ll *list = ll_create();
  ll_append(list, d);

  // Assert that it was allocated correctly
  //TEST_ASSERT(!CHECK(ALLOCATION_ERROR), 1);
  
  TEST_ASSERT(ll_length(list) == 1, 2);

  TEST_ASSERT(ll_get(list, 0).data_llint == 13, 3);

  ll_delete(list);
  return 0;
}

int ll_test_create_empty()
{
  struct smb_ll *list = ll_create();
  
  // Assert that allocation went OK
  //TEST_ASSERT(!CHECK(ALLOCATION_ERROR), 1);

  TEST_ASSERT(ll_length(list) == 0, 2);

  ll_delete(list);
  return 0;
}

int ll_test_append()
{
  DATA d;
  d.data_llint = 0;

  int current_assertion = 1;

  struct smb_ll *list = ll_create();

  for ( ; d.data_llint < 200; d.data_llint++) {
    // Put a small, 200 item load on it.  This tests appending on
    // empty and general appending.
    ll_append(list, d);

    TEST_ASSERT(ll_length(list) == d.data_llint + 1, current_assertion);
    current_assertion++;

    // Test that the data is correct.
    for (int i = 0; i < ll_length(list); i++) {
      TEST_ASSERT(ll_get(list, i).data_llint == i, current_assertion);
      current_assertion++;
    }
  }

  ll_delete(list);
  return 0;
}

int ll_test_prepend()
{
  DATA d;
  d.data_llint = 0;
  int current_assertion = 1;
  struct smb_ll *list = ll_create();

  // Test prepend about 200 times...
  for ( ; d.data_llint < 200; d.data_llint++) {
    ll_prepend(list, d);
    
    TEST_ASSERT(ll_length(list) == d.data_llint + 1, current_assertion);
    current_assertion++;

    for (int i = 0; i < ll_length(list); i++) {
      TEST_ASSERT(ll_get(list, i).data_llint == d.data_llint - i, current_assertion);
      current_assertion++;
    }
  }
  
  ll_delete(list);
  return 0;
}

int ll_test_set()
{
  DATA d;
  struct smb_ll *list = ll_create();
  const int length = 30;
  int current_assertion = 1;
  
  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint).data_llint == d.data_llint, current_assertion);
    current_assertion++;
  }

  // Test that the length is correct
  TEST_ASSERT(ll_length(list) == length, current_assertion);
  current_assertion++;
  
  // Test set
  for (int i = 0; i < ll_length(list); i++) {
    d.data_llint = length - i;
    ll_set(list, i, d);
    TEST_ASSERT(ll_get(list, i).data_llint == d.data_llint, current_assertion);
    current_assertion++;
  }

  // Test that the length is still correct
  TEST_ASSERT(ll_length(list) == length, current_assertion);
  current_assertion++;

  ll_delete(list);
  return 0;
}

int ll_test_remove()
{
  DATA d;
  d.data_llint = 0;
  struct smb_ll *list = ll_create();
  int current_assertion = 1;
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint).data_llint == d.data_llint, current_assertion);
    current_assertion++;
  }

  // Remove first element
  ll_remove(list, 0);
  TEST_ASSERT(ll_length(list) == length - 1, current_assertion);
  current_assertion++;
  TEST_ASSERT(ll_get(list, 0).data_llint == 1, current_assertion);
  current_assertion++;

  // Remove middle element
  ll_remove(list, 10); // list[10] == 11 before
  TEST_ASSERT(ll_length(list) == length - 2, current_assertion);
  current_assertion++;
  TEST_ASSERT(ll_get(list, 10).data_llint == 12, current_assertion);
  current_assertion++;
  
  // Remove last element
  ll_remove(list, ll_length(list) - 1);
  TEST_ASSERT(ll_length(list) == length - 3, current_assertion);
  current_assertion++;

  // Test all elements values
  int value = 1;
  for (int i = 0; i < length - 3; i++) {
    if (i == 10) value++;
    TEST_ASSERT(ll_get(list, i).data_llint == value, current_assertion);
    current_assertion++;
    value++;
  }

  ll_delete(list);
  return 0;
}

int ll_test_insert()
{
  DATA d;
  d.data_llint = 0;
  struct smb_ll *list = ll_create();
  int current_assertion = 1;
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint).data_llint == d.data_llint, current_assertion);
    current_assertion++;
  }

  // Here are the three insertions for the test:
  d.data_llint = 100;
  ll_insert(list, 0, d);
  TEST_ASSERT(ll_length(list) == length + 1, current_assertion);
  current_assertion++;

  d.data_llint = 101;
  ll_insert(list, 10, d);
  TEST_ASSERT(ll_length(list) == length + 2, current_assertion);
  current_assertion++;
  
  d.data_llint = 102;
  ll_insert(list, ll_length(list), d);
  TEST_ASSERT(ll_length(list) == length + 3, current_assertion);
  current_assertion++;
  
  int value = 0;
  
  for (int i = 0; i < ll_length(list); i++) {
    if (i == 0) {
      TEST_ASSERT(ll_get(list, i).data_llint == 100, current_assertion);
    } else if (i == 10) {
      TEST_ASSERT(ll_get(list, i).data_llint == 101, current_assertion);
    } else if (i == ll_length(list) - 1) {
      TEST_ASSERT(ll_get(list, i).data_llint == 102, current_assertion);
    } else {
      TEST_ASSERT(ll_get(list, i).data_llint == value, current_assertion);
      value++;
    }
    current_assertion++;
  }

  ll_delete(list);
  return 0;
}

void linked_list_test()
{
  // Use the smbunit test framework.  Load tests and run them.
  smb_ut_group *group = su_create_test_group("linked list");

  smb_ut_test *create = su_create_test("create", ll_test_create, 0, 1);
  su_add_test(group, create);

  smb_ut_test *create_empty = su_create_test("create_empty", ll_test_create_empty, 0, 1);
  su_add_test(group, create_empty);

  smb_ut_test *append = su_create_test("append", ll_test_append, 0, 1);
  su_add_test(group, append);

  smb_ut_test *prepend = su_create_test("prepend", ll_test_prepend, 0, 1);
  su_add_test(group, prepend);

  smb_ut_test *set = su_create_test("set", ll_test_set, 0, 1);
  su_add_test(group, set);

  smb_ut_test *remove = su_create_test("remove", ll_test_remove, 0, 1);
  su_add_test(group, remove);
  
  smb_ut_test *insert = su_create_test("insert", ll_test_insert, 0, 1);
  su_add_test(group, insert);

  su_run_group(group);
  su_delete_group(group);
}
