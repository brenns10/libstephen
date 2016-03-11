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
  switch (l->input[l->index]) {
  case '(':
    l->tok = (Token){CharSym, '('};
    break;
  case ')':
    l->tok = (Token){CharSym, ')'};
    break;
  case '[':
    l->tok = (Token){CharSym, '['};
    break;
  case ']':
    l->tok = (Token){CharSym, ']'};
    break;
  case '+':
    l->tok = (Token){CharSym, '+'};
    break;
  case '-':
    l->tok = (Token){CharSym, '-'};
    break;
  case '*':
    l->tok = (Token){CharSym, '*'};
    break;
  case '?':
    l->tok = (Token){CharSym, '?'};
    break;
  case '^':
    l->tok = (Token){CharSym, '^'};
    break;
  case 'n':
    l->tok = (Token){CharSym, '\n'};
    break;
  case '.':
    l->tok = (Token){CharSym, '.'};
    break;
  case '|':
    l->tok = (Token){CharSym, '|'};
    break;
  default:
    l->tok = (Token){Special, l->input[l->index]};
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

  switch (l->input[l->index]) {
  case '(':
    l->tok = (Token){LParen, '('};
    break;
  case ')':
    l->tok = (Token){RParen, ')'};
    break;
  case '[':
    l->tok = (Token){LBracket, '['};
    break;
  case ']':
    l->tok = (Token){RBracket, ']'};
    break;
  case '+':
    l->tok = (Token){Plus, '+'};
    break;
  case '-':
    l->tok = (Token){Minus, '-'};
    break;
  case '*':
    l->tok = (Token){Star, '*'};
    break;
  case '?':
    l->tok = (Token){Question, '?'};
    break;
  case '^':
    l->tok = (Token){Caret, '^'};
    break;
  case '|':
    l->tok = (Token){Pipe, '|'};
    break;
  case '.':
    l->tok = (Token){Dot, '.'};
    break;
  case '\\':
    l->index++;
    escape(l);
    break;
  case '\0':
    l->tok = (Token){Eof, '\0'};
    break;
  default:
    l->tok = (Token){CharSym, l->input[l->index]};
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
