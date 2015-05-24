/***************************************************************************//**

  @file         charbuftest.c

  @author       Stephen Brennan

  @date         Created Saturday, 23 May 2015

  @brief        Tests for the charbuf.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <string.h>
#include <wchar.h>

#include "libstephen/cb.h"
#include "libstephen/ut.h"

/**
   @brief Ensure that simple operation has no memory leaks.
 */
int test_cbuf_memory(void)
{
  cbuf *c = cb_create(10);
  cb_delete(c);
  return 0;
}

/**
   @brief Ensure that simple operation has no memory leaks.
 */
int test_wcbuf_memory(void)
{
  wcbuf *wc = wcb_create(10);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending to a cbuf with no allocation.
 */
int test_cbuf_append_noalloc(void)
{
  cbuf *c = cb_create(10);
  cb_append(c, "abc");
  cb_append(c, "def");
  TEST_ASSERT(0 == strcmp(c->buf, "abcdef"));
  TEST_ASSERT(c->capacity == 10);
  TEST_ASSERT(c->length == 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending to a wcbuf with no reallocation.
 */
int test_wcbuf_append_noalloc(void)
{
  wcbuf *wc = wcb_create(10);
  wcb_append(wc, L"abc");
  wcb_append(wc, L"def");
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abcdef"));
  TEST_ASSERT(wc->capacity == 10);
  TEST_ASSERT(wc->length == 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending to a cbuf with reallocation.
 */
int test_cbuf_append_realloc(void)
{
  cbuf *c = cb_create(4);
  cb_append(c, "abc");
  cb_append(c, "def");
  TEST_ASSERT(0 == strcmp(c->buf, "abcdef"));
  TEST_ASSERT(c->capacity == 8);
  TEST_ASSERT(c->length == 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending to a wcbuf with reallocation.
 */
int test_wcbuf_append_realloc(void)
{
  wcbuf *wc = wcb_create(4);
  wcb_append(wc, L"abc");
  wcb_append(wc, L"def");
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abcdef"));
  TEST_ASSERT(wc->capacity == 8);
  TEST_ASSERT(wc->length == 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test printf to a cbuf.
 */
int test_cbuf_printf(void)
{
  cbuf *cb = cb_create(8);
  cb_append(cb, "prefix ");
  cb_printf(cb, "format %ls %s %d suffix", L"wcs", "mbs", 20);
  TEST_ASSERT(0 == strcmp(cb->buf, "prefix format wcs mbs 20 suffix"));
  cb_delete(cb);
  return 0;
}

/**
   @brief Test printf to a wcbuf.
 */
int test_wcbuf_printf(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_append(wcb, L"prefix ");
  wcb_printf(wcb, L"format %ls %s %d suffix", L"wcs", "mbs", 20);
  TEST_ASSERT(0 == wcscmp(wcb->buf, L"prefix format wcs mbs 20 suffix"));
  wcb_delete(wcb);
  return 0;
}

void charbuf_test(void)
{
  smb_ut_group *group = su_create_test_group("charbuf");

  smb_ut_test *cbuf_memory = su_create_test("cbuf_memory", test_cbuf_memory);
  su_add_test(group, cbuf_memory);

  smb_ut_test *wcbuf_memory = su_create_test("wcbuf_memory", test_wcbuf_memory);
  su_add_test(group, wcbuf_memory);

  smb_ut_test *cbuf_append_noalloc = su_create_test("cbuf_append_noalloc", test_cbuf_append_noalloc);
  su_add_test(group, cbuf_append_noalloc);

  smb_ut_test *wcbuf_append_noalloc = su_create_test("wcbuf_append_noalloc", test_wcbuf_append_noalloc);
  su_add_test(group, wcbuf_append_noalloc);

  smb_ut_test *cbuf_append_realloc = su_create_test("cbuf_append_realloc", test_cbuf_append_realloc);
  su_add_test(group, cbuf_append_realloc);

  smb_ut_test *wcbuf_append_realloc = su_create_test("wcbuf_append_realloc", test_wcbuf_append_realloc);
  su_add_test(group, wcbuf_append_realloc);

  smb_ut_test *cbuf_printf = su_create_test("cbuf_printf", test_cbuf_printf);
  su_add_test(group, cbuf_printf);

  smb_ut_test *wcbuf_printf = su_create_test("wcbuf_printf", test_wcbuf_printf);
  su_add_test(group, wcbuf_printf);

  su_run_group(group);
  su_delete_group(group);
}