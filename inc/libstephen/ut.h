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

/**
   @brief Create and return a new unit test.
   @param description A description of the test.
   @param run A function pointer to the test function.
   @returns A pointer to the new test.
 */
smb_ut_test *su_create_test(char *description, int (*run)());
/**
   @brief Create and return a new test group.
   @param description A short description for the group.
   @returns A pointer to the test group.
 */
smb_ut_group *su_create_test_group(char *description);
/**
   @brief Add a test to the given test group.

   A maximum of SMB_UNIT_TESTS_PER_GROUP may be added to the group.  After the
   limit is reached, this function fails *silently*, so as to prevent
   interference with the actual tests.
   @param group A pointer to the group to add the test to.
   @param test A pointer to the test.
 */
void su_add_test(smb_ut_group *group, smb_ut_test *test);
/**
   @brief Run the given test.

   This is pretty simple, it runs the test and returns the error code (which
   should be a line number).  If there was an error, it prints the code in the
   standard "file:lineno" way so that Emacs will jump right to the failed
   assertion.
   @param test The test to run
   @param file The filename of the test (used for the error message).
   @returns The line number of the failed assertion, or 0 if none occurred.
 */
int su_run_test(smb_ut_test *test, char *file);
/**
   @brief Run a group of tests.

   The tests are run sequentially (in the order they were added to the group).
   If a test fails, the remaining tests are not executed.
   @param group A pointer to the smb_ut_group to run.
   @returns An integer.  Since the tests are run sequentially via the
   su_run_test() function, it returns 0 if all tests succeeded, or else the
   return code of the failed test from su_run_test().
 */
int su_run_group(smb_ut_group *group);
/**
   @brief Frees the memory associated with the test, and performs cleanup.

   Note that no actual cleanup is required by the test, so the only benefit to
   using this function is that it is future-safe (updates to smbunit may require
   cleanup to be performed in this function).
   @param test The test to free
 */
void su_delete_test(smb_ut_test *test);
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
void su_delete_group(smb_ut_group *group);

#endif // LIBSTEPHEN_UT_H
