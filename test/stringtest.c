/***************************************************************************//**

  @file         stringtest.c

  @author       Stephen Brennan

  @date         Created Sunday, 12 July 2015

  @brief        Tests for string functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

#include "libstephen/ut.h"
#include "libstephen/str.h"

int test_read_file(void)
{
  FILE *f = fopen("res/file.txt", "r");
  char *expected =
    "This is the first line of my file.\n"
    "This is the second line of my file.\n";
  char *str = read_file(f);

  TA_STR_EQ(expected, str)
  smb_free(str);
  fclose(f);
  return 0;
}

int test_read_filew(void)
{
  FILE *f = fopen("res/file.txt", "r");
  wchar_t *expected =
    L"This is the first line of my file.\n"
    L"This is the second line of my file.\n";
  wchar_t *str = read_filew(f);

  TA_WSTR_EQ(expected, str);
  smb_free(str);
  fclose(f);
  return 0;
}

int test_read_line(void)
{
  FILE *f = fopen("res/file.txt", "r");
  int i = 0;
  char *lines[] = {
    "This is the first line of my file.",
    "This is the second line of my file.",
    ""
  };
  char *line;

  while (!feof(f)) {
    line = read_line(f);
    TA_STR_EQ(lines[i], line)
    smb_free(line);
    i++;
  }
  fclose(f);
  return 0;
}

int test_read_linew(void)
{
  FILE *f = fopen("res/file.txt", "r");
  int i = 0;
  wchar_t *lines[] = {
    L"This is the first line of my file.",
    L"This is the second line of my file.",
    L""
  };
  wchar_t *line;

  while (!feof(f)) {
    line = read_linew(f);
    TA_WSTR_EQ(lines[i], line);
    smb_free(line);
    i++;
  }
  fclose(f);
  return 0;
}

int test_split_lines(void)
{
  char multiline_string[] = "line1\nline2\nline3\n";
  char *lines[] = {"line1", "line2", "line3"};
  char *line;
  smb_ll *linelist = split_lines(multiline_string);
  smb_iter it = ll_get_iter(linelist);
  smb_status st = SMB_SUCCESS;
  while (it.has_next(&it)) {
    line = it.next(&it, &st).data_ptr;
    TA_STR_EQ(lines[it.index-1], line)
    assert(st == SMB_SUCCESS);
  }
  ll_delete(linelist);
  return 0;
}

int test_split_linesw(void)
{
  wchar_t multiline_string[] = L"line1\nline2\nline3\n";
  wchar_t *lines[] = {L"line1", L"line2", L"line3"};
  wchar_t *line;
  smb_ll *linelist = split_linesw(multiline_string);
  smb_iter it = ll_get_iter(linelist);
  smb_status st = SMB_SUCCESS;
  while (it.has_next(&it)) {
    line = it.next(&it, &st).data_ptr;
    TA_WSTR_EQ(lines[it.index-1], line);
    assert(st == SMB_SUCCESS);
  }
  ll_delete(linelist);
  return 0;
}

int test_split_lines_nonewline(void)
{
  char multiline_string[] = "line1\nline2\nline3";
  char *lines[] = {"line1", "line2", "line3"};
  char *line;
  smb_ll *linelist = split_lines(multiline_string);
  smb_iter it = ll_get_iter(linelist);
  smb_status st = SMB_SUCCESS;
  while (it.has_next(&it)) {
    line = it.next(&it, &st).data_ptr;
    TA_STR_EQ(lines[it.index-1], line)
    assert(st == SMB_SUCCESS);
  }
  ll_delete(linelist);
  return 0;
}

int test_split_linesw_nonewline(void)
{
  wchar_t multiline_string[] = L"line1\nline2\nline3";
  wchar_t *lines[] = {L"line1", L"line2", L"line3"};
  wchar_t *line;
  smb_ll *linelist = split_linesw(multiline_string);
  smb_iter it = ll_get_iter(linelist);
  smb_status st = SMB_SUCCESS;
  while (it.has_next(&it)) {
    line = it.next(&it, &st).data_ptr;
    TA_WSTR_EQ(lines[it.index-1], line);
    assert(st == SMB_SUCCESS);
  }
  ll_delete(linelist);
  return 0;
}

void string_test(void)
{
  smb_ut_group *group = su_create_test_group("test/stringtest.c");

  smb_ut_test *read_file = su_create_test("read_file", test_read_file);
  su_add_test(group, read_file);

  smb_ut_test *read_filew = su_create_test("read_filew", test_read_filew);
  su_add_test(group, read_filew);

  smb_ut_test *read_line = su_create_test("read_line", test_read_line);
  su_add_test(group, read_line);

  smb_ut_test *read_linew = su_create_test("read_linew", test_read_linew);
  su_add_test(group, read_linew);

  smb_ut_test *split_lines = su_create_test("split_lines", test_split_lines);
  su_add_test(group, split_lines);

  smb_ut_test *split_linesw = su_create_test("split_linesw", test_split_linesw);
  su_add_test(group, split_linesw);

  smb_ut_test *split_lines_nonewline = su_create_test("split_lines_nonewline", test_split_lines_nonewline);
  su_add_test(group, split_lines_nonewline);

  smb_ut_test *split_linesw_nonewline = su_create_test("split_linesw_nonewline", test_split_linesw_nonewline);
  su_add_test(group, split_linesw_nonewline);

  su_run_group(group);
  su_delete_group(group);
}
