/***************************************************************************//**

  @file         smbunit.c

  @author       Stephen Brennan

  @date         Created Saturday, 28 September 2013

  @brief        A simple, lightweight unit test runner based on function
                pointers.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>            /* printf */
#include <string.h>           /* strncpy */

#include "libstephen/base.h"
#include "libstephen/ut.h"    /* functions we're defining */

smb_ut_test *su_create_test(char *description, int (*run)())
{
  smb_ut_test *test = smb_new(smb_ut_test, 1);
  strncpy(test->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  test->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;
  test->run = run;
  return test;
}

smb_ut_group *su_create_test_group(char *description)
{
  smb_ut_group *group = smb_new(smb_ut_group, 1);
  strncpy(group->description, description, SMB_UNIT_DESCRIPTION_SIZE - 1);
  group->description[SMB_UNIT_DESCRIPTION_SIZE - 1] = 0;

  group->num_tests = 0;
  return group;
}

void su_add_test(smb_ut_group *group, smb_ut_test *test)
{
  if (group->num_tests < SMB_UNIT_TESTS_PER_GROUP) {
    group->tests[group->num_tests++] = test;
  }
}

int su_run_test(smb_ut_test *test, char *file)
{
  int result = test->run();

  if (result) {
    printf ("%s:%d: assertion failed in %s\n", file, result, test->description);
    return 1;
  }

  printf ("TEST \"%s\" passed!\n",test->description);
  return 0;
}

int su_run_group(smb_ut_group *group)
{
  int result = 0;
  printf ("## GROUP \"%s\" running...\n",group->description);
  for (int i = 0; i < group->num_tests; i++) {
    result = su_run_test(group->tests[i], group->description);
    if (result) {
      printf ("## GROUP \"%s\" failed on test: %d\n\n", group->description, i);
      exit(result);
    }
  }
  printf ("## GROUP \"%s\" passed!\n\n", group->description);
  return 0;
}

void su_delete_test(smb_ut_test *test)
{
  smb_free(test);
}

void su_delete_group(smb_ut_group *group)
{
  for (int i = 0; i < group->num_tests; i++) {
    if (group->tests[i]) // don't delete if already deleted
      su_delete_test(group->tests[i]);
  }
  smb_free(group);
}
