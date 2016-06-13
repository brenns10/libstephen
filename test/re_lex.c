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
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "\\(\\)\\[\\]\\+\\*\\?\\-\\^\\.\\n\\w\\|";
  l.input.wstr = NULL;
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
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "()[]+*?-^.|";
  l.input.wstr = NULL;
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
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "abcdef";
  l.input.wstr = NULL;
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

static int test_lex_escapes_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"\\(\\)\\[\\]\\+\\*\\?\\-\\^\\.\\n\\w\\|";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'(');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L')');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'[');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L']');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'+');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'*');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'?');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'-');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'^');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'.');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'\n');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Special);
  TA_WCHAR_EQ(l.tok.c, L'w');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'|');

  return 0;
}

static int test_lex_tokens_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"()[]+*?-^.|";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TA_INT_EQ(l.tok.sym, LParen);
  TA_WCHAR_EQ(l.tok.c, L'(');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, RParen);
  TA_WCHAR_EQ(l.tok.c, L')');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, LBracket);
  TA_WCHAR_EQ(l.tok.c, L'[');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, RBracket);
  TA_WCHAR_EQ(l.tok.c, L']');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Plus);
  TA_WCHAR_EQ(l.tok.c, L'+');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Star);
  TA_WCHAR_EQ(l.tok.c, L'*');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Question);
  TA_WCHAR_EQ(l.tok.c, L'?');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Minus);
  TA_WCHAR_EQ(l.tok.c, L'-');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Caret);
  TA_WCHAR_EQ(l.tok.c, L'^');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Dot);
  TA_WCHAR_EQ(l.tok.c, L'.');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, Pipe);
  TA_WCHAR_EQ(l.tok.c, L'|');

  return 0;
}

static int test_lex_buffer_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"abcdef";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'a');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'b');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'c');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'd');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'e');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'f');
  unget((Token){CharSym, L'e'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'e');
  unget((Token){CharSym, L'd'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'd');
  unget((Token){CharSym, L'c'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'c');
  unget((Token){CharSym, L'b'}, &l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'b');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'c');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'd');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'e');
  nextsym(&l);
  TA_INT_EQ(l.tok.sym, CharSym);
  TA_WCHAR_EQ(l.tok.c, L'f');

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

  smb_ut_test *lex_escapes_wide = su_create_test("lex_escapes_wide", test_lex_escapes_wide);
  su_add_test(group, lex_escapes_wide);

  smb_ut_test *lex_tokens_wide = su_create_test("lex_tokens_wide", test_lex_tokens_wide);
  su_add_test(group, lex_tokens_wide);

  smb_ut_test *lex_buffer_wide = su_create_test("lex_buffer_wide", test_lex_buffer_wide);
  su_add_test(group, lex_buffer_wide);

  su_run_group(group);
  su_delete_group(group);
}
