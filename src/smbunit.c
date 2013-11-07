/*******************************************************************************

  File:         ~/repos/libstephen/smbunit.c

  Author:       Stephen Brennan

  Date Created: Saturday, 28 September 2013

  Description: A simple, lightweight unit test runner based on function
  pointers.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "libstephen.h"

TEST *su_create_test(char * description, int (*run)(), int expected_errors, int check_mem_leaks)
{
  TEST *test = (TEST*) malloc(sizeof(TEST));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(TEST));
  strncpy(test->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  test->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;
  
  test->run = run;
  test->expected_errors = expected_errors;
  test->check_mem_leaks = check_mem_leaks;
  
  return test;
}

TEST_GROUP *su_create_test_group(char * description)
{
  TEST_GROUP *group = (TEST_GROUP*) malloc(sizeof(TEST_GROUP));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(TEST_GROUP));
  strncpy(group->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  group->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;

  group->num_tests = 0;
  return group;
}

void su_add_test(TEST_GROUP *group, TEST *test)
{
  if (group->num_tests < SMB_UNIT_TESTS_PER_GROUP) {
    group->tests[group->num_tests++] = test;
  }
}

int su_run_test(TEST *test)
{
  CLEAR_ALL_ERRORS; // just in case
  int mallocs = SMB_GET_MALLOC_COUNTER;
  int result = test->run();
  mallocs = SMB_GET_MALLOC_COUNTER - mallocs;

  if (result) {
    printf ("TEST \"%s\" failed with code: %d\n",test->description, result);
    return 1;
  }

  if (test->expected_errors && !CHECK(test->expected_errors)) {
    printf ("TEST \"%s\" did not raise: %X\n",test->description, test->expected_errors);
    return 2;
  }

  if (test->check_mem_leaks && mallocs) {
    printf ("TEST \"%s\" LEAKED %d BYTES!\n", test->description, mallocs);
    return 3;
  }

  printf ("TEST \"%s\" passed!\n",test->description);
  return 0;
}

int su_run_group(TEST_GROUP *group)
{
  int result = 0;
  printf ("## GROUP \"%s\" running...\n",group->description);
  for (int i = 0; i < group->num_tests; i++) {
    result = su_run_test(group->tests[i]);
    if (result) {
      printf ("## GROUP \"%s\" failed on test: %d\n\n", group->description, i);
      return result;
    }
  }
  printf ("## GROUP \"%s\" passed!\n\n", group->description);
  return 0;
}

void su_delete_test(TEST *test)
{
  free(test);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(TEST));
}

void su_delete_group(TEST_GROUP *group)
{
  for (int i = 0; i < group->num_tests; i++) {
    if (group->tests[i]) // don't delete if already deleted
      su_delete_test(group->tests[i]);
  }
  free(group);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(TEST_GROUP));
}
