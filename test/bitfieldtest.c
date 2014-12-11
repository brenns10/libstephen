/***************************************************************************//**

  @file         bitfieldtest.c

  @author       Stephen Brennan

  @date         Created Wednesday,  5 February 2014

  @brief        Test for the libstephen bitfield.

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

#include "libstephen/bf.h"
#include "libstephen/ut.h"
#include "tests.h"

#define test_bools 80

////////////////////////////////////////////////////////////////////////////////
// TESTS

int bf_test_init()
{
  smb_status status;
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int i;

  bf_init(field, test_bools, &status);
  TEST_ASSERT(status == SMB_SUCCESS);

  for (i = 0; i < SMB_BITFIELD_SIZE(test_bools); i++) {
    TEST_ASSERT(field[i] == 0);
  }

  return 0;
}

int bf_test_memory()
{
  smb_status status;
  unsigned char *field;

  field = bf_create(test_bools, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  bf_delete(field, test_bools);

  return 0; // looking for memory leaks here
}

int bf_test_check()
{
  smb_status status;
  unsigned char field[2] = {0x00, 0xFF};
  int i;

  // don't want to init...that ruins our test variables
  //bf_init(field, 16);

  for (i = 0; i < 8; i++) {
    TEST_ASSERT(!bf_check(field, i));
  }

  for (i = 8; i < 16; i++) {
    TEST_ASSERT(bf_check(field, i));
  }

  return 0;
}

int bf_test_set()
{
  smb_status status;
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int i;

  bf_init(field, test_bools, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  for (i = 0; i < test_bools; i += 2) {
    TEST_ASSERT(!bf_check(field, i));
    bf_set(field, i);
    TEST_ASSERT(bf_check(field, i));
  }

  for (i = 0; i < test_bools; i++) {
    if (i % 2 == 0) {// even
      TEST_ASSERT(bf_check(field, i));
    } else {
      TEST_ASSERT(!bf_check(field, i));
    }
  }

  return 0;
}

int bf_test_clear()
{
  smb_status status;
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int i;

  bf_init(field, test_bools, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  for (i = 0; i < test_bools; i++) {
    bf_set(field, i); // this is already tested
  }

  for (i = 0; i < test_bools; i += 2) {
    TEST_ASSERT(bf_check(field, i));
    bf_clear(field, i);
    TEST_ASSERT(!bf_check(field, i));
  }

  for (i = 0; i < test_bools; i++) {
    if (i % 2 == 0) {// even
      TEST_ASSERT(!bf_check(field, i));
    } else {
      TEST_ASSERT(bf_check(field, i));
    }
  }

  return 0;
}

int bf_test_flip()
{
  smb_status status;
  unsigned char field[SMB_BITFIELD_SIZE(test_bools)];
  int i;

  bf_init(field, test_bools, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
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
      TEST_ASSERT(!bf_check(field, i));
    } else {
      TEST_ASSERT(bf_check(field, i));
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST LOADER AND RUNNER

void bit_field_test() {
  smb_ut_group *group = su_create_test_group("bit field");

  smb_ut_test *init = su_create_test("init", bf_test_init, 1);
  su_add_test(group, init);

  smb_ut_test *memory = su_create_test("memory", bf_test_memory, 1);
  su_add_test(group, memory);

  smb_ut_test *check = su_create_test("check", bf_test_check, 1);
  su_add_test(group, check);

  smb_ut_test *set = su_create_test("set", bf_test_set, 1);
  su_add_test(group, set);

  smb_ut_test *clear = su_create_test("clear", bf_test_clear, 1);
  su_add_test(group, clear);

  smb_ut_test *flip = su_create_test("flip", bf_test_flip, 1);
  su_add_test(group, flip);

  su_run_group(group);
  su_delete_group(group);
}
