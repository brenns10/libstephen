/*******************************************************************************

  File:         ~/repos/libstephen/test/arraylisttest.c

  Author:       Stephen Brennan

  Date Created: Saturday, 28 September 2013

  Description:  Test of the array list data structure.

*******************************************************************************/

#include "../libstephen.h"
#include "tests.h"

////////////////////////////////////////////////////////////////////////////////
// TESTS

int al_test_create()
{
  DATA d;
  d.data_llint = 13;

  ARRAY_LIST *list = al_create();
  al_append(list, d);
  
  // Assert that it was allocated correctly.
  TEST_ASSERT(!CHECK(ALLOCATION_ERROR), 1);

  TEST_ASSERT(al_length(list) == 1, 2);

  TEST_ASSERT(al_get(list, 0).data_llint == 13, 3);
  
  al_delete(list);
  return 0;
}

int al_test_create_empty()
{
  DATA d;
  d.data_llint = 13;

  ARRAY_LIST *list = al_create();

  // Assert that it was allocated correctly.
  TEST_ASSERT(!CHECK(ALLOCATION_ERROR), 1);
  
  TEST_ASSERT(al_length(list) == 0, 2);

  al_delete(list);
  return 0;
}

int al_test_append()
{
  DATA d;
  d.data_llint = 0;

  ARRAY_LIST *list = al_create();
  
  // Test append about 21 times to check that reallocation works successfully
  for ( ; d.data_llint < 22; d.data_llint++) {
    al_append(list, d);

    // Assert that the length of the list is correct
    TEST_ASSERT(al_length(list) == d.data_llint + 1, d.data_llint + 1);
    
    // Assert that each element in the list is correct
    for (int i = 0; i < al_length(list); i++) {
      TEST_ASSERT(al_get(list, i).data_llint == i, d.data_llint + 1);
    }
  }

  al_delete(list);
  return 0;
}

int al_test_prepend()
{
  DATA d;
  d.data_llint = 0;

  ARRAY_LIST *list = al_create();
  
  // Test prepend about 21 times to check that reallocation works successfully
  for ( ; d.data_llint < 22; d.data_llint++) {
    al_prepend(list, d);

    // Assert that the length of the list is correct
    TEST_ASSERT(al_length(list) == d.data_llint + 1, d.data_llint + 1);
    
    // Assert that each element in the list is correct
    for (int i = 0; i < al_length(list); i++) {
      TEST_ASSERT(al_get(list, i).data_llint == d.data_llint - i, d.data_llint + 1);
    }
  }

  al_delete(list);
  return 0;
}

int al_test_set()
{
  DATA d;
  ARRAY_LIST *list = al_create();
  const int length = 30;

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d);
  }

  // Verify data
  int assertionNum = 1;
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == i, assertionNum);
    assertionNum++; // separate line because the macro would place it within an
		    // if statement
  }

  // Test that the length is correct
  TEST_ASSERT(al_length(list) == length, assertionNum);
  assertionNum++;

  // Test set
  for (int i = 0; i < al_length(list); i++) {
    d.data_llint = al_length(list) - i;
    al_set(list, i , d);
    TEST_ASSERT(al_get(list, i).data_llint == d.data_llint, assertionNum);
    assertionNum++;
  }

  // Test that the length is still correct
  TEST_ASSERT(al_length(list) == length, assertionNum);
  assertionNum++;

  al_delete(list);
  return 0;
}

int al_test_remove()
{
  DATA d;
  ARRAY_LIST *list = al_create();
  const int length = 23;

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d);
  }

  // Verify data
  int assertionNum = 1;
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == i, assertionNum);
    assertionNum++; // separate line because the macro would place it within an
		    // if statement
  }

  // Remove first
  al_remove(list, 0);
  TEST_ASSERT(al_get(list, 0).data_llint == 1, assertionNum);
  assertionNum++;
  TEST_ASSERT(al_length(list) == length - 1, assertionNum);
  assertionNum++;

  // Remove last
  al_remove(list, al_length(list) - 1);
  TEST_ASSERT(al_get(list, al_length(list) - 1).data_llint == length - 2, assertionNum);
  assertionNum++;
  TEST_ASSERT(al_length(list) == length - 2, assertionNum);  
  assertionNum++;
  // Current list: 1 2 3 4 5 6 7 8 ... 
  
  // Remove from middle
  al_remove(list, 2);
  // Current list: 1 2 4 5 6 7 8 ...
  TEST_ASSERT(al_length(list) == length - 3, assertionNum);
  assertionNum++;

  // Test all the elements to make sure the data is correct
  int values[] = {1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  for (int i = 0; i < length - 3; i++) {
    TEST_ASSERT(al_get(list, i).data_llint == values[i], assertionNum);
    assertionNum++;
  }

  // Remove the remaining elements, and check that nothing bad happens
  for (int i = length - 4; i >= 0; i--) {
    al_remove(list, 0);
    TEST_ASSERT(al_length(list) == i, assertionNum);
    assertionNum++;
  }

  al_delete(list);
  return 0;
}

int al_test_insert()
{
  DATA d;
  ARRAY_LIST *list = al_create();
  const int length = 20;

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d);
  }

  // Verify data
  int assertionNum = 1;
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == i, assertionNum);
    assertionNum++; // separate line because the macro would place it within an
		    // if statement
  }

  // Test insert when a realloc needs to occur
  d.data_llint = 100;
  al_insert(list, 10, d);
  int values_one[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12, 13, 14, 
		      15, 16, 17, 18, 19};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == values_one[i], assertionNum);
    assertionNum++;
  }

  // Test insert at end
  d.data_llint++;
  al_insert(list, al_length(list), d);
  int values_two[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12, 13, 14, 
		      15, 16, 17, 18, 19, 101};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == values_two[i], assertionNum);
    assertionNum++;
  }

  // Test insert at beginning
  d.data_llint++;
  al_insert(list, al_length(list), d);
  int values_three[] = {102, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12, 
			13, 14, 15, 16, 17, 18, 19, 101};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i).data_llint == values_three[i], assertionNum);
    assertionNum++;
  }

  al_delete(list);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST LOADER AND RUNNER

void array_list_test()
{
  struct smb_ut_group *group = su_create_test_group("array list");

  struct smb_ut_test *create = su_create_test("create", al_test_create, 0, 1);
  su_add_test(group, create);
  
  struct smb_ut_test *create_empty = su_create_test("create_empty", al_test_create_empty, 0, 1);
  su_add_test(group, create_empty);

  struct smb_ut_test *append = su_create_test("append", al_test_append, 0, 1);
  su_add_test(group, append);

  struct smb_ut_test *prepend = su_create_test("prepend", al_test_prepend, 0, 1);
  su_add_test(group, prepend);
  
  struct smb_ut_test *set = su_create_test("set", al_test_set, 0, 1);
  su_add_test(group, set);
  
  struct smb_ut_test *remove = su_create_test("remove", al_test_remove, 0, 1);
  su_add_test(group, remove);

  struct smb_ut_test *insert = su_create_test("insert", al_test_remove, 0, 1);
  su_add_test(group, insert);

  // The other elementary operations on the ARRAY_LIST, i.e. the push, pop and
  // peek functions, are all merely wrappers over the functions already tested.
  // Additionally, the delete and length functions are adequately tested by the
  // tests as they are.  So this is a fairly complete set of tests.

  su_run_group(group);

  su_delete_group(group);
}
