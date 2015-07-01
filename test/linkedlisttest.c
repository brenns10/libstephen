/***************************************************************************//**

  @file         linkedlisttest.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Test of the linked list.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

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

int ll_test_sort()
{
  smb_ll *list = ll_create();
  smb_status status = SMB_SUCCESS;
  int values[] = {8, 1, 3, 5, 12, 4, 7, 9};
  int results[] = {1, 3, 4, 5, 7, 8, 9, 12};
  int i;
  for (i = 0; i < sizeof(values)/sizeof(int); i++) {
    ll_append(list, (DATA){.data_llint=values[i]});
  }
  ll_sort(list, &data_compare_int);
  for (i = 0; i < sizeof(results)/sizeof(int); i++) {
    TEST_ASSERT(ll_get(list, i, &status).data_llint == results[i]);
    TEST_ASSERT(status == SMB_SUCCESS);
  }
  TEST_ASSERT(list->tail->data.data_llint ==
              results[sizeof(results)/sizeof(int) - 1]);
  TEST_ASSERT(list->tail->next == NULL);
  ll_delete(list);
  return 0;
}

int ll_test_sort_empty()
{
  smb_ll *list = ll_create();
  ll_sort(list, &data_compare_int);
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

  smb_ut_test *sort = su_create_test("sort", ll_test_sort);
  su_add_test(group, sort);

  smb_ut_test *sort_empty = su_create_test("sort_empty", ll_test_sort_empty);
  su_add_test(group, sort_empty);

  su_run_group(group);
  su_delete_group(group);
}
