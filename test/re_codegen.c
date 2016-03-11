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
  size_t n;
  instr *prog = recomp(".", &n);

  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == Any);
  TEST_ASSERT(prog[1].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_special(void)
{
  size_t n;
  instr *prog;

  prog = recomp("\\d", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == Range);
  TEST_ASSERT(prog[0].s == 1);
  TEST_ASSERT(0 == strncmp("09", (char*)prog[0].x, 2));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  prog = recomp("\\D", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == NRange);
  TEST_ASSERT(prog[0].s == 1);
  TEST_ASSERT(0 == strncmp("09", (char*)prog[0].x, 2));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  prog = recomp("\\w", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == Range);
  TEST_ASSERT(prog[0].s == 4);
  TEST_ASSERT(0 == strncmp("azAZ09__", (char*)prog[0].x, 8));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  prog = recomp("\\W", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == NRange);
  TEST_ASSERT(prog[0].s == 4);
  TEST_ASSERT(0 == strncmp("azAZ09__", (char*)prog[0].x, 8));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  prog = recomp("\\s", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == Range);
  TEST_ASSERT(prog[0].s == 6);
  TEST_ASSERT(0 == strncmp("  \t\t\n\n\r\r\f\f\v\v", (char*)prog[0].x, 12));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  prog = recomp("\\S", &n);
  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == NRange);
  TEST_ASSERT(prog[0].s == 6);
  TEST_ASSERT(0 == strncmp("  \t\t\n\n\r\r\f\f\v\v", (char*)prog[0].x, 12));
  TEST_ASSERT(prog[1].code == Match);
  free_prog(prog, n);

  return 0;
}

static int test_plus(void)
{
  size_t n;
  instr *prog = recomp("a+", &n);

  TEST_ASSERT(n == 3);
  TEST_ASSERT(prog[0].code == Char);
  TEST_ASSERT(prog[0].c == 'a');
  TEST_ASSERT(prog[1].code == Split);
  TEST_ASSERT(prog[1].x == prog);
  TEST_ASSERT(prog[1].y == prog + 2);
  TEST_ASSERT(prog[2].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_plus_question(void)
{
  size_t n;
  instr *prog = recomp("a+?", &n);

  TEST_ASSERT(n == 3);
  TEST_ASSERT(prog[0].code == Char);
  TEST_ASSERT(prog[0].c == 'a');
  TEST_ASSERT(prog[1].code == Split);
  TEST_ASSERT(prog[1].x == prog + 2);
  TEST_ASSERT(prog[1].y == prog);
  TEST_ASSERT(prog[2].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_star(void)
{
  size_t n;
  instr *prog = recomp("a*", &n);

  TEST_ASSERT(n == 4);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[0].x == prog + 1);
  TEST_ASSERT(prog[0].y == prog + 3);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Jump);
  TEST_ASSERT(prog[2].x == prog);
  TEST_ASSERT(prog[3].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_star_question(void)
{
  size_t n;
  instr *prog = recomp("a*?", &n);

  TEST_ASSERT(n == 4);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[0].x == prog + 3);
  TEST_ASSERT(prog[0].y == prog + 1);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Jump);
  TEST_ASSERT(prog[2].x == prog);
  TEST_ASSERT(prog[3].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_question(void)
{
  size_t n;
  instr *prog = recomp("a?", &n);

  TEST_ASSERT(n == 3);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[0].x == prog + 1);
  TEST_ASSERT(prog[0].y == prog + 2);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_question_question(void)
{
  size_t n;
  instr *prog = recomp("a??", &n);

  TEST_ASSERT(n == 3);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[0].x == prog + 2);
  TEST_ASSERT(prog[0].y == prog + 1);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_concat(void)
{
  size_t n;
  instr *prog = recomp("ab", &n);

  TEST_ASSERT(n == 3);
  TEST_ASSERT(prog[0].code == Char);
  TEST_ASSERT(prog[0].c == 'a');
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'b');
  TEST_ASSERT(prog[2].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_alternate(void)
{
  size_t n;
  instr *prog = recomp("a|b", &n);

  TEST_ASSERT(n == 5);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[0].x == prog + 1);
  TEST_ASSERT(prog[0].y == prog + 3);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Jump);
  TEST_ASSERT(prog[2].x == prog + 4);
  TEST_ASSERT(prog[3].code == Char);
  TEST_ASSERT(prog[3].c == 'b');
  TEST_ASSERT(prog[4].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_capture(void)
{
  size_t n;
  instr *prog = recomp("(a)", &n);

  TEST_ASSERT(n == 4);
  TEST_ASSERT(prog[0].code == Save);
  TEST_ASSERT(prog[0].s == 0);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[1].c == 'a');
  TEST_ASSERT(prog[2].code == Save);
  TEST_ASSERT(prog[2].s == 1);
  TEST_ASSERT(prog[3].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_class(void)
{
  size_t n;
  instr *prog = recomp("[a-bd -]", &n);

  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == Range);
  TEST_ASSERT(prog[0].s == 4);
  char *block = (char*) prog[0].x;
  TEST_ASSERT(block[0] == 'a');
  TEST_ASSERT(block[1] == 'b');
  TEST_ASSERT(block[2] == 'd');
  TEST_ASSERT(block[3] == 'd');
  TEST_ASSERT(block[4] == ' ');
  TEST_ASSERT(block[5] == ' ');
  TEST_ASSERT(block[6] == '-');
  TEST_ASSERT(block[7] == '-');
  TEST_ASSERT(prog[1].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_nclass(void)
{
  size_t n;
  instr *prog = recomp("[^a-bd f-g]", &n);

  TEST_ASSERT(n == 2);
  TEST_ASSERT(prog[0].code == NRange);
  TEST_ASSERT(prog[0].s == 4);
  char *block = (char*) prog[0].x;
  TEST_ASSERT(block[0] == 'a');
  TEST_ASSERT(block[1] == 'b');
  TEST_ASSERT(block[2] == 'd');
  TEST_ASSERT(block[3] == 'd');
  TEST_ASSERT(block[4] == ' ');
  TEST_ASSERT(block[5] == ' ');
  TEST_ASSERT(block[6] == 'f');
  TEST_ASSERT(block[7] == 'g');
  TEST_ASSERT(prog[1].code == Match);

  free_prog(prog, n);
  return 0;
}

static int test_join_complex(void)
{
  size_t n;
  instr *prog = recomp("a*b+", &n);

  TEST_ASSERT(n == 6);
  TEST_ASSERT(prog[0].code == Split);
  TEST_ASSERT(prog[1].code == Char);
  TEST_ASSERT(prog[2].code == Jump);
  TEST_ASSERT(prog[3].code == Char);
  TEST_ASSERT(prog[4].code == Split);
  TEST_ASSERT(prog[5].code == Match);

  free_prog(prog, n);
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
