/***************************************************************************//**

  @file         regextest.c

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Tests for stuff under regex/.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "libstephen/fsm.h"
#include "libstephen/regex.h"
#include "libstephen/log.h"

static int test_memory(void)
{
  regex_hit *hit = regex_hit_create(0, 5);
  TEST_ASSERT(hit->start == 0);
  TEST_ASSERT(hit->length == 5);
  regex_hit_delete(hit);
  return 0;
}

static int test_single(void)
{
  fsm *f = regex_parse(L"a");
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"c"));
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abcd"));
  fsm_delete(f, true);
  return 0;
}

static int test_multiple(void)
{
  fsm *f = regex_parse(L"abcd");
  TEST_ASSERT(fsm_sim_nondet(f, L"abcd"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abcde"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abc"));
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"blah"));
  fsm_delete(f, true);
  return 0;
}

static int test_character_class(void)
{
  fsm *f = regex_parse(L"[abcd]");
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(fsm_sim_nondet(f, L"c"));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"e"));
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abcd"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"uuuu"));
  fsm_delete(f, true);
  return 0;
}

static int test_class_range(void)
{
  fsm *f = regex_parse(L"[a-d]");
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(fsm_sim_nondet(f, L"c"));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"e"));
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abcd"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"uuuu"));
  fsm_delete(f, true);
  return 0;
}

static int test_subexpression(void)
{
  fsm *f = regex_parse(L"(a|b|c|d)");
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(fsm_sim_nondet(f, L"c"));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"e"));
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"abcd"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"uuuu"));
  fsm_delete(f, true);
  return 0;
}

static int test_plus(void)
{
  fsm *f = regex_parse(L"a+");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aa"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aaa"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ab"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ba"));
  fsm_delete(f, true);
  return 0;
}

static int test_question(void)
{
  fsm *f = regex_parse(L"a?");
  TEST_ASSERT(fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"aa"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"aaa"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ab"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ba"));
  fsm_delete(f, true);
  return 0;
}

static int test_kleene(void)
{
  fsm *f = regex_parse(L"a*");
  TEST_ASSERT(fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aa"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aaa"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ab"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ba"));
  fsm_delete(f, true);
  return 0;
}

static int test_dot(void)
{
  fsm *f = regex_parse(L".");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(fsm_sim_nondet(f, L"!"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ab"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ba"));
  fsm_delete(f, true);
  return 0;
}

static int test_whitespace(void)
{
  fsm *f = regex_parse(L"\\s+");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L" \t \v\r\n\f"));
  TEST_ASSERT(!fsm_sim_nondet(f, L" \t\v\r\n\f,"));
  TEST_ASSERT(!fsm_sim_nondet(f, L" \t\v\r\n\fa"));
  TEST_ASSERT(!fsm_sim_nondet(f, L" \t\v\r\n\fA"));
  TEST_ASSERT(!fsm_sim_nondet(f, L" \t\v\r\n\f1"));
  fsm_delete(f, true);
  return 0;
}

static int test_word(void)
{
  fsm *f = regex_parse(L"\\w+");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"Aa0_9zBZ"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"Aa0_9zBZ,"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"Aa0_9zBZ "));
  TEST_ASSERT(!fsm_sim_nondet(f, L"Aa0_9zBZ'"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"Aa0_9zBZ\t"));
  fsm_delete(f, true);
  return 0;
}

static int test_digit(void)
{
  fsm *f = regex_parse(L"\\d+");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"0123456789"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789_"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789A"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789!"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789 "));
  fsm_delete(f, true);
  return 0;
}

static int test_whitespace_neg(void)
{
  fsm *f = regex_parse(L"\\S+");
  TEST_ASSERT(fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L" \t \v\r\n\f"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aoeu_1245+[!{("));
  fsm_delete(f, true);
  return 0;
}

static int test_word_neg(void)
{
  fsm *f = regex_parse(L"\\W+");
  TEST_ASSERT(fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"Aa0_9zBZ"));
  TEST_ASSERT(fsm_sim_nondet(f, L"&=!}([=*})+ \t\v\r\n"));
  fsm_delete(f, true);
  return 0;
}

static int test_digit_neg(void)
{
  fsm *f = regex_parse(L"\\D+");
  TEST_ASSERT(fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"0123456789"));
  TEST_ASSERT(fsm_sim_nondet(f, L"aBcD \t\r\n =}#!]!"));
  fsm_delete(f, true);
  return 0;
}

static int test_class_neg(void)
{
  fsm *f = regex_parse(L"[^abc]");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(fsm_sim_nondet(f, L"A"));
  TEST_ASSERT(fsm_sim_nondet(f, L"9"));
  TEST_ASSERT(fsm_sim_nondet(f, L" "));
  TEST_ASSERT(fsm_sim_nondet(f, L"!"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"c"));
  fsm_delete(f, true);
  return 0;
}

static int test_class_neg_range(void)
{
  fsm *f = regex_parse(L"[^a-c]");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(fsm_sim_nondet(f, L"A"));
  TEST_ASSERT(fsm_sim_nondet(f, L"9"));
  TEST_ASSERT(fsm_sim_nondet(f, L" "));
  TEST_ASSERT(fsm_sim_nondet(f, L"!"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"c"));
  fsm_delete(f, true);
  return 0;
}

static int test_hyphen(void)
{
  fsm * f = regex_parse(L"[^a-c-]");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(fsm_sim_nondet(f, L"d"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"b"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"c"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"-"));
  fsm_delete(f, true);
  return 0;
}

static int test_class_escape(void)
{
  fsm *f = regex_parse(L"[^\\n]");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"\n"));
  TEST_ASSERT(fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L" "));
  TEST_ASSERT(fsm_sim_nondet(f, L"#"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"ab"));
  TEST_ASSERT(!fsm_sim_nondet(f, L"\nb"));
  fsm_delete(f, true);
  return 0;
}

static int test_escape(void)
{
  fsm * f = regex_parse(L"\\n");
  TEST_ASSERT(!fsm_sim_nondet(f, L""));
  TEST_ASSERT(!fsm_sim_nondet(f, L"a"));
  TEST_ASSERT(fsm_sim_nondet(f, L"\n"));
  fsm_delete(f, true);
  return 0;
}

static int test_capture(void)
{
  fsm *f = regex_parse(L"number (\\d+)");
  smb_al *capture = NULL;
  smb_status status = SMB_SUCCESS;

  TEST_ASSERT(fsm_sim_nondet_capture(f, L"number 55", &capture));
  TEST_ASSERT(capture != NULL);
  TEST_ASSERT(al_length(capture) == 2);
  TEST_ASSERT(al_get(capture, 0, &status).data_llint == 7);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 1, &status).data_llint == 9);
  TEST_ASSERT(status == SMB_SUCCESS);

  fsm_delete(f, true);
  al_delete(capture);
  return 0;
}

static int test_capture_many(void)
{
  fsm *f = regex_parse(L"numbers: (\\d+) +(\\d+) +(\\d+)");
  smb_al *capture = NULL;
  smb_status status = SMB_SUCCESS;

  TEST_ASSERT(fsm_sim_nondet_capture(f, L"numbers: 55 987654321 1", &capture));
  TEST_ASSERT(capture != NULL);
  TEST_ASSERT(al_length(capture) == 6);
  iter_print(al_get_iter(capture), stdout, data_printer_int);
  TEST_ASSERT(al_get(capture, 0, &status).data_llint == 9);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 1, &status).data_llint == 11);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 2, &status).data_llint == 12);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 3, &status).data_llint == 21);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 4, &status).data_llint == 22);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_get(capture, 5, &status).data_llint == 23);
  TEST_ASSERT(status == SMB_SUCCESS);

  fsm_delete(f, true);
  al_delete(capture);
  return 0;
}

void regex_test(void)
{
  smb_ut_group *group = su_create_test_group("regex");

  smb_ut_test *memory = su_create_test("memory", test_memory);
  su_add_test(group, memory);

  smb_ut_test *single = su_create_test("single", test_single);
  su_add_test(group, single);

  smb_ut_test *multiple = su_create_test("multiple", test_multiple);
  su_add_test(group, multiple);

  smb_ut_test *character_class = su_create_test("character_class", test_character_class);
  su_add_test(group, character_class);

  smb_ut_test *class_range = su_create_test("class_range", test_class_range);
  su_add_test(group, class_range);

  smb_ut_test *subexpression = su_create_test("subexpression", test_subexpression);
  su_add_test(group, subexpression);

  smb_ut_test *plus = su_create_test("plus", test_plus);
  su_add_test(group, plus);

  smb_ut_test *question = su_create_test("question", test_question);
  su_add_test(group, question);

  smb_ut_test *kleene = su_create_test("kleene", test_kleene);
  su_add_test(group, kleene);

  smb_ut_test *dot = su_create_test("dot", test_dot);
  su_add_test(group, dot);

  smb_ut_test *whitespace = su_create_test("whitespace", test_whitespace);
  su_add_test(group, whitespace);

  smb_ut_test *word = su_create_test("word", test_word);
  su_add_test(group, word);

  smb_ut_test *digit = su_create_test("digit", test_digit);
  su_add_test(group, digit);

  smb_ut_test *whitespace_neg = su_create_test("whitespace_neg", test_whitespace_neg);
  su_add_test(group, whitespace_neg);

  smb_ut_test *word_neg = su_create_test("word_neg", test_word_neg);
  su_add_test(group, word_neg);

  smb_ut_test *digit_neg = su_create_test("digit_neg", test_digit_neg);
  su_add_test(group, digit_neg);

  smb_ut_test *class_neg = su_create_test("class_neg", test_class_neg);
  su_add_test(group, class_neg);

  smb_ut_test *class_neg_range = su_create_test("class_neg_range", test_class_neg_range);
  su_add_test(group, class_neg_range);

  smb_ut_test *hyphen = su_create_test("hyphen", test_hyphen);
  su_add_test(group, hyphen);

  smb_ut_test *class_escape = su_create_test("class_escape", test_class_escape);
  su_add_test(group, class_escape);

  smb_ut_test *escape = su_create_test("escape", test_escape);
  su_add_test(group, escape);

  smb_ut_test *capture = su_create_test("capture", test_capture);
  su_add_test(group, capture);

  smb_ut_test *capture_many = su_create_test("capture_many", test_capture_many);
  su_add_test(group, capture_many);

  su_run_group(group);
  su_delete_group(group);
}
