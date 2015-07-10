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
   @brief Test concatenating to a cbuf with no allocation.
 */
int test_cbuf_concat_noalloc(void)
{
  cbuf *c = cb_create(10);
  cb_concat(c, "abc");
  cb_concat(c, "def");
  TEST_ASSERT(0 == strcmp(c->buf, "abcdef"));
  TEST_ASSERT(c->capacity == 10);
  TEST_ASSERT(c->length == 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test concatenating to a wcbuf with no reallocation.
 */
int test_wcbuf_concat_noalloc(void)
{
  wcbuf *wc = wcb_create(10);
  wcb_concat(wc, L"abc");
  wcb_concat(wc, L"def");
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abcdef"));
  TEST_ASSERT(wc->capacity == 10);
  TEST_ASSERT(wc->length == 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test concatenating to a cbuf with reallocation.
 */
int test_cbuf_concat_realloc(void)
{
  cbuf *c = cb_create(4);
  cb_concat(c, "abc");
  cb_concat(c, "def");
  TEST_ASSERT(0 == strcmp(c->buf, "abcdef"));
  TEST_ASSERT(c->capacity == 8);
  TEST_ASSERT(c->length == 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test concatenating to a wcbuf with reallocation.
 */
int test_wcbuf_concat_realloc(void)
{
  wcbuf *wc = wcb_create(4);
  wcb_concat(wc, L"abc");
  wcb_concat(wc, L"def");
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abcdef"));
  TEST_ASSERT(wc->capacity == 8);
  TEST_ASSERT(wc->length == 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending a character to a cbuf without reallocation.
 */
int test_cbuf_append_noalloc(void)
{
  cbuf *c = cb_create(4);
  cb_append(c, 'a');
  cb_append(c, 'b');
  cb_append(c, 'c');
  TEST_ASSERT(0 == strcmp(c->buf, "abc"));
  TEST_ASSERT(c->capacity == 4);
  TEST_ASSERT(c->length == 3);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending a character to a wcbuf without reallocation.
 */
int test_wcbuf_append_noalloc(void)
{
  wcbuf *wc = wcb_create(4);
  wcb_append(wc, L'a');
  wcb_append(wc, L'b');
  wcb_append(wc, L'c');
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abc"));
  TEST_ASSERT(wc->capacity == 4);
  TEST_ASSERT(wc->length == 3);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending a character to a cbuf with reallocation.
 */
int test_cbuf_append_realloc(void)
{
  cbuf *c = cb_create(3);
  cb_append(c, 'a');
  cb_append(c, 'b');
  cb_append(c, 'c');
  TEST_ASSERT(0 == strcmp(c->buf, "abc"));
  TEST_ASSERT(c->capacity == 6);
  TEST_ASSERT(c->length == 3);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending a character to a wcbuf with reallocation.
 */
int test_wcbuf_append_realloc(void)
{
  wcbuf *wc = wcb_create(3);
  wcb_append(wc, L'a');
  wcb_append(wc, L'b');
  wcb_append(wc, L'c');
  TEST_ASSERT(0 == wcscmp(wc->buf, L"abc"));
  TEST_ASSERT(wc->capacity == 6);
  TEST_ASSERT(wc->length == 3);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test printf to a cbuf.
 */
int test_cbuf_printf(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "prefix ");
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
  wcb_concat(wcb, L"prefix ");
  wcb_printf(wcb, L"format %ls %s %d suffix", L"wcs", "mbs", 20);
  TEST_ASSERT(0 == wcscmp(wcb->buf, L"prefix format wcs mbs 20 suffix"));
  wcb_delete(wcb);
  return 0;
}

/**
   @brief Test trimming a cbuf.
 */
int test_cbuf_trim(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "abc");
  TEST_ASSERT(cb->capacity == 8);
  TEST_ASSERT(cb->length == 3);
  cb_trim(cb);
  TEST_ASSERT(cb->capacity == 4);
  TEST_ASSERT(cb->length == 3);
  cb_delete(cb);
  return 0;
}

/**
   @brief Test trimming a wcbuf.
 */
int test_wcbuf_trim(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_concat(wcb, L"abc");
  TEST_ASSERT(wcb->capacity == 8);
  TEST_ASSERT(wcb->length == 3);
  wcb_trim(wcb);
  TEST_ASSERT(wcb->capacity == 4);
  TEST_ASSERT(wcb->length == 3);
  wcb_delete(wcb);
  return 0;
}

/**
   @brief Test clearing a cbuf.
 */
int test_cbuf_clear(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "abc");
  TEST_ASSERT(strcmp(cb->buf, "abc") == 0);
  cb_clear(cb);
  TEST_ASSERT(strcmp(cb->buf, "") == 0);
  cb_delete(cb);
  return 0;
}

/**
   @brief Test clearing a wcbuf.
 */
int test_wcbuf_clear(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_concat(wcb, L"abc");
  TEST_ASSERT(wcscmp(wcb->buf, L"abc") == 0);
  wcb_clear(wcb);
  TEST_ASSERT(wcscmp(wcb->buf, L"") == 0);
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

  smb_ut_test *cbuf_concat_noalloc = su_create_test("cbuf_concat_noalloc", test_cbuf_concat_noalloc);
  su_add_test(group, cbuf_concat_noalloc);

  smb_ut_test *wcbuf_concat_noalloc = su_create_test("wcbuf_concat_noalloc", test_wcbuf_concat_noalloc);
  su_add_test(group, wcbuf_concat_noalloc);

  smb_ut_test *cbuf_concat_realloc = su_create_test("cbuf_concat_realloc", test_cbuf_concat_realloc);
  su_add_test(group, cbuf_concat_realloc);

  smb_ut_test *wcbuf_concat_realloc = su_create_test("wcbuf_concat_realloc", test_wcbuf_concat_realloc);
  su_add_test(group, wcbuf_concat_realloc);

  smb_ut_test *cbuf_printf = su_create_test("cbuf_printf", test_cbuf_printf);
  su_add_test(group, cbuf_printf);

  smb_ut_test *wcbuf_printf = su_create_test("wcbuf_printf", test_wcbuf_printf);
  su_add_test(group, wcbuf_printf);

  smb_ut_test *cbuf_append_noalloc = su_create_test("cbuf_append_noalloc", test_cbuf_append_noalloc);
  su_add_test(group, cbuf_append_noalloc);

  smb_ut_test *wcbuf_append_noalloc = su_create_test("wcbuf_append_noalloc", test_wcbuf_append_noalloc);
  su_add_test(group, wcbuf_append_noalloc);

  smb_ut_test *cbuf_append_realloc = su_create_test("cbuf_append_realloc", test_cbuf_append_realloc);
  su_add_test(group, cbuf_append_realloc);

  smb_ut_test *wcbuf_append_realloc = su_create_test("wcbuf_append_realloc", test_wcbuf_append_realloc);
  su_add_test(group, wcbuf_append_realloc);

  smb_ut_test *cbuf_trim = su_create_test("cbuf_trim", test_cbuf_trim);
  su_add_test(group, cbuf_trim);

  smb_ut_test *wcbuf_trim = su_create_test("wcbuf_trim", test_wcbuf_trim);
  su_add_test(group, wcbuf_trim);

  smb_ut_test *cbuf_clear = su_create_test("cbuf_clear", test_cbuf_clear);
  su_add_test(group, cbuf_clear);

  smb_ut_test *wcbuf_clear = su_create_test("wcbuf_clear", test_wcbuf_clear);
  su_add_test(group, wcbuf_clear);

  su_run_group(group);
  su_delete_group(group);
}
