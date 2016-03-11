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
  size_t n;
  instr *prog = recomp(".", &n);

  TEST_ASSERT(execute(prog, n, "a", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "b", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "c", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "(", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "*", NULL) == 1);
  TEST_ASSERT(execute(prog, n, ".", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "", NULL) == -1);

  free_prog(prog, n);
  return 0;
}

static int test_char(void)
{
  size_t n;
  instr *prog = recomp("a", &n);

  TEST_ASSERT(execute(prog, n, "a", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "b", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "c", NULL) == -1);

  free_prog(prog, n);
  return 0;
}

static int test_range(void)
{
  size_t n;
  instr *prog = recomp("[a-c -]", &n);

  TEST_ASSERT(execute(prog, n, "a", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "b", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "c", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "d", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "e", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "A", NULL) == -1);
  TEST_ASSERT(execute(prog, n, " ", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "-", NULL) == 1);

  free_prog(prog, n);
  return 0;
}

static int test_nrange(void)
{
  size_t n;
  instr *prog = recomp("[^a-c -]", &n);

  TEST_ASSERT(execute(prog, n, "a", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "b", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "c", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "d", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "e", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "A", NULL) == 1);
  TEST_ASSERT(execute(prog, n, " ", NULL) == -1);
  TEST_ASSERT(execute(prog, n, "-", NULL) == -1);

  free_prog(prog, n);
  return 0;
}

static int test_split_jump(void)
{
  size_t n;
  instr *prog = recomp("a*", &n);

  TEST_ASSERT(execute(prog, n, "", NULL) == 0);
  TEST_ASSERT(execute(prog, n, "a", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "aa", NULL) == 2);
  TEST_ASSERT(execute(prog, n, "aaa", NULL) == 3);
  TEST_ASSERT(execute(prog, n, "aaaa", NULL) == 4);
  TEST_ASSERT(execute(prog, n, "b", NULL) == 0);

  free_prog(prog, n);
  return 0;
}

/*
  So, this one serves two purposes - in pike.c:addthread(), there is an if
  statement for detecting whether a thread has already been added to the list at
  this instruction.  This requires a significant amount of non-determinism -
  i.e. two different ways to get to the same instruction.  The easiest way to
  get that was two greedy stars of the same character right next to each other -
  that's a lot of ways to get to each instruction.

  By having a test for this, (a) we get coverage for that if statement, and (b)
  we get to ensure that the memory management is handled correctly.  So make
  sure we're valgrinding these tests.
 */
static int test_nondeterminism_freeing(void)
{
  size_t n;
  instr *prog = recomp("a*a*", &n);

  TEST_ASSERT(execute(prog, n, "", NULL) == 0);
  TEST_ASSERT(execute(prog, n, "a", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "aa", NULL) == 2);
  TEST_ASSERT(execute(prog, n, "aaa", NULL) == 3);
  TEST_ASSERT(execute(prog, n, "aaaa", NULL) == 4);
  TEST_ASSERT(execute(prog, n, "b", NULL) == 0);

  free_prog(prog, n);
  return 0;
}

static int test_save(void)
{
  size_t n;
  size_t *capture;
  instr *prog = recomp("(a*)b", &n);

  TEST_ASSERT(numsaves(prog, n) == 2);

  TEST_ASSERT(execute(prog, n, "b", &capture) == 1);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 0);
  free(capture);
  TEST_ASSERT(execute(prog, n, "ab", &capture) == 2);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 1);
  free(capture);
  TEST_ASSERT(execute(prog, n, "aab", &capture) == 3);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 2);
  free(capture);
  TEST_ASSERT(execute(prog, n, "aaab", &capture) == 4);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 3);
  free(capture);

  free_prog(prog, n);
  return 0;
}

/* Make sure this gives no memory leaks. */
static int test_save_null(void)
{
  size_t n;
  instr *prog = recomp("(a*)b", &n);

  TEST_ASSERT(numsaves(prog, n) == 2);

  TEST_ASSERT(execute(prog, n, "b", NULL) == 1);
  TEST_ASSERT(execute(prog, n, "ab", NULL) == 2);
  TEST_ASSERT(execute(prog, n, "aab", NULL) == 3);
  TEST_ASSERT(execute(prog, n, "aaab", NULL) == 4);
  free_prog(prog, n);
  return 0;
}

static int test_save_discard_stash(void)
{
  size_t n;
  size_t *capture;
  instr *prog = recomp("(a*)b+", &n);

  TEST_ASSERT(numsaves(prog, n) == 2);

  TEST_ASSERT(execute(prog, n, "aabbb", &capture) == 5);
  TEST_ASSERT(capture[0] == 0);
  TEST_ASSERT(capture[1] == 2);
  free(capture);

  free_prog(prog, n);
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
