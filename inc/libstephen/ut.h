/***************************************************************************//**

  @file         ut.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Unit Testing

  @copyright    Copyright (c) 2014, Stephen Brennan.
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

#ifndef LIBSTEPHEN_UT_H
#define LIBSTEPHEN_UT_H

/**
   @brief The size of a description text field (in characters) for an smbunit test.
 */
#define SMB_UNIT_DESCRIPTION_SIZE 20

/**
   @brief The max number of unit tests in a single test group.
 */
#define SMB_UNIT_TESTS_PER_GROUP 20

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

  /**
     @brief Should we check for memory leaks?
   */
  int check_mem_leaks;

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

smb_ut_test *su_create_test(char *description, int (*run)(), int check_mem_leaks);
smb_ut_group *su_create_test_group(char *description);
void su_add_test(smb_ut_group *group, smb_ut_test *test);
int su_run_test(smb_ut_test *test);
int su_run_group(smb_ut_group *group);
void su_delete_test(smb_ut_test *test);
void su_delete_group(smb_ut_group *group);

#endif // LIBSTEPHEN_UT_H
