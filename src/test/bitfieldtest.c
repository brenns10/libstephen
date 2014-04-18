/*******************************************************************************

  File:         bitfieldtest.c

  Author:       Stephen Brennan

  Date Created: Wednesday,  5 February 2014

  Description:  Tests for the libstephen bitfield functions.

*******************************************************************************/

#include "../libstephen.h"
#include "tests.h"

#define test_bools 80

////////////////////////////////////////////////////////////////////////////////
// TESTS

int bf_test_init() {
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int i;
  int assert = 1;

  bf_init(field, test_bools);

  for (i = 0; i < SMB_BITFIELD_SIZE(test_bools); i++) {
    TEST_ASSERT(field[i] == 0, assert);
    assert++;
  }

  return 0;
}

int bf_test_memory() {
  unsigned char *field;

  field = bf_create(test_bools);
  bf_delete(field, test_bools);

  return 0; // looking for memory leaks here
}

int bf_test_check() {
  unsigned char field[2] = {0x00, 0xFF};
  int assert = 1;
  int i;

  // don't want to init...that ruins our test variables
  //bf_init(field, 16);

  for (i = 0; i < 8; i++) {
    TEST_ASSERT(!bf_check(field, i), assert);
    assert++;
  }

  for (i = 8; i < 16; i++) {
    TEST_ASSERT(bf_check(field, i), assert);
    assert++;
  }

  return 0;
}

int bf_test_set() {
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int assert = 1;
  int i;

  bf_init(field, test_bools);
  for (i = 0; i < test_bools; i += 2) {
    TEST_ASSERT(!bf_check(field, i), assert);
    //    assert++;
    bf_set(field, i);
    TEST_ASSERT(bf_check(field, i), assert);
    assert++;
  }

  for (i = 0; i < test_bools; i++) {
    if (i % 2 == 0) {// even
      TEST_ASSERT(bf_check(field, i), assert);
    } else {
      TEST_ASSERT(!bf_check(field, i), assert);
    }
    assert++;
  }

  return 0;
}

int bf_test_clear() {
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int assert = 1;
  int i;

  bf_init(field, test_bools);
  for (i = 0; i < test_bools; i++) {
    bf_set(field, i); // this is already tested
  }

  for (i = 0; i < test_bools; i += 2) {
    TEST_ASSERT(bf_check(field, i), assert);
    //    assert++;
    bf_clear(field, i);
    TEST_ASSERT(!bf_check(field, i), assert);
    assert++;
  }

  for (i = 0; i < test_bools; i++) {
    if (i % 2 == 0) {// even
      TEST_ASSERT(!bf_check(field, i), assert);
    } else {
      TEST_ASSERT(bf_check(field, i), assert);
    }
    assert++;
  }

  return 0;
}

int bf_test_flip() {
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int assert = 1;
  int i;

  // Set all the even numbered fields
  for (i = 0; i < test_bools; i += 2) {
    bf_set(field, i); //tested
  }

  // Flip them all
  for (i = 0; i < test_bools; i++) {
    bf_flip(field, i);
  }

  for (i = 0; i < test_bools; i++) {
    if (i % 2 == 0) {// even
      TEST_ASSERT(!bf_check(field, i), assert);
    } else {
      TEST_ASSERT(bf_check(field, i), assert);
    }
    assert++;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST LOADER AND RUNNER

void bit_field_test() {
  struct smb_ut_group *group = su_create_test_group("bit field");

  struct smb_ut_test *init = su_create_test("init", bf_test_init, 0, 1);
  su_add_test(group, init);

  struct smb_ut_test *memory = su_create_test("memory", bf_test_memory, 0, 1);
  su_add_test(group, memory);

  struct smb_ut_test *check = su_create_test("check", bf_test_check, 0, 1);
  su_add_test(group, check);

  struct smb_ut_test *set = su_create_test("set", bf_test_set, 0, 1);
  su_add_test(group, set);

  struct smb_ut_test *clear = su_create_test("clear", bf_test_set, 0, 1);
  su_add_test(group, clear);

  struct smb_ut_test *flip = su_create_test("flip", bf_test_set, 0, 1);
  su_add_test(group, flip);

  su_run_group(group);
  su_delete_group(group);
}
