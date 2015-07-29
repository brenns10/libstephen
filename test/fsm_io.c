/***************************************************************************//**

  @file         fsm_io.c

  @author       Stephen Brennan

  @date         Saturday, 23 May 2015

  @brief        Tests for FSM I/O.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdbool.h>
#include <wchar.h>

#include "libstephen/ut.h"
#include "libstephen/fsm.h"

/**
   @brief This function tests reading in a FSM and running it.

   It reads in the even a's and b's machine, and it simulates it on similar
   inputs.
 */
static int test_read_fsm(void) {
  int i;
  smb_status status = SMB_SUCCESS;
  const wchar_t *machine =
    L"start:0\n"
    L"accept:0\n"
    L"0-1:+a-a\n"
    L"0-2:+b-b\n"
    L"1-0:+a-a\n"
    L"1-3:+b-b\n"
    L"2-3:+a-a\n"
    L"2-0:+b-b\n"
    L"3-1:+b-b\n"
    L"3-2:+a-a\n";
  const wchar_t *inputs[] = {
    L"ababa",
    L"aabaa",
    L"aaaabbbba",
    L"ab",
    L"abab",
    L"aabb"
  };
  const bool results[] = {false, false, false, false, true, true};

  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  for (i = 0; i < sizeof(inputs)/sizeof(wchar_t*); i++) {
    TEST_ASSERT(fsm_sim_nondet(f, inputs[i]) == results[i]);
  }
  fsm_delete(f, true);
  return 0;
}

/**
   @brief This function tests reading an FSM with escape sequences.
 */
static int test_escape(void)
{
  int i;
  smb_status status = SMB_SUCCESS;
  const wchar_t *machine =
    L"start:0\n"
    L"accept:12\n"
    L"0-1:+\\a-\\a\n"           // alarm
    L"1-2:+\\b-\\b\n"           // backspace
    L"2-3:+\\e-\\e\n"           // EPSILON
    L"3-4:+\\f-\\f\n"           // form feed
    L"4-5:+\\n-\\n\n"           // newline (lf)
    L"5-6:+\\r-\\r\n"           // carriage return
    L"6-7:+\\t-\\t\n"           // tab
    L"7-8:+\\v-\\v\n"           // vertical tab
    L"8-9:+\\\\-\\\\\n"         // backslash
    L"9-10:+\\+-\\+\n"          // other character
    L"10-11:+\\x3A-\\x3a\n"     // colon
    L"11-12:+\\u0051-\\u0051\n";// Q
  const wchar_t *inputs[] = {
    L"\a\b\f\n\r\t\v\\+:Q", // accept
    L"a\b\f\n\r\t\v\\+:Q",  // reject
    L"\ab\f\n\r\t\v\\+:Q",  // reject
    L"\a\bf\n\r\t\v\\+:Q",  // reject
    L"\a\b\fn\r\t\v\\+:Q",  // reject
    L"\a\b\f\nr\t\v\\+:Q",  // reject
    L"\a\b\f\n\rt\v\\+:Q",  // reject
    L"\a\b\f\n\r\tv\\+:Q",  // reject
    L"\a\b\f\n\r\t\v/+:Q",  // reject
    L"\a\b\f\n\r\t\v\\-:Q", // reject
    L"\a\b\f\n\r\t\v\\-;Q", // reject
    L"\a\b\f\n\r\t\v\\-:q", // reject
  };
  const bool results[] = {true, false, false, false, false, false, false, false,
                          false, false, false, false};

  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  for (i = 0; i < sizeof(inputs)/sizeof(wchar_t*); i++) {
    TEST_ASSERT(fsm_sim_nondet(f, inputs[i]) == results[i]);
  }
  fsm_delete(f, true);
  return 0;
}

int test_read_combine(void)
{
  const wchar_t *m1spec =
    L"start:0\n"
    L"accept:7\n"
    L"0-1:+s-sS-S\n"
    L"1-2:+t-t\n"
    L"2-3:+e-e\n"
    L"3-4:+p-p\n"
    L"4-5:+h-h\n"
    L"5-6:+e-e\n"
    L"6-7:+n-n\n";

  const wchar_t *m2spec =
    L"start:0\n"
    L"accept:7\n"
    L"0-1:+b-bB-B\n"
    L"1-2:+r-r\n"
    L"2-3:+e-e\n"
    L"3-4:+n-n\n"
    L"4-5:+n-n\n"
    L"5-6:+a-a\n"
    L"6-7:+n-n\n";

  const wchar_t *inputs[] = {
    L"stephen",
    L"Stephen",
    L"brennan",
    L"Brennan",
    L"stephenbrennan",
    L"stephenBrennan",
    L"Stephenbrennan",
    L"StephenBrennan",
    L"StephenstephenStephen",
    L"BrennanbrennanBrennan",
    L""
  };
  const bool results[][6] = {
    // {M1, M2,    M1 U M2, M1M2, M1*,  M2*
    {true,  false, true,  false, true,  false}, // "stephen"
    {true,  false, true,  false, true,  false}, // "Stephen"
    {false, true,  true,  false, false, true }, // "brennan"
    {false, true,  true,  false, false, true }, // "Brennan"
    {false, false, false, true,  false, false}, // "stephenbrennan"
    {false, false, false, true,  false, false}, // "stephenBrennan"
    {false, false, false, true,  false, false}, // "Stephenbrennan"
    {false, false, false, true,  false, false}, // "StephenBrennan"
    {false, false, false, false, true,  false}, // "Stephenstephenstephen"
    {false, false, false, false, false, true }, // "BrennanbrennanBrennan"
    {false, false, false, false, true,  true }  // ""
  };
  int i;
  smb_status status = SMB_SUCCESS;
  fsm *m1Um2, *m1Cm2, *m1S, *m2S;
  fsm *m1 = fsm_read(m1spec, &status), *m2 = fsm_read(m2spec, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  m1Um2 = fsm_copy(m1);
  m1Cm2 = fsm_copy(m1);
  m1S = fsm_copy(m1);
  m2S = fsm_copy(m2);
  fsm_union(m1Um2, m2);
  fsm_concat(m1Cm2, m2);
  fsm_kleene(m1S);
  fsm_kleene(m2S);

  for (i = 0; i < sizeof(inputs)/sizeof(wchar_t *); i++) {
    TEST_ASSERT(results[i][0] == fsm_sim_nondet(m1, inputs[i]));
    TEST_ASSERT(results[i][1] == fsm_sim_nondet(m2, inputs[i]));
    TEST_ASSERT(results[i][2] == fsm_sim_nondet(m1Um2, inputs[i]));
    TEST_ASSERT(results[i][3] == fsm_sim_nondet(m1Cm2, inputs[i]));
    TEST_ASSERT(results[i][4] == fsm_sim_nondet(m1S, inputs[i]));
    TEST_ASSERT(results[i][5] == fsm_sim_nondet(m2S, inputs[i]));
  }

  fsm_delete(m1, true);
  fsm_delete(m2, true);
  fsm_delete(m1Um2, true);
  fsm_delete(m1Cm2, true);
  fsm_delete(m1S, true);
  fsm_delete(m2S, true);
  return 0;
}

int test_empty_string(void)
{
  const wchar_t *machine = L"";
  smb_status status = SMB_SUCCESS;
  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(f == NULL);
  TEST_ASSERT(status == CKY_TOO_FEW_LINES);
  return 0;
}

int test_no_start(void)
{
  const wchar_t *machine = L"accept:1\n0-1:+a-a\n";
  smb_status status = SMB_SUCCESS;
  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(f != NULL);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  fsm_delete(f, true);
  return 0;
}

int test_no_trailing_newline(void)
{
  const wchar_t *machine = L"start:0\naccept:1\n0-1:+a-a";
  smb_status status = SMB_SUCCESS;
  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(f != NULL);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  fsm_delete(f, true);
  return 0;
}

int test_whitespace(void)
{
  const wchar_t *machine = L"start: 0\naccept: 1\n0-1:+a-a";
  smb_status status = SMB_SUCCESS;
  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(f != NULL);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  fsm_delete(f, true);
  return 0;
}

static int fail_on(const wchar_t *machine)
{
  smb_status status = SMB_SUCCESS;
  fsm *f = fsm_read(machine, &status);
  TEST_ASSERT(f == NULL);
  TEST_ASSERT(status == CKY_MALFORMED_TRANS);
  return 0;
}

int test_missing_src(void)
{
  return fail_on(L"start:0\naccept:1\n-1:+a-a");
}

int test_missing_hyphen(void)
{
  return fail_on(L"start:0\naccept:1\n01:+a-a");
}

int test_missing_dst(void)
{
  return fail_on(L"start:0\naccept:1\n-1:+a-a");
}

int test_missing_type(void)
{
  return fail_on(L"start:0\naccept:1\n0-1:a-a");
}

int test_bad_escaping(void)
{
  return fail_on(L"start:0\naccept:1\n0-1:+\\u-\\u");
}

int test_bad_escaping2(void)
{
  return fail_on(L"start:0\naccept:1\n0-1:+\\");
}

int test_simple_str(void)
{
  // Create FSM "src" that accepts "foo".
  wchar_t *machine =
    L"start:0\n"
    L"accept:3\n"
    L"0-1:+f-f\n"
    L"1-2:+o-o\n"
    L"2-3:+o-o\n";
  wchar_t *out;
  fsm *src = fsm_create();
  int s00 = fsm_add_state(src, false);
  int s01 = fsm_add_state(src, false);
  int s02 = fsm_add_state(src, false);
  int s03 = fsm_add_state(src, true);
  src->start = s00;
  fsm_add_single(src, s00, s01, L'f', L'f', 0);
  fsm_add_single(src, s01, s02, L'o', L'o', 0);
  fsm_add_single(src, s02, s03, L'o', L'o', 0);

  out = fsm_str(src);
  TEST_ASSERT(0 == wcscmp(machine, out));
  smb_free(out);
  fsm_delete(src, true);
  return 0;
}

void fsm_io_test(void)
{
  smb_ut_group *group = su_create_test_group("fsm_io");

  smb_ut_test *read_fsm = su_create_test("read_fsm", test_read_fsm);
  su_add_test(group, read_fsm);

  smb_ut_test *escape = su_create_test("escape", test_escape);
  su_add_test(group, escape);

  smb_ut_test *read_combine = su_create_test("read_combine", test_read_combine);
  su_add_test(group, read_combine);

  smb_ut_test *empty_string = su_create_test("empty_string", test_empty_string);
  su_add_test(group, empty_string);

  smb_ut_test *no_start = su_create_test("no_start", test_no_start);
  su_add_test(group, no_start);

  smb_ut_test *no_trailing_newline = su_create_test("no_trailing_newline", test_no_trailing_newline);
  su_add_test(group, no_trailing_newline);

  smb_ut_test *whitespace = su_create_test("whitespace", test_whitespace);
  su_add_test(group, whitespace);

  smb_ut_test *missing_src = su_create_test("missing_src", test_missing_src);
  su_add_test(group, missing_src);

  smb_ut_test *missing_hyphen = su_create_test("missing_hyphen", test_missing_hyphen);
  su_add_test(group, missing_hyphen);

  smb_ut_test *missing_dst = su_create_test("missing_dst", test_missing_dst);
  su_add_test(group, missing_dst);

  smb_ut_test *missing_type = su_create_test("missing_type", test_missing_type);
  su_add_test(group, missing_type);

  smb_ut_test *bad_escaping = su_create_test("bad_escaping", test_bad_escaping);
  su_add_test(group, bad_escaping);

  smb_ut_test *bad_escaping2 = su_create_test("bad_escaping2", test_bad_escaping2);
  su_add_test(group, bad_escaping2);

  smb_ut_test *simple_str = su_create_test("simple_str", test_simple_str);
  su_add_test(group, simple_str);

  su_run_group(group);
  su_delete_group(group);
}
