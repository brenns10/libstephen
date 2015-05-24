/***************************************************************************//**

  @file         ut.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Unit Testing

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_UT_H
#define LIBSTEPHEN_UT_H

/**
   @brief The size of a description text field (in characters) for an smbunit test.
 */
#define SMB_UNIT_DESCRIPTION_SIZE 50

/**
   @brief The max number of unit tests in a single test group.
 */
#define SMB_UNIT_TESTS_PER_GROUP 50

/**
   @brief Asserts that an expression is true.  If false, returns line number.

   Note that this is a macro, so using some things (++ and -- operators
   especially) can have unintended results.  As a general rule, do not put any
   complicated code inside the TEST_ASSERT() statement at all.

   # PARAMETERS #

   - expr: The expression to check.  If the expression evaluates to true (that
     is, not 0), the assertion passes.  If the expression evaluates to false
     (that is, zero), the assertion fails.

 */
#define TEST_ASSERT(expr) if(!(expr)) return __LINE__

/**
   @brief Defines a single unit test.

   Members should be modified with care, preferably not at all, except by using
   the smbunit functions.

 */
typedef struct smb_ut_test
{
  /**
     @brief A 20 character null-terminated string that identifies this
     particular test.
   */
  char description[SMB_UNIT_DESCRIPTION_SIZE];

  /**
     @brief Function pointer to the test to run.  The function should return 0
     if the test is successful.
   */
  int (*run)();

  /**
     @brief The error value of any expected errors from this test.
   */
  int expected_errors;

} smb_ut_test;

/**
   @brief A structure holding a group of unit tests that are all related.

   Members shouldn't me modified by client code.  All should be managed by the
   functions in smbunit.
 */
typedef struct smb_ut_group
{
  /**
     @brief A short description (length defined by SMB_UNIT_DESCRIPTION_SIZE)
     for the test.
   */
  char description[SMB_UNIT_DESCRIPTION_SIZE];

  /**
     @brief The number of tests in the group.
  */
  int num_tests;

  /**
     @brief Pointers to the tests contained.  Max amount of tests is SMB_UNIT_TESTS_PER_GROUP.
   */
  smb_ut_test *tests[SMB_UNIT_TESTS_PER_GROUP];

} smb_ut_group;

smb_ut_test *su_create_test(char *description, int (*run)());
smb_ut_group *su_create_test_group(char *description);
void su_add_test(smb_ut_group *group, smb_ut_test *test);
int su_run_test(smb_ut_test *test);
int su_run_group(smb_ut_group *group);
void su_delete_test(smb_ut_test *test);
void su_delete_group(smb_ut_group *group);

#endif // LIBSTEPHEN_UT_H
