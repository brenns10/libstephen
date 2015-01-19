/***************************************************************************//**

  @file         linkedlisttest.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Test of the linked list.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
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

#include <stdio.h>

#include "tests.h"
#include "libstephen/ll.h"
#include "libstephen/ut.h"

int ll_test_create()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  d.data_llint = 13;

  smb_ll *list = ll_create();
  ll_append(list, d);

  TEST_ASSERT(ll_length(list) == 1);

  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 13);
  TEST_ASSERT(status == SMB_SUCCESS);

  ll_delete(list);
  return 0;
}

int ll_test_create_empty()
{
  smb_ll *list = ll_create();
  TEST_ASSERT(ll_length(list) == 0);
  ll_delete(list);
  return 0;
}

int ll_test_append()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_ll *list = ll_create();

  for (d.data_llint = 0; d.data_llint < 200; d.data_llint++) {
    // Put a small, 200 item load on it.  This tests appending on
    // empty and general appending.
    ll_append(list, d);
    TEST_ASSERT(ll_length(list) == d.data_llint + 1);

    // Test that the data is correct.
    for (int i = 0; i < ll_length(list); i++) {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  ll_delete(list);
  return 0;
}

int ll_test_prepend()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_ll *list = ll_create();

  // Test prepend about 200 times...
  for (d.data_llint = 0; d.data_llint < 200; d.data_llint++) {
    ll_prepend(list, d);
    TEST_ASSERT(ll_length(list) == d.data_llint + 1);

    for (int i = 0; i < ll_length(list); i++) {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == d.data_llint - i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  ll_delete(list);
  return 0;
}

int ll_test_set()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_ll *list = ll_create();
  const int length = 30;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is correct
  TEST_ASSERT(ll_length(list) == length);

  // Test set
  for (int i = 0; i < ll_length(list); i++) {
    d.data_llint = length - i;
    ll_set(list, i, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
    TEST_ASSERT(ll_get(list, i, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is still correct
  TEST_ASSERT(ll_length(list) == length);

  ll_delete(list);
  return 0;
}

int ll_test_remove()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_ll *list = ll_create();
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Remove first element
  ll_remove(list, 0, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_length(list) == length - 1);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Remove middle element
  ll_remove(list, 10, &status); // list[10] == 11 before
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_length(list) == length - 2);
  TEST_ASSERT(ll_get(list, 10, &status).data_llint == 12);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Remove last element
  ll_remove(list, ll_length(list) - 1, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_length(list) == length - 3);

  // Test all elements values
  int value = 1;
  for (int i = 0; i < length - 3; i++) {
    if (i == 10) value++;
    TEST_ASSERT(ll_get(list, i, &status).data_llint == value);
    TEST_ASSERT(status == SMB_SUCCESS);
    value++;
  }

  ll_delete(list);
  return 0;
}

int ll_test_insert()
{
  smb_status status = SMB_SUCCESS;
  DATA d;
  smb_ll *list = ll_create();
  const int length = 20;

  // Create the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    ll_append(list, d);
  }

  // Verify the data
  for (d.data_llint = 0 ; d.data_llint < length; d.data_llint++) {
    TEST_ASSERT(ll_get(list, d.data_llint, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Here are the three insertions for the test:
  d.data_llint = 100;
  ll_insert(list, 0, d);
  TEST_ASSERT(ll_length(list) == length + 1);

  d.data_llint = 101;
  ll_insert(list, 10, d);
  TEST_ASSERT(ll_length(list) == length + 2);

  d.data_llint = 102;
  ll_insert(list, ll_length(list), d);
  TEST_ASSERT(ll_length(list) == length + 3);

  int value = 0;

  for (int i = 0; i < ll_length(list); i++) {
    if (i == 0) {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == 100);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == 10) {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == 101);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else if (i == ll_length(list) - 1) {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == 102);
      TEST_ASSERT(status == SMB_SUCCESS);
    } else {
      TEST_ASSERT(ll_get(list, i, &status).data_llint == value);
      TEST_ASSERT(status == SMB_SUCCESS);
      value++;
    }
  }

  ll_delete(list);
  return 0;
}

void linked_list_test()
{
  // Use the smbunit test framework.  Load tests and run them.
  smb_ut_group *group = su_create_test_group("linked list");

  smb_ut_test *create = su_create_test("create", ll_test_create);
  su_add_test(group, create);

  smb_ut_test *create_empty = su_create_test("create_empty", ll_test_create_empty);
  su_add_test(group, create_empty);

  smb_ut_test *append = su_create_test("append", ll_test_append);
  su_add_test(group, append);

  smb_ut_test *prepend = su_create_test("prepend", ll_test_prepend);
  su_add_test(group, prepend);

  smb_ut_test *set = su_create_test("set", ll_test_set);
  su_add_test(group, set);

  smb_ut_test *remove = su_create_test("remove", ll_test_remove);
  su_add_test(group, remove);

  smb_ut_test *insert = su_create_test("insert", ll_test_insert);
  su_add_test(group, insert);

  su_run_group(group);
  su_delete_group(group);
}
