/***************************************************************************//**

  @file         smbunit.c

  @author       Stephen Brennan

  @date         Created Saturday, 28 September 2013

  @brief        A simple, lightweight unit test runner based on function
                pointers.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
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

#include <stdio.h>            /* printf */
#include <string.h>           /* strncpy */

#include "libstephen/ut.h"    /* functions we're defining */

/**
   @brief Create and return a new unit test.

   @param description A description of the test.
   @param run A function pointer to the test function.
   @param check_mem_leaks Whether to check if the mallocs before = mallocs
   after.  0 for no, 1 for yes.
   @returns A pointer to the new test.
 */
smb_ut_test *su_create_test(char *description, int (*run)(), int check_mem_leaks)
{
  smb_ut_test *test = smb_new(smb_ut_test, 1);
  strncpy(test->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  test->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;

  test->run = run;
  test->check_mem_leaks = check_mem_leaks;

  return test;
}

/**
   @brief Create and return a new test group.

   @param description A short description for the group.
   @returns A pointer to the test group.
 */
smb_ut_group *su_create_test_group(char *description)
{
  smb_ut_group *group = smb_new(smb_ut_group, 1);
  strncpy(group->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  group->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;

  group->num_tests = 0;
  return group;
}

/**
   @brief Add a test to the given test group.

   A maximum of SMB_UNIT_TESTS_PER_GROUP may be added to the group.  After the
   limit is reached, this function fails *silently*, so as to prevent
   interference with the actual tests.

   @param group A pointer to the group to add the test to.
   @param test A pointer to the test.
 */
void su_add_test(smb_ut_group *group, smb_ut_test *test)
{
  if (group->num_tests < SMB_UNIT_TESTS_PER_GROUP) {
    group->tests[group->num_tests++] = test;
  }
}

/**
   @brief Run the given test.

   Tracks memory allocations and thrown errors.  In order to do this, all errors
   are cleared before execution of the test.

   @param test The test to run
   @returns A code based on the execution of the test (and whether or not the
   feature is enabled for the test):
   @retval 0 Test passed all its conditions.
   @retval 1 Test returned a non-zero return code.  This will be reported on
   stdout, along with the specific return code.  The reason for a non-zero
   return code is usually a failed assertion, in which case the code corresponds
   to the assertion number.
   @retval 3 Memory was leaked.  The test returned 0 and all expected errors
   were found (or no errors were expected or found), but memory leaked.
 */
int su_run_test(smb_ut_test *test)
{
  int result = test->run();

  if (result) {
    printf ("TEST \"%s\" failed with code: %d\n",test->description, result);
    return 1;
  }

  printf ("TEST \"%s\" passed!\n",test->description);
  return 0;
}

/**
   @brief Run a group of tests.

   The tests are run sequentially (in the order they were added to the group).
   If a test fails, the remaining tests are not executed.

   @param group A pointer to the smb_ut_group to run.

   @returns An integer.  Since the tests are run sequentially via the
   su_run_test() function, it returns 0 if all tests succeeded, or else the
   return code of the failed test from su_run_test().
 */
int su_run_group(smb_ut_group *group)
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

/**
   @brief Frees the memory associated with the test, and performs cleanup.

   Note that no actual cleanup is required by the test, so the only benefit to
   using this function is that it is future-safe (updates to smbunit may require
   cleanup to be performed in this function).

   @param test The test to free
 */
void su_delete_test(smb_ut_test *test)
{
  smb_free(test);
}

/**
   @brief Free the memory associated with the group AND ALL TESTS WITHIN IT.
   You MUST use this to delete test groups.

   Note that if a pointer to a smb_ut_test within the smb_ut_group
   is already invalid (freed), then su_delete_group() assumes that it has been
   freed and moves on.  So you may include a single test in more than one group
   and safely delete them both (but after deleting the first group, the test
   will no longer be valid and a segmentation fault will occur if you try to run
   the second group).

   @param group A pointer to the group to free
 */
void su_delete_group(smb_ut_group *group)
{
  for (int i = 0; i < group->num_tests; i++) {
    if (group->tests[i]) // don't delete if already deleted
      su_delete_test(group->tests[i]);
  }
  smb_free(group);
}
