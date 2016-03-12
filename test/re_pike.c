/***************************************************************************//**

  @file         pike.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 February 2016

  @brief        Pike VM tests.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "tests.h"

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

static int test_any(void)
{
  Regex r = recomp(".");

  TEST_ASSERT(execute(r, "a", NULL) == 1);
  TEST_ASSERT(execute(r, "b", NULL) == 1);
  TEST_ASSERT(execute(r, "c", NULL) == 1);
  TEST_ASSERT(execute(r, "(", NULL) == 1);
  TEST_ASSERT(execute(r, "*", NULL) == 1);
  TEST_ASSERT(execute(r, ".", NULL) == 1);
  TEST_ASSERT(execute(r, "", NULL) == -1);

  free_prog(r);
  return 0;
}

static int test_char(void)
{
  Regex r = recomp("a");

  TEST_ASSERT(execute(r, "a", NULL) == 1);
  TEST_ASSERT(execute(r, "b", NULL) == -1);
  TEST_ASSERT(execute(r, "c", NULL) == -1);

  free_prog(r);
  return 0;
}

static int test_range(void)
{
  Regex r = recomp("[a-c -]");

  TEST_ASSERT(execute(r, "a", NULL) == 1);
  TEST_ASSERT(execute(r, "b", NULL) == 1);
  TEST_ASSERT(execute(r, "c", NULL) == 1);
  TEST_ASSERT(execute(r, "d", NULL) == -1);
  TEST_ASSERT(execute(r, "e", NULL) == -1);
  TEST_ASSERT(execute(r, "A", NULL) == -1);
  TEST_ASSERT(execute(r, " ", NULL) == 1);
  TEST_ASSERT(execute(r, "-", NULL) == 1);

  free_prog(r);
  return 0;
}

static int test_nrange(void)
{
  Regex r = recomp("[^a-c -]");

  TEST_ASSERT(execute(r, "a", NULL) == -1);
  TEST_ASSERT(execute(r, "b", NULL) == -1);
  TEST_ASSERT(execute(r, "c", NULL) == -1);
  TEST_ASSERT(execute(r, "d", NULL) == 1);
  TEST_ASSERT(execute(r, "e", NULL) == 1);
  TEST_ASSERT(execute(r, "A", NULL) == 1);
  TEST_ASSERT(execute(r, " ", NULL) == -1);
  TEST_ASSERT(execute(r, "-", NULL) == -1);

  free_prog(r);
  return 0;
}

static int test_split_jump(void)
{
  Regex r = recomp("a*");

  TEST_ASSERT(execute(r, "", NULL) == 0);
  TEST_ASSERT(execute(r, "a", NULL) == 1);
  TEST_ASSERT(execute(r, "aa", NULL) == 2);
  TEST_ASSERT(execute(r, "aaa", NULL) == 3);
  TEST_ASSERT(execute(r, "aaaa", NULL) == 4);
  TEST_ASSERT(execute(r, "b", NULL) == 0);

  free_prog(r);
  return 0;
}

/*
  So, this one serves two purposes - in pike.c:addthread(), there is an if
  statement for detecting whether a thread has already been added to the list at
  this Instruction.  This requires a significant amount of non-determinism -
  i.e. two different ways to get to the same instruction.  The easiest way to
  get that was two greedy stars of the same character right next to each other -
  that's a lot of ways to get to each instruction.

  By having a test for this, (a) we get coverage for that if statement, and (b)
  we get to ensure that the memory management is handled correctly.  So make
  sure we're valgrinding these tests.
 */
static int test_nondeterminism_freeing(void)
{
  Regex r = recomp("a*a*");

  TEST_ASSERT(execute(r, "", NULL) == 0);
  TEST_ASSERT(execute(r, "a", NULL) == 1);
  TEST_ASSERT(execute(r, "aa", NULL) == 2);
  TEST_ASSERT(execute(r, "aaa", NULL) == 3);
  TEST_ASSERT(execute(r, "aaaa", NULL) == 4);
  TEST_ASSERT(execute(r, "b", NULL) == 0);

  free_prog(r);
  return 0;
}

static int test_save(void)
{
  size_t *capture;
  Regex r = recomp("(a*)b");

  TEST_ASSERT(numsaves(r) == 2);

  TEST_ASSERT(execute(r, "b", &capture) == 1);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 0);
  free(capture);
  TEST_ASSERT(execute(r, "ab", &capture) == 2);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 1);
  free(capture);
  TEST_ASSERT(execute(r, "aab", &capture) == 3);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 2);
  free(capture);
  TEST_ASSERT(execute(r, "aaab", &capture) == 4);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 3);
  free(capture);

  free_prog(r);
  return 0;
}

/* Make sure this gives no memory leaks. */
static int test_save_null(void)
{
  Regex r = recomp("(a*)b");

  TEST_ASSERT(numsaves(r) == 2);

  TEST_ASSERT(execute(r, "b", NULL) == 1);
  TEST_ASSERT(execute(r, "ab", NULL) == 2);
  TEST_ASSERT(execute(r, "aab", NULL) == 3);
  TEST_ASSERT(execute(r, "aaab", NULL) == 4);
  free_prog(r);
  return 0;
}

static int test_save_discard_stash(void)
{
  size_t *capture;
  Regex r = recomp("(a*)b+");

  TEST_ASSERT(numsaves(r) == 2);

  TEST_ASSERT(execute(r, "aabbb", &capture) == 5);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 2);
  free(capture);

  free_prog(r);
  return 0;
}

void pike_test(void)
{
  smb_ut_group *group = su_create_test_group("test/pike.c");

  smb_ut_test *any = su_create_test("any", test_any);
  su_add_test(group, any);

  smb_ut_test *char_ = su_create_test("char", test_char);
  su_add_test(group, char_);

  smb_ut_test *range = su_create_test("range", test_range);
  su_add_test(group, range);

  smb_ut_test *nrange = su_create_test("nrange", test_nrange);
  su_add_test(group, nrange);

  smb_ut_test *split_jump = su_create_test("split_jump", test_split_jump);
  su_add_test(group, split_jump);

  smb_ut_test *nondeterminism_freeing = su_create_test("nondeterminism_freeing", test_nondeterminism_freeing);
  su_add_test(group, nondeterminism_freeing);

  smb_ut_test *save = su_create_test("save", test_save);
  su_add_test(group, save);

  smb_ut_test *save_null = su_create_test("save_null", test_save_null);
  su_add_test(group, save_null);

  smb_ut_test *save_discard_stash = su_create_test("save_discard_stash", test_save_discard_stash);
  su_add_test(group, save_discard_stash);

  su_run_group(group);
  su_delete_group(group);
}
