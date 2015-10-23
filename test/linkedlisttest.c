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
  for (i = 0; (unsigned) i < sizeof(values)/sizeof(int); i++) {
    ll_append(list, (DATA){.data_llint=values[i]});
  }
  ll_sort(list, &data_compare_int);
  for (i = 0; (unsigned) i < sizeof(results)/sizeof(int); i++) {
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

static bool is_even(DATA d) {
  return d.data_llint % 2 == 0;
}

int ll_test_filter_empty()
{
  smb_ll *list = ll_create();
  ll_filter(list, &is_even);
  TEST_ASSERT(ll_length(list) == 0);
  ll_delete(list);
  return 0;
}

int ll_test_filter_no_match(void)
{
  smb_status status = SMB_SUCCESS;
  smb_ll *list = ll_create();
  ll_append(list, LLINT(1));
  ll_append(list, LLINT(3));
  ll_append(list, LLINT(5));
  ll_filter(list, &is_even);
  TEST_ASSERT(ll_length(list) == 3);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 1, &status).data_llint == 3);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 2, &status).data_llint == 5);
  TEST_ASSERT(status == SMB_SUCCESS);
  ll_delete(list);
  return 0;
}

int ll_test_filter(void)
{
  smb_status status = SMB_SUCCESS;
  smb_ll *list = ll_create();
  ll_append(list, LLINT(1));
  ll_append(list, LLINT(2));
  ll_append(list, LLINT(3));
  ll_append(list, LLINT(4));
  ll_filter(list, &is_even);
  TEST_ASSERT(ll_length(list) == 2);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 1, &status).data_llint == 3);
  TEST_ASSERT(status == SMB_SUCCESS);
  ll_delete(list);
  return 0;
}

static DATA increment(DATA d) {
  d.data_llint++;
  return d;
}

int ll_test_map_empty()
{
  smb_ll *list = ll_create();
  ll_map(list, &increment);
  TEST_ASSERT(ll_length(list) == 0);
  ll_delete(list);
  return 0;
}

int ll_test_map(void)
{
  smb_status status = SMB_SUCCESS;
  smb_ll *list = ll_create();
  ll_append(list, LLINT(1));
  ll_append(list, LLINT(2));
  ll_append(list, LLINT(3));
  ll_append(list, LLINT(4));
  ll_map(list, &increment);
  TEST_ASSERT(ll_length(list) == 4);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 2);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 1, &status).data_llint == 3);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 2, &status).data_llint == 4);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 3, &status).data_llint == 5);
  TEST_ASSERT(status == SMB_SUCCESS);
  ll_delete(list);
  return 0;
}

static DATA subtract(DATA d1, DATA d2) {
  return LLINT(d1.data_llint - d2.data_llint);
}

int ll_test_foldl_empty(void)
{
  smb_ll *list = ll_create();
  DATA result = ll_foldl(list, LLINT(0), &subtract);
  TEST_ASSERT(result.data_llint == 0);
  TEST_ASSERT(ll_length(list) == 0);
  ll_delete(list);
  return 0;
}

int ll_test_foldl(void)
{
  smb_status status = SMB_SUCCESS;
  smb_ll *list = ll_create();
  ll_append(list, LLINT(1));
  ll_append(list, LLINT(2));
  ll_append(list, LLINT(3));
  ll_append(list, LLINT(4));
  DATA result = ll_foldl(list, LLINT(0), &subtract);
  TEST_ASSERT(result.data_llint == -10);
  TEST_ASSERT(ll_length(list) == 4);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 1, &status).data_llint == 2);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 2, &status).data_llint == 3);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 3, &status).data_llint == 4);
  TEST_ASSERT(status == SMB_SUCCESS);
  ll_delete(list);
  return 0;
}

int ll_test_foldr_empty(void)
{
  smb_ll *list = ll_create();
  DATA result = ll_foldr(list, LLINT(0), &subtract);
  TEST_ASSERT(result.data_llint == 0);
  TEST_ASSERT(ll_length(list) == 0);
  ll_delete(list);
  return 0;
}

int ll_test_foldr(void)
{
  smb_status status = SMB_SUCCESS;
  smb_ll *list = ll_create();
  ll_append(list, LLINT(1));
  ll_append(list, LLINT(2));
  ll_append(list, LLINT(3));
  ll_append(list, LLINT(4));
  DATA result = ll_foldr(list, LLINT(0), &subtract);
  TEST_ASSERT(result.data_llint == -2);
  TEST_ASSERT(ll_length(list) == 4);
  TEST_ASSERT(ll_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 1, &status).data_llint == 2);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 2, &status).data_llint == 3);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(ll_get(list, 3, &status).data_llint == 4);
  TEST_ASSERT(status == SMB_SUCCESS);
  ll_delete(list);
  return 0;
}

void linked_list_test()
{
  // Use the smbunit test framework.  Load tests and run them.
  smb_ut_group *group = su_create_test_group("test/linkedlisttest.c");

  smb_ut_test *create = su_create_test("create", ll_test_create);
  su_add_test(group, create);

  smb_ut_test *create_empty = su_create_test("create_empty", ll_test_create_empty);
  su_add_test(group, create_empty);

  smb_ut_test *sort = su_create_test("sort", ll_test_sort);
  su_add_test(group, sort);

  smb_ut_test *sort_empty = su_create_test("sort_empty", ll_test_sort_empty);
  su_add_test(group, sort_empty);

  smb_ut_test *filter_empty = su_create_test("filter_empty", ll_test_filter_empty);
  su_add_test(group, filter_empty);

  smb_ut_test *filter_no_match = su_create_test("filter_no_match", ll_test_filter_no_match);
  su_add_test(group, filter_no_match);

  smb_ut_test *filter = su_create_test("filter", ll_test_filter);
  su_add_test(group, filter);

  smb_ut_test *map = su_create_test("map", ll_test_map);
  su_add_test(group, map);

  smb_ut_test *map_empty = su_create_test("map_empty", ll_test_map_empty);
  su_add_test(group, map_empty);

  smb_ut_test *foldl_empty = su_create_test("foldl_empty", ll_test_foldl_empty);
  su_add_test(group, foldl_empty);

  smb_ut_test *foldl = su_create_test("foldl", ll_test_foldl);
  su_add_test(group, foldl);

  smb_ut_test *foldr_empty = su_create_test("foldr_empty", ll_test_foldr_empty);
  su_add_test(group, foldr_empty);

  smb_ut_test *foldr = su_create_test("foldr", ll_test_foldr);
  su_add_test(group, foldr);

  su_run_group(group);
  su_delete_group(group);
}
