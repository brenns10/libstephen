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
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '(');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, ')');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '[');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, ']');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '+');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '*');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '?');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '-');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '^');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '.');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '\n');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Special);
  TA_CHAR_EQ(l.tok.c, 'w');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, '|');

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
  TA_INT_EQ(l.tok.sym, LParen);
  TA_CHAR_EQ(l.tok.c, '(');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, RParen);
  TA_CHAR_EQ(l.tok.c, ')');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, LBracket);
  TA_CHAR_EQ(l.tok.c, '[');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, RBracket);
  TA_CHAR_EQ(l.tok.c, ']');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Plus);
  TA_CHAR_EQ(l.tok.c, '+');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Star);
  TA_CHAR_EQ(l.tok.c, '*');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Question);
  TA_CHAR_EQ(l.tok.c, '?');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Minus);
  TA_CHAR_EQ(l.tok.c, '-');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Caret);
  TA_CHAR_EQ(l.tok.c, '^');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Dot);
  TA_CHAR_EQ(l.tok.c, '.');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Pipe);
  TA_CHAR_EQ(l.tok.c, '|');

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
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'a');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'b');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'c');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'd');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'e');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'f');
  unget((Token){CharSym, 'e'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'e');
  unget((Token){CharSym, 'd'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'd');
  unget((Token){CharSym, 'c'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'c');
  unget((Token){CharSym, 'b'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'b');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'c');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'd');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'e');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_CHAR_EQ(l.tok.c, 'f');

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
