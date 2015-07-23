/***************************************************************************//**

  @file         regexsearchtest.c

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Test regex search functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "libstephen/regex.h"

int test_simple_search(void)
{
  smb_status status = SMB_SUCCESS;
  wchar_t *search_text = L"words words words";
  smb_al *results = regex_search(L"\\w+", search_text, false, false);
  regex_hit *hit;
  int starts[] = {0, 6, 12};
  int lengths[] = {5, 5, 5};
  int i;
  TEST_ASSERT(al_length(results) == sizeof(starts)/sizeof(int));
  for (i = 0; i < al_length(results); i++) {
    hit = al_get(results, i, &status).data_ptr;
    TEST_ASSERT(status == SMB_SUCCESS);
    TEST_ASSERT(hit->start == starts[i]);
    TEST_ASSERT(hit->length == lengths[i]);
    regex_hit_delete(hit);
  }
  al_delete(results);
  return 0;
}

int test_greedy_search(void)
{
  smb_status status = SMB_SUCCESS;
  wchar_t *search_text = L"Give me a call at (123) 456-7890!";
  smb_al *results = regex_search(L"\\(\\d\\d\\d\\) ?\\d\\d\\d-\\d\\d\\d\\d",
                                 search_text, true, false);
  regex_hit *hit;
  TEST_ASSERT(al_length(results) == 1);
  hit = al_get(results, 0, &status).data_ptr;
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(hit->start == 18);
  TEST_ASSERT(hit->length == 14);
  regex_hit_delete(hit);
  al_delete(results);
  return 0;
}

int test_overlap(void)
{
  smb_status status = SMB_SUCCESS;
  wchar_t *search_text = L"ImAWord";
  smb_al *results = regex_search(L"\\w+", search_text, false, true);
  regex_hit *hit;
  int starts[] = {0, 1, 2, 3, 4, 5, 6};
  int lengths[] = {7, 6, 5, 4, 3, 2, 1};
  int i;
  TEST_ASSERT(al_length(results) == sizeof(starts)/sizeof(int));
  for (i = 0; i < al_length(results); i++) {
    hit = al_get(results, i, &status).data_ptr;
    TEST_ASSERT(status == SMB_SUCCESS);
    TEST_ASSERT(hit->start == starts[i]);
    TEST_ASSERT(hit->length == lengths[i]);
    regex_hit_delete(hit);
  }
  al_delete(results);
  return 0;
}

void regex_search_test(void)
{
  smb_ut_group *group = su_create_test_group("regex_search");

  smb_ut_test *simple_search = su_create_test("simple_search", test_simple_search);
  su_add_test(group, simple_search);

  smb_ut_test *greedy_search = su_create_test("greedy_search", test_greedy_search);
  su_add_test(group, greedy_search);

  smb_ut_test *overlap = su_create_test("overlap", test_overlap);
  su_add_test(group, overlap);

  su_run_group(group);
  su_delete_group(group);
}
