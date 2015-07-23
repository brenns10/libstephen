/***************************************************************************//**

  @file         fsm.c

  @author       Stephen Brennan

  @date         Sunday, 18 January 2015

  @brief        Test fsm functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdbool.h>

#include "libstephen/ut.h"
#include "libstephen/fsm.h"


/**
   @brief Test for memory leaks.

   This will always pass, but Valgrind may catch memory leaks if errors exist.
   This is why you should always run tests with Valgrind!!
 */
int fsm_test_memory(void)
{
  fsm_trans fsm_trans_stack;
  fsm_trans *fsm_trans_heap;
  fsm fsm_stack;
  fsm *fsm_heap;

  fsm_trans_init(&fsm_trans_stack, 3, FSM_TRANS_POSITIVE, 12);
  fsm_trans_heap = fsm_trans_create(3, FSM_TRANS_POSITIVE, 0);
  fsm_trans_destroy(&fsm_trans_stack);

  fsm_init(&fsm_stack);
  fsm_heap = fsm_create();
  fsm_destroy(&fsm_stack, true);

  fsm_add_state(fsm_heap, true);
  fsm_add_trans(fsm_heap, 0, fsm_trans_heap);
  fsm_delete(fsm_heap, true);
  return 0;
}

/**
   @brief Test fsm_sim for memory leaks (use Valgrind!).
 */
static int test_fsm_sim_memory(void)
{
  fsm f;
  fsm_sim *pfs;
  smb_al *state;

  // Dummy resources for the FSM simulation.
  state = al_create();
  fsm_init(&f);

  // Test is to make sure that these work properly.
  pfs = fsm_sim_create(&f, state);
  fsm_sim_delete(pfs, true);

  // Only need to destroy the FSM, since the "curr" state was freed.
  fsm_destroy(&f, true);
  return 0;
}

/**
   @brief Test fsm_trans checking.

   Makes sure that testing transitions works correctly.
 */
int fsm_test_check(void)
{
  fsm_trans *single_positive = fsm_trans_create(1, FSM_TRANS_POSITIVE, 10);
  fsm_trans *multiple_positive = fsm_trans_create(2, FSM_TRANS_POSITIVE, 10);
  fsm_trans *single_negative = fsm_trans_create(1, FSM_TRANS_NEGATIVE, 10);
  fsm_trans *multiple_negative = fsm_trans_create(2, FSM_TRANS_NEGATIVE, 10);

  single_positive->start[0] = L'b';
  multiple_positive->start[0] = L'b';
  single_negative->start[0] = L'b';
  multiple_negative->start[0] = L'b';

  single_positive->end[0] = L'b';
  multiple_positive->end[0] = L'b';
  single_negative->end[0] = L'b';
  multiple_negative->end[0] = L'b';

  multiple_positive->start[1] = L'c';
  multiple_negative->start[1] = L'c';

  multiple_positive->end[1] = L'c';
  multiple_negative->end[1] = L'c';

  TEST_ASSERT(!fsm_trans_check(single_positive, L'a'));
  TEST_ASSERT(!fsm_trans_check(multiple_positive, L'a'));
  TEST_ASSERT(fsm_trans_check(single_negative, L'a'));
  TEST_ASSERT(fsm_trans_check(multiple_negative, L'a'));

  TEST_ASSERT(fsm_trans_check(single_positive, L'b'));
  TEST_ASSERT(fsm_trans_check(multiple_positive, L'b'));
  TEST_ASSERT(!fsm_trans_check(single_negative, L'b'));
  TEST_ASSERT(!fsm_trans_check(multiple_negative, L'b'));

  TEST_ASSERT(!fsm_trans_check(single_positive, L'c'));
  TEST_ASSERT(fsm_trans_check(multiple_positive, L'c'));
  TEST_ASSERT(fsm_trans_check(single_negative, L'c'));
  TEST_ASSERT(!fsm_trans_check(multiple_negative, L'c'));

  TEST_ASSERT(!fsm_trans_check(single_positive, L'd'));
  TEST_ASSERT(!fsm_trans_check(multiple_positive, L'd'));
  TEST_ASSERT(fsm_trans_check(single_negative, L'd'));
  TEST_ASSERT(fsm_trans_check(multiple_negative, L'd'));

  fsm_trans_delete(single_positive);
  fsm_trans_delete(multiple_positive);
  fsm_trans_delete(single_negative);
  fsm_trans_delete(multiple_negative);
  return 0;
}

/**
   @brief Test the shortcut methods for creating "single" FSM transitions.
 */
int test_shortcut(void)
{
  fsm_trans *a = fsm_trans_create_single(L'a', L'b', FSM_TRANS_POSITIVE, 10);
  fsm_trans b;
  fsm_trans_init_single(&b, L'a', L'b', FSM_TRANS_NEGATIVE, 10);

  TEST_ASSERT(fsm_trans_check(a, L'a'));
  TEST_ASSERT(!fsm_trans_check(&b, L'a'));
  TEST_ASSERT(fsm_trans_check(a, L'b'));
  TEST_ASSERT(!fsm_trans_check(&b, L'b'));
  TEST_ASSERT(!fsm_trans_check(a, L'c'));
  TEST_ASSERT(fsm_trans_check(&b, L'c'));

  fsm_trans_delete(a);
  fsm_trans_destroy(&b);
  return 0;
}

/**
   @brief Test the copy method works.  Valgrind will help here.
 */
int test_trans_copy(void)
{
  fsm_trans *a = fsm_trans_create_single(L'a', L'b', FSM_TRANS_POSITIVE, 10);
  fsm_trans *c = fsm_trans_copy(a);
  TEST_ASSERT(fsm_trans_check(c, L'a'));
  TEST_ASSERT(fsm_trans_check(c, L'b'));
  TEST_ASSERT(!fsm_trans_check(c, L'c'));
  fsm_trans_delete(a);
  fsm_trans_delete(c);
  return 0;
}

/**
   @brief Test that simple deterministic FSMs work.
 */
int test_simple_machine(void)
{
  fsm *f = fsm_create();
  int start = fsm_add_state(f, false);
  int end = fsm_add_state(f, true);
  fsm_trans *t = fsm_trans_create_single(L'a', L'a', FSM_TRANS_POSITIVE, end);

  f->start = start;
  fsm_add_trans(f, start, t);
  TEST_ASSERT(fsm_sim_det(f, L"a"));
  TEST_ASSERT(!fsm_sim_det(f, L"fail"));
  TEST_ASSERT(!fsm_sim_det(f, L""));
  fsm_delete(f, true);
  return 0;
}

/**
   @brief Test of the fsm_create_single_char() function.
 */
int test_create_single_char(void)
{
  fsm *f = fsm_create_single_char(L'a');
  TEST_ASSERT(fsm_sim_det(f, L"a"));
  TEST_ASSERT(!fsm_sim_det(f, L"fail"));
  TEST_ASSERT(!fsm_sim_det(f, L""));
  TEST_ASSERT(!fsm_sim_det(f, L"b"));
  fsm_delete(f, true);
  return 0;
}

/**
   @brief Test of the fsm_copy() function.

   When I copy a FSM, the old one should be unmodified, and the new one should
   accept and reject the same things.  After deleting the original, the new one
   should continue to work properly.
 */
static int test_copy(void)
{
  fsm *orig = fsm_create();
  fsm *copy;
  int start;
  int next;
  int end;

  // Create a FSM to accept "ab".
  start = fsm_add_state(orig, false);
  next = fsm_add_state(orig, false);
  end = fsm_add_state(orig, true);
  fsm_add_single(orig, start, next, L'a', L'a', FSM_TRANS_POSITIVE);
  fsm_add_single(orig, next, end, L'b', L'b', FSM_TRANS_POSITIVE);
  orig->start = start;

  // Ensure this works correctly.
  TEST_ASSERT(fsm_sim_det(orig, L"ab"));
  TEST_ASSERT(!fsm_sim_det(orig, L"ba"));

  // Create a copy of the original.
  copy = fsm_copy(orig);

  // The original should still work
  TEST_ASSERT(fsm_sim_det(orig, L"ab"));
  TEST_ASSERT(!fsm_sim_det(orig, L"ba"));

  // The copy should also work.
  TEST_ASSERT(fsm_sim_det(copy, L"ab"));
  TEST_ASSERT(!fsm_sim_det(copy, L"ba"));

  // After freeing the original, the new one should (still) work.
  fsm_delete(orig, true);
  TEST_ASSERT(fsm_sim_det(copy, L"ab"));
  TEST_ASSERT(!fsm_sim_det(copy, L"ba"));

  // Cleanup the copy and exit.
  fsm_delete(copy, true);
  return 0;
}

/**
   @brief Test the fsm_concat() function.
 */
static int test_concat(void)
{
  fsm *src = fsm_create();
  int s00 = fsm_add_state(src, false);
  int s01 = fsm_add_state(src, false);
  int s02 = fsm_add_state(src, false);
  int s03 = fsm_add_state(src, true);
  fsm *dst = fsm_create();
  int s10 = fsm_add_state(dst, false);
  int s11 = fsm_add_state(dst, false);
  int s12 = fsm_add_state(dst, false);
  int s13 = fsm_add_state(dst, true);

  // src accepts "bar"
  src->start = s00;
  fsm_add_single(src, s00, s01, L'b', L'b', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s01, s02, L'a', L'a', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s02, s03, L'r', L'r', FSM_TRANS_POSITIVE);

  // dst accepts "foo"
  dst->start = s10;
  fsm_add_single(dst, s10, s11, L'f', L'f', FSM_TRANS_POSITIVE);
  fsm_add_single(dst, s11, s12, L'o', L'o', FSM_TRANS_POSITIVE);
  fsm_add_single(dst, s12, s13, L'o', L'o', FSM_TRANS_POSITIVE);

  // They should work as advertised.
  TEST_ASSERT(fsm_sim_det(src, L"bar"));
  TEST_ASSERT(!fsm_sim_det(src, L"foo"));
  TEST_ASSERT(!fsm_sim_det(dst, L"bar"));
  TEST_ASSERT(fsm_sim_det(dst, L"foo"));

  // concatenate them, and you have foobar!
  fsm_concat(dst, src);
  fsm_delete(src, true);

  // Check that this is true.  We must use nondet because concat uses epsilon
  // transitions, making the resulting FSM non-deterministic.
  TEST_ASSERT(fsm_sim_nondet(dst, L"foobar"));
  TEST_ASSERT(!fsm_sim_nondet(dst, L"bar"));
  TEST_ASSERT(!fsm_sim_nondet(dst, L"foo"));
  TEST_ASSERT(!fsm_sim_nondet(dst, L""));
  TEST_ASSERT(!fsm_sim_nondet(dst, L"foobarr"));

  fsm_delete(dst, true);
  return 0;
}

/**
   @brief Test the fsm_union() function.

   When I create a union of two FSMs, the union should accept either of the
   inputs, even when the input FSMs have been freed.
 */
static int test_union(void)
{
  fsm *src = fsm_create();
  int s00 = fsm_add_state(src, false);
  int s01 = fsm_add_state(src, false);
  int s02 = fsm_add_state(src, false);
  int s03 = fsm_add_state(src, true);
  fsm *dst = fsm_create();
  int s10 = fsm_add_state(dst, false);
  int s11 = fsm_add_state(dst, false);
  int s12 = fsm_add_state(dst, false);
  int s13 = fsm_add_state(dst, true);

  // src accepts "bar"
  src->start = s00;
  fsm_add_single(src, s00, s01, L'b', L'b', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s01, s02, L'a', L'a', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s02, s03, L'r', L'r', FSM_TRANS_POSITIVE);

  // dst accepts "foo"
  dst->start = s10;
  fsm_add_single(dst, s10, s11, L'f', L'f', FSM_TRANS_POSITIVE);
  fsm_add_single(dst, s11, s12, L'o', L'o', FSM_TRANS_POSITIVE);
  fsm_add_single(dst, s12, s13, L'o', L'o', FSM_TRANS_POSITIVE);

  // They should work as advertised.
  TEST_ASSERT(fsm_sim_det(src, L"bar"));
  TEST_ASSERT(!fsm_sim_det(src, L"foo"));
  TEST_ASSERT(!fsm_sim_det(dst, L"bar"));
  TEST_ASSERT(fsm_sim_det(dst, L"foo"));

  // union them and you should get foo or bar
  fsm_union(dst, src);
  fsm_delete(src, true);

  // Check that this is true.  We must use nondet because concat uses epsilon
  // transitions, making the resulting FSM non-deterministic.
  TEST_ASSERT(!fsm_sim_nondet(dst, L"foobar"));
  TEST_ASSERT(fsm_sim_nondet(dst, L"bar"));
  TEST_ASSERT(fsm_sim_nondet(dst, L"foo"));
  TEST_ASSERT(!fsm_sim_nondet(dst, L""));
  TEST_ASSERT(!fsm_sim_nondet(dst, L"foobarr"));

  fsm_delete(dst, true);
  return 0;
}

/**
   @brief Test the fsm_kleene() function.
 */
static int test_kleene(void)
{
  // Create FSM "src" that accepts "foo".
  fsm *src = fsm_create();
  int s00 = fsm_add_state(src, false);
  int s01 = fsm_add_state(src, false);
  int s02 = fsm_add_state(src, false);
  int s03 = fsm_add_state(src, true);
  src->start = s00;
  fsm_add_single(src, s00, s01, L'f', L'f', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s01, s02, L'o', L'o', FSM_TRANS_POSITIVE);
  fsm_add_single(src, s02, s03, L'o', L'o', FSM_TRANS_POSITIVE);

  // Test the original FSM.
  TEST_ASSERT(!fsm_sim_det(src, L""));
  TEST_ASSERT(fsm_sim_det(src, L"foo"));
  TEST_ASSERT(!fsm_sim_det(src, L"foofoo"));

  // Now modify it to accept "(foo)*"
  fsm_kleene(src);

  // Test that it now accepts 0 or more instances.
  TEST_ASSERT(fsm_sim_nondet(src, L""));
  TEST_ASSERT(fsm_sim_nondet(src, L"foo"));
  TEST_ASSERT(fsm_sim_nondet(src, L"foofoo"));
  TEST_ASSERT(!fsm_sim_nondet(src, L"foobarfoo"));

  // Delete and return
  fsm_delete(src, true);
  return 0;
}

void fsm_test(void)
{
  smb_ut_group *group = su_create_test_group("fsm");

  smb_ut_test *test_memory = su_create_test("test_memory", fsm_test_memory);
  su_add_test(group, test_memory);

  smb_ut_test *fsm_sim_memory = su_create_test("fsm_sim_memory", test_fsm_sim_memory);
  su_add_test(group, fsm_sim_memory);

  smb_ut_test *test_check = su_create_test("test_check", fsm_test_check);
  su_add_test(group, test_check);

  smb_ut_test *shortcut = su_create_test("shortcut", test_shortcut);
  su_add_test(group, shortcut);

  smb_ut_test *trans_copy = su_create_test("trans_copy", test_trans_copy);
  su_add_test(group, trans_copy);

  smb_ut_test *simple_machine = su_create_test("simple_machine", test_simple_machine);
  su_add_test(group, simple_machine);

  smb_ut_test *create_single_char = su_create_test("create_single_char", test_create_single_char);
  su_add_test(group, create_single_char);

  smb_ut_test *copy = su_create_test("copy", test_copy);
  su_add_test(group, copy);

  smb_ut_test *concat = su_create_test("concat", test_concat);
  su_add_test(group, concat);

  smb_ut_test *union_ = su_create_test("union", test_union);
  su_add_test(group, union_);

  smb_ut_test *kleene = su_create_test("kleene", test_kleene);
  su_add_test(group, kleene);

  su_run_group(group);
  su_delete_group(group);
}
