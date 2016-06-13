/***************************************************************************//**

  @file         lex.c

  @author       Stephen Brennan

  @date         Created Friday, 29 January 2016

  @brief        All lexer-related functions.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>

#include "libstephen/re_internals.h"

void escape(Lexer *l)
{
  switch (InputIdx(l->input, l->index)) {
  case L'(':
    l->tok = (Token){CharSym, L'('};
    break;
  case L')':
    l->tok = (Token){CharSym, L')'};
    break;
  case L'[':
    l->tok = (Token){CharSym, L'['};
    break;
  case L']':
    l->tok = (Token){CharSym, L']'};
    break;
  case L'+':
    l->tok = (Token){CharSym, L'+'};
    break;
  case L'-':
    l->tok = (Token){CharSym, L'-'};
    break;
  case L'*':
    l->tok = (Token){CharSym, L'*'};
    break;
  case L'?':
    l->tok = (Token){CharSym, L'?'};
    break;
  case L'^':
    l->tok = (Token){CharSym, L'^'};
    break;
  case L'n':
    l->tok = (Token){CharSym, L'\n'};
    break;
  case L'.':
    l->tok = (Token){CharSym, L'.'};
    break;
  case L'|':
    l->tok = (Token){CharSym, L'|'};
    break;
  default:
    l->tok = (Token){Special, InputIdx(l->input, l->index)};
    break;
  }
}

Token nextsym(Lexer *l)
{
  if (l->tok.sym == Eof) {
    return l->tok; // eof never ceases to be eof!
  }

  // Handle buffered symbols first.
  l->prev = l->tok;
  if (l->nbuf > 0) {
    l->tok = l->buf[0];
    for (size_t i = 0; i < l->nbuf - 1; i++) {
      l->buf[i] = l->buf[i+1];
    }
    l->nbuf--;
    //printf(";; nextsym(): unbuffering {%s, '%s'}\n", names[l->tok.sym],
    //       char_to_string(l->tok.c));
    return l->tok;
  }

  switch (InputIdx(l->input, l->index)) {
  case L'(':
    l->tok = (Token){LParen, L'('};
    break;
  case L')':
    l->tok = (Token){RParen, L')'};
    break;
  case L'[':
    l->tok = (Token){LBracket, L'['};
    break;
  case L']':
    l->tok = (Token){RBracket, L']'};
    break;
  case L'+':
    l->tok = (Token){Plus, L'+'};
    break;
  case L'-':
    l->tok = (Token){Minus, L'-'};
    break;
  case L'*':
    l->tok = (Token){Star, L'*'};
    break;
  case L'?':
    l->tok = (Token){Question, L'?'};
    break;
  case L'^':
    l->tok = (Token){Caret, L'^'};
    break;
  case L'|':
    l->tok = (Token){Pipe, L'|'};
    break;
  case L'.':
    l->tok = (Token){Dot, L'.'};
    break;
  case L'\\':
    l->index++;
    escape(l);
    break;
  case L'\0':
    l->tok = (Token){Eof, L'\0'};
    break;
  default:
    l->tok = (Token){CharSym, InputIdx(l->input, l->index)};
    break;
  }
  l->index++;
  //printf(";; nextsym(): {%s, '%s'}\n", names[l->tok.sym], char_to_string(l->tok.c));
  return l->tok;
}

void unget(Token t, Lexer *l)
{
  if (l->nbuf >= LEXER_BUFSIZE) {
    fprintf(stderr, "error: maximum lexer buffer size exceeded, dumbass.\n");
    exit(1);
  }

  //printf(";; unget(): buffering {%s, '%s'}\n", names[t.sym], char_to_string(t.c));

  for (int i = l->nbuf - 1; i >= 0; i--) {
    l->buf[i+1] = l->buf[i];
  }
  l->buf[0] = l->tok;
  l->tok = t;
  l->nbuf++;
}
