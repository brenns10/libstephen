/***************************************************************************//**

  @file         codegen.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 February 2016

  @brief        Code generation tests.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <string.h>

#include "libstephen/ut.h"
#include "tests.h"

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

static int test_dot(void)
{
  Regex r = recomp(".");

  TA_SIZE_EQ(r.n, (size_t)2);
  TA_INT_EQ(r.i[0].code, Any);
  TA_INT_EQ(r.i[1].code, Match);

  refree(r);
  return 0;
}

static int test_special(void)
{
  Regex r = recomp("\\d");
  TA_SIZE_EQ(r.n, (size_t)2);
  TA_INT_EQ(r.i[0].code, Range);
  TA_SIZE_EQ(r.i[0].s, (size_t)1);
  TA_STRN_EQ("09", (char*)r.i[0].x, 2);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  r = recomp("\\D");
  TA_SIZE_EQ(r.n, (size_t) 2);
  TA_INT_EQ(r.i[0].code, NRange);
  TA_SIZE_EQ(r.i[0].s, (size_t)1);
  TA_STRN_EQ("09", (char*)r.i[0].x, 2);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  r = recomp("\\w");
  TA_SIZE_EQ(r.n, (size_t) 2);
  TA_INT_EQ(r.i[0].code, Range);
  TA_SIZE_EQ(r.i[0].s, (size_t) 4);
  TA_STRN_EQ("azAZ09__", (char*)r.i[0].x, 8);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  r = recomp("\\W");
  TA_SIZE_EQ(r.n, (size_t) 2);
  TA_INT_EQ(r.i[0].code, NRange);
  TA_SIZE_EQ(r.i[0].s, (size_t) 4);
  TA_STRN_EQ("azAZ09__", (char*)r.i[0].x, 8);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  r = recomp("\\s");
  TA_SIZE_EQ(r.n, (size_t) 2);
  TA_INT_EQ(r.i[0].code, Range);
  TA_SIZE_EQ(r.i[0].s, (size_t) 6);
  TA_STRN_EQ("  \t\t\n\n\r\r\f\f\v\v", (char*)r.i[0].x, 12);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  r = recomp("\\S");
  TA_SIZE_EQ(r.n, (size_t) 2);
  TA_INT_EQ(r.i[0].code, NRange);
  TA_SIZE_EQ(r.i[0].s, (size_t) 6);
  TA_STRN_EQ("  \t\t\n\n\r\r\f\f\v\v", (char*)r.i[0].x, 12);
  TA_INT_EQ(r.i[1].code, Match);
  refree(r);

  return 0;
}

static int test_plus(void)
{
  Regex r = recomp("a+");

  TA_SIZE_EQ(r.n, (size_t) 3);
  TA_INT_EQ(r.i[0].code, Char);
  TA_CHAR_EQ(r.i[0].c, 'a');
  TA_INT_EQ(r.i[1].code, Split);
  TA_PTR_EQ(r.i[1].x, r.i);
  TA_PTR_EQ(r.i[1].y, r.i + 2);
  TA_INT_EQ(r.i[2].code, Match);

  refree(r);
  return 0;
}

static int test_plus_question(void)
{
  Regex r = recomp("a+?");

  TA_SIZE_EQ(r.n, 3);
  TA_INT_EQ(r.i[0].code, Char);
  TA_CHAR_EQ(r.i[0].c, 'a');
  TA_INT_EQ(r.i[1].code, Split);
  TA_PTR_EQ(r.i[1].x, r.i + 2);
  TA_PTR_EQ(r.i[1].y, r.i);
  TA_INT_EQ(r.i[2].code, Match);

  refree(r);
  return 0;
}

static int test_star(void)
{
  Regex r = recomp("a*");

  TA_SIZE_EQ(r.n, 4);
  TA_INT_EQ(r.i[0].code, Split);
  TA_PTR_EQ(r.i[0].x, r.i + 1);
  TA_PTR_EQ(r.i[0].y, r.i + 3);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Jump);
  TA_PTR_EQ(r.i[2].x, r.i);
  TA_INT_EQ(r.i[3].code, Match);

  refree(r);
  return 0;
}

static int test_star_question(void)
{
  Regex r = recomp("a*?");

  TA_SIZE_EQ(r.n, 4);
  TA_INT_EQ(r.i[0].code, Split);
  TA_PTR_EQ(r.i[0].x, r.i + 3);
  TA_PTR_EQ(r.i[0].y, r.i + 1);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Jump);
  TA_PTR_EQ(r.i[2].x, r.i);
  TA_INT_EQ(r.i[3].code, Match);

  refree(r);
  return 0;
}

static int test_question(void)
{
  Regex r = recomp("a?");

  TA_SIZE_EQ(r.n, 3);
  TA_INT_EQ(r.i[0].code, Split);
  TA_PTR_EQ(r.i[0].x, r.i + 1);
  TA_PTR_EQ(r.i[0].y, r.i + 2);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Match);

  refree(r);
  return 0;
}

static int test_question_question(void)
{
  Regex r = recomp("a??");

  TA_SIZE_EQ(r.n, 3);
  TA_INT_EQ(r.i[0].code, Split);
  TA_PTR_EQ(r.i[0].x, r.i + 2);
  TA_PTR_EQ(r.i[0].y, r.i + 1);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Match);

  refree(r);
  return 0;
}

static int test_concat(void)
{
  Regex r = recomp("ab");

  TA_SIZE_EQ(r.n, 3);
  TA_INT_EQ(r.i[0].code, Char);
  TA_CHAR_EQ(r.i[0].c, 'a');
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'b');
  TA_INT_EQ(r.i[2].code, Match);

  refree(r);
  return 0;
}

static int test_alternate(void)
{
  Regex r = recomp("a|b");

  TA_SIZE_EQ(r.n, 5);
  TA_INT_EQ(r.i[0].code, Split);
  TA_PTR_EQ(r.i[0].x, r.i + 1);
  TA_PTR_EQ(r.i[0].y, r.i + 3);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Jump);
  TA_PTR_EQ(r.i[2].x, r.i + 4);
  TA_INT_EQ(r.i[3].code, Char);
  TA_CHAR_EQ(r.i[3].c, 'b');
  TA_INT_EQ(r.i[4].code, Match);

  refree(r);
  return 0;
}

static int test_capture(void)
{
  Regex r = recomp("(a)");

  TA_SIZE_EQ(r.n, 4);
  TA_INT_EQ(r.i[0].code, Save);
  TA_SIZE_EQ(r.i[0].s, 0);
  TA_INT_EQ(r.i[1].code, Char);
  TA_CHAR_EQ(r.i[1].c, 'a');
  TA_INT_EQ(r.i[2].code, Save);
  TA_SIZE_EQ(r.i[2].s, 1);
  TA_INT_EQ(r.i[3].code, Match);

  refree(r);
  return 0;
}

static int test_class(void)
{
  Regex r = recomp("[a-bd -]");

  TA_SIZE_EQ(r.n, 2);
  TA_INT_EQ(r.i[0].code, Range);
  TA_SIZE_EQ(r.i[0].s, 4);
  char *block = (char*) r.i[0].x;
  TA_CHAR_EQ(block[0], 'a');
  TA_CHAR_EQ(block[1], 'b');
  TA_CHAR_EQ(block[2], 'd');
  TA_CHAR_EQ(block[3], 'd');
  TA_CHAR_EQ(block[4], ' ');
  TA_CHAR_EQ(block[5], ' ');
  TA_CHAR_EQ(block[6], '-');
  TA_CHAR_EQ(block[7], '-');
  TA_INT_EQ(r.i[1].code, Match);

  refree(r);
  return 0;
}

static int test_nclass(void)
{
  Regex r = recomp("[^a-bd f-g]");

  TA_SIZE_EQ(r.n, 2);
  TA_INT_EQ(r.i[0].code, NRange);
  TA_SIZE_EQ(r.i[0].s, 4);
  char *block = (char*) r.i[0].x;
  TA_CHAR_EQ(block[0], 'a');
  TA_CHAR_EQ(block[1], 'b');
  TA_CHAR_EQ(block[2], 'd');
  TA_CHAR_EQ(block[3], 'd');
  TA_CHAR_EQ(block[4], ' ');
  TA_CHAR_EQ(block[5], ' ');
  TA_CHAR_EQ(block[6], 'f');
  TA_CHAR_EQ(block[7], 'g');
  TA_INT_EQ(r.i[1].code, Match);

  refree(r);
  return 0;
}

static int test_join_complex(void)
{
  Regex r = recomp("a*b+");

  TA_SIZE_EQ(r.n, 6);
  TA_INT_EQ(r.i[0].code, Split);
  TA_INT_EQ(r.i[1].code, Char);
  TA_INT_EQ(r.i[2].code, Jump);
  TA_INT_EQ(r.i[3].code, Char);
  TA_INT_EQ(r.i[4].code, Split);
  TA_INT_EQ(r.i[5].code, Match);

  refree(r);
  return 0;
}

void codegen_test(void)
{
  smb_ut_group *group = su_create_test_group("test/codegen.c");

  smb_ut_test *dot = su_create_test("dot", test_dot);
  su_add_test(group, dot);

  smb_ut_test *special = su_create_test("special", test_special);
  su_add_test(group, special);

  smb_ut_test *plus = su_create_test("plus", test_plus);
  su_add_test(group, plus);

  smb_ut_test *plus_question = su_create_test("plus_question", test_plus_question);
  su_add_test(group, plus_question);

  smb_ut_test *star = su_create_test("star", test_star);
  su_add_test(group, star);

  smb_ut_test *star_question = su_create_test("star_question", test_star_question);
  su_add_test(group, star_question);

  smb_ut_test *question = su_create_test("question", test_question);
  su_add_test(group, question);

  smb_ut_test *question_question = su_create_test("question_question", test_question_question);
  su_add_test(group, question_question);

  smb_ut_test *concat = su_create_test("concat", test_concat);
  su_add_test(group, concat);

  smb_ut_test *alternate = su_create_test("alternate", test_alternate);
  su_add_test(group, alternate);

  smb_ut_test *capture = su_create_test("capture", test_capture);
  su_add_test(group, capture);

  smb_ut_test *class = su_create_test("class", test_class);
  su_add_test(group, class);

  smb_ut_test *nclass = su_create_test("nclass", test_nclass);
  su_add_test(group, nclass);

  smb_ut_test *join_complex = su_create_test("join_complex", test_join_complex);
  su_add_test(group, join_complex);

  su_run_group(group);
  su_delete_group(group);
}
