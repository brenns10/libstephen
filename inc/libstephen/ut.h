/***************************************************************************//**

  @file         libstephen/ut.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Unit Testing

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_UT_H
#define LIBSTEPHEN_UT_H

#include <stdio.h>
#include <string.h>
#include <wchar.h>

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
#define TEST_ASSERT(expr)\
  do {\
    if(!(expr)) {\
      fprintf(stderr, "Assertion: " #expr " failed.\n"); \
      return __LINE__;\
    }\
  } while (0);

#define _ASSERTION_BASE_(X, Y, SPEC, TYPE, INV) \
  do {\
    if ((X) INV (Y)) {\
      fprintf(stderr, "Assertion failed: " SPEC " " #INV " " SPEC "\n",\
              (TYPE) (X), (TYPE) (Y));\
      return __LINE__;\
    }\
  }\
  while (0);

#define TA_PTR_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, !=)
#define TA_PTR_NE(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, ==)
#define TA_PTR_LT(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, >=)
#define TA_PTR_GT(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, <=)
#define TA_PTR_LE(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, >)
#define TA_PTR_GE(X, Y) _ASSERTION_BASE_(X, Y, "%p", void*, <)

#define TA_SIZE_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, !=)
#define TA_SIZE_NE(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, ==)
#define TA_SIZE_LT(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, >=)
#define TA_SIZE_GT(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, <=)
#define TA_SIZE_LE(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, >)
#define TA_SIZE_GE(X, Y) _ASSERTION_BASE_(X, Y, "%zu", size_t, <)

#define TA_INT_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, !=)
#define TA_INT_NE(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, ==)
#define TA_INT_LT(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, >=)
#define TA_INT_GT(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, <=)
#define TA_INT_LE(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, >)
#define TA_INT_GE(X, Y) _ASSERTION_BASE_(X, Y, "%d", int, <)

#define TA_LLINT_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, !=)
#define TA_LLINT_NE(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, ==)
#define TA_LLINT_LT(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, >=)
#define TA_LLINT_GT(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, <=)
#define TA_LLINT_LE(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, >)
#define TA_LLINT_GE(X, Y) _ASSERTION_BASE_(X, Y, "%lld", long long, <)

#define TA_UINT_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, !=)
#define TA_UINT_NE(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, ==)
#define TA_UINT_LT(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, >=)
#define TA_UINT_GT(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, <=)
#define TA_UINT_LE(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, >)
#define TA_UINT_GE(X, Y) _ASSERTION_BASE_(X, Y, "%u", unsigned, <)

#define TA_LLUINT_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, !=)
#define TA_LLUINT_NE(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, ==)
#define TA_LLUINT_LT(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, >=)
#define TA_LLUINT_GT(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, <=)
#define TA_LLUINT_LE(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, >)
#define TA_LLUINT_GE(X, Y) _ASSERTION_BASE_(X, Y, "%llu", long long unsigned, <)

#define TA_FLT_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, !=)
#define TA_FLT_NE(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, ==)
#define TA_FLT_LT(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, >=)
#define TA_FLT_GT(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, <=)
#define TA_FLT_LE(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, >)
#define TA_FLT_GE(X, Y) _ASSERTION_BASE_(X, Y, "%f", float, <)

#define TA_CHAR_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, !=)
#define TA_CHAR_NE(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, ==)
#define TA_CHAR_LT(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, >=)
#define TA_CHAR_GT(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, <=)
#define TA_CHAR_LE(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, >)
#define TA_CHAR_GE(X, Y) _ASSERTION_BASE_(X, Y, "%c", char, <)

#define TA_WCHAR_EQ(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, !=)
#define TA_WCHAR_NE(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, ==)
#define TA_WCHAR_LT(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, >=)
#define TA_WCHAR_GT(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, <=)
#define TA_WCHAR_LE(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, >)
#define TA_WCHAR_GE(X, Y) _ASSERTION_BASE_(X, Y, "%lc", wchar_t, <)

#define TA_STR_EQ(x, y)\
  do {\
    if (strcmp((x), (y)) != 0) {\
      fprintf(stderr, "Assertion failed: \"%s\" != \"%s\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);
#define TA_STR_NE(x, y)\
  do {\
    if (strcmp((x), (y) == 0) {\
      fprintf(stderr, "Assertion failed:  \"%s\" != \"%s\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);

#define TA_STRN_EQ(x, y, n)\
  do {\
    if (strncmp((x), (y), (n)) != 0) {\
      fprintf(stderr, "Assertion failed: \"%s\" != \"%s\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);
#define TA_STRN_NE(x, y, n)\
  do {\
    if (strcmp((x), (y), (n)) == 0) {\
      fprintf(stderr, "Assertion failed:  \"%s\" == \"%s\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);

#define TA_WSTR_EQ(x, y)\
  do {\
    if (wcscmp((x), (y)) != 0) {\
      fprintf(stderr, "Assertion failed: \"%ls\" != \"%ls\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);
#define TA_WSTR_NE(x, y)\
  do {\
    if (wcscmp((x), (y)) == 0) {\
      fprintf(stderr, "Assertion failed: \"%ls\" != \"%ls\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);

#define TA_WSTRN_EQ(x, y, n)\
  do {\
    if (wcsncmp((x), (y), (n)) != 0) {\
      fprintf(stderr, "Assertion failed: \"%ls\" != \"%ls\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);
#define TA_WSTRN_NE(x, y, n)\
  do {\
    if (wcsncmp((x), (y), (n)) == 0) {\
      fprintf(stderr, "Assertion failed:  \"%ls\" == \"%ls\"\n", x, y);\
      return __LINE__;\
    }\
  } while (0);

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
