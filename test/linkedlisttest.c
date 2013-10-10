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
  
  LINKED_LIST *list = ll_create(d);

  // Assert that it was allocated correctly
  //TEST_ASSERT(!CHECK(ALLOCATION_ERROR), 1);
  
  TEST_ASSERT(ll_length(list) == 1, 2);

  TEST_ASSERT(ll_get(list, 0).data_llint == 13, 3);

  ll_delete(list);
  return 0;
}

int ll_test_create_empty()
{
  LINKED_LIST *list = ll_create_empty();
  
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

  LINKED_LIST *list = ll_create_empty();

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
  LINKED_LIST *list = ll_create_empty();

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
  LINKED_LIST *list = ll_create_empty();
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

void linked_list_test()
{
  // Use the smbunit test framework.  Load tests and run them.
  TEST_GROUP *group = su_create_test_group("linked list");

  TEST *create = su_create_test("create", ll_test_create, 0, 1);
  su_add_test(group, create);

  TEST *create_empty = su_create_test("create_empty", ll_test_create_empty, 0, 1);
  su_add_test(group, create_empty);

  TEST *append = su_create_test("append", ll_test_append, 0, 1);
  su_add_test(group, append);

  TEST *prepend = su_create_test("prepend", ll_test_prepend, 0, 1);
  su_add_test(group, prepend);

  TEST *set = su_create_test("set", ll_test_set, 0, 1);
  su_add_test(group, set);

  

  su_run_group(group);
  su_delete_group(group);
}
