/***************************************************************************//**

  @file         lex.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 February 2016

  @brief        Lexer tests.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "tests.h"

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

static int test_lex_escapes(void)
{
  Lexer l;
  l.tok = (Token){0};
  l.input = "\\(\\)\\[\\]\\+\\*\\?\\-\\^\\.\\n\\w\\|";
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '(');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == ')');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '[');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == ']');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '+');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '*');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '?');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '-');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '^');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '.');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '\n');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Special);
  TEST_ASSERT(l.tok.c == 'w');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == '|');

  return 0;
}

static int test_lex_tokens(void)
{
  Lexer l;
  l.tok = (Token){0};
  l.input = "()[]+*?-^.|";
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TEST_ASSERT(l.tok.sym == LParen);
  TEST_ASSERT(l.tok.c == '(');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == RParen);
  TEST_ASSERT(l.tok.c == ')');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == LBracket);
  TEST_ASSERT(l.tok.c == '[');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == RBracket);
  TEST_ASSERT(l.tok.c == ']');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Plus);
  TEST_ASSERT(l.tok.c == '+');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Star);
  TEST_ASSERT(l.tok.c == '*');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Question);
  TEST_ASSERT(l.tok.c == '?');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Minus);
  TEST_ASSERT(l.tok.c == '-');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Caret);
  TEST_ASSERT(l.tok.c == '^');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Dot);
  TEST_ASSERT(l.tok.c == '.');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == Pipe);
  TEST_ASSERT(l.tok.c == '|');

  return 0;
}

static int test_lex_buffer(void)
{
  Lexer l;
  l.tok = (Token){0};
  l.input = "abcdef";
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'a');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'b');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'c');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'd');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'e');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'f');
  unget((Token){CharSym, 'e'}, &l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'e');
  unget((Token){CharSym, 'd'}, &l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'd');
  unget((Token){CharSym, 'c'}, &l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'c');
  unget((Token){CharSym, 'b'}, &l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'b');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'c');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'd');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'e');
  nextsym(&l);
  TEST_ASSERT(l.tok.sym == CharSym);
  TEST_ASSERT(l.tok.c == 'f');

  return 0;
}

void lex_test(void)
{
  smb_ut_group *group = su_create_test_group("test/lex.c");

  smb_ut_test *lex_escapes = su_create_test("lex_escapes", test_lex_escapes);
  su_add_test(group, lex_escapes);

  smb_ut_test *lex_tokens = su_create_test("lex_tokens", test_lex_tokens);
  su_add_test(group, lex_tokens);

  smb_ut_test *lex_buffer = su_create_test("lex_buffer", test_lex_buffer);
  su_add_test(group, lex_buffer);

  su_run_group(group);
  su_delete_group(group);
}
