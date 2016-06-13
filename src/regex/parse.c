/***************************************************************************//**

  @file         parse.c

  @author       Stephen Brennan

  @date         Created Saturday, 23 January 2016

  @brief        Recursive descent regex parser!  (modeled after Wikipedia)

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

char *names[] = {
  "CharSym", "Special", "Eof", "LParen", "RParen", "LBracket", "RBracket",
  "Plus", "Minus", "Star", "Question", "Caret", "Pipe", "Dot"
};

char *ntnames[] = {
  "TERM", "EXPR", "REGEX", "CLASS", "SUB"
};

/*
  Convenience functions for parse trees.
 */

static PTree *terminal_tree(Token tok)
{
  PTree *tree = calloc(1, sizeof(PTree));
  tree->nchildren = 0;
  tree->production = 0; // marks this as terminal
  tree->tok = tok;
  return tree;
}

static PTree *nonterminal_tree(NTSym nt, size_t nchildren)
{
  PTree *tree = calloc(1, sizeof(PTree));
  tree->nchildren = nchildren;
  tree->production = 1; // update this on return.
  tree->nt = nt;
  return tree;
}

void free_tree(PTree *tree)
{
  for (size_t i = 0; i < tree->nchildren; i++) {
    free_tree(tree->children[i]);
  }
  free(tree);
}

/*
  Simple convenience functions for a parser.
 */

static bool accept(TSym s, Lexer *l)
{
  if (l->tok.sym == s) {
    nextsym(l);
    return true;
  }
  return false;
}

static void expect(TSym s, Lexer *l)
{
  if (l->tok.sym == s) {
    nextsym(l);
    return;
  }
  fprintf(stderr, "error: expected %s, got %s\n", names[s], names[l->tok.sym]);
  exit(1);
}

PTree *TERM(Lexer *l)
{
  if (accept(CharSym, l) || accept(Dot, l) || accept(Special, l) ||
      accept(Caret, l) || accept(Minus, l)) {
    PTree *result = nonterminal_tree(TERMnt, 1);
    result->children[0] = terminal_tree(l->prev);
    result->production = 1;
    return result;
  } else if (accept(LParen, l)) {
    PTree *result = nonterminal_tree(TERMnt, 3);
    result->children[0] = terminal_tree(l->prev);
    result->children[1] = REGEX(l);
    expect(RParen, l);
    result->children[2] = terminal_tree(l->prev);
    result->production = 2;
    return result;
  } else if (accept(LBracket, l)) {
    PTree *result;
    if (accept(Caret, l)) {
      result = nonterminal_tree(TERMnt, 3);
      result->children[0] = terminal_tree((Token){LBracket, '['});
      result->children[1] = CLASS(l);
      expect(RBracket, l);
      result->children[2] = terminal_tree(l->prev);
      result->production = 4;
    } else {
      result = nonterminal_tree(TERMnt, 3);
      result->children[0] = terminal_tree((Token){LBracket, '['});
      result->children[1] = CLASS(l);
      expect(RBracket, l);
      result->children[2] = terminal_tree(l->prev);
      result->production = 3;
    }
    return result;
  } else {
    fprintf(stderr, "error: TERM: syntax error\n");
    exit(1);
  }
}

PTree *EXPR(Lexer *l)
{
  PTree *result = nonterminal_tree(EXPRnt, 1);
  result->children[0] = TERM(l);
  if (accept(Plus, l) || accept(Star, l) || accept(Question, l)) {
    result->nchildren++;
    result->children[1] = terminal_tree(l->prev);
    if (accept(Question, l)) {
      result->nchildren++;
      result->children[2] = terminal_tree((Token){Question, '?'});
    }
  }
  return result;
}

PTree *SUB(Lexer *l)
{
  PTree *result = nonterminal_tree(SUBnt, 1);
  PTree *orig = result, *prev = result;

  while (l->tok.sym != Eof && l->tok.sym != RParen && l->tok.sym != Pipe) { // seems like a bit of a hack
    result->children[0] = EXPR(l);
    result->children[1] = nonterminal_tree(SUBnt, 0);
    result->nchildren = 2;
    prev = result;
    result = result->children[1];
  }

  // This prevents SUB nonterminals with no children in the final parse tree.
  if (prev != result) {
    prev->nchildren = 1;
    free(result);
  }
  return orig;
}

PTree *REGEX(Lexer *l)
{
  PTree *result = nonterminal_tree(REGEXnt, 1);
  result->children[0] = SUB(l);

  if (accept(Pipe, l)) {
    result->nchildren = 3;
    result->children[1] = terminal_tree(l->prev);
    result->children[2] = REGEX(l);
  }
  return result;
}

bool CCHAR(Lexer *l)
{
  TSym acceptable[] = {CharSym, Dot, LParen, RParen, Plus, Star, Question, Pipe};
  for (size_t i = 0; i < nelem(acceptable); i++) {
    if (accept(acceptable[i], l)) {
      l->prev.sym = CharSym;
      return true;
    }
  }
  return false;
}

PTree *CLASS(Lexer *l)
{
  PTree *result = nonterminal_tree(CLASSnt, 0), *curr, *prev;
  Token t1, t2, t3;
  curr = result;

  while (true) {
    if (CCHAR(l)) {
      prev = curr;
      t1 = l->prev;
      if (accept(Minus, l)) {
        t2 = l->prev;
        if (CCHAR(l)) {
          t3 = l->prev;
          // We have ourselves a range!  Parse it.
          curr->children[0] = terminal_tree(t1);
          curr->children[1] = terminal_tree(t3);
          curr->children[2] = nonterminal_tree(CLASSnt, 0);
          curr->nchildren = 3;
          curr->production = 1;
          curr = curr->children[2];
        } else {
          // character followed by minus, but not range.
          unget(t2, l);
          curr->children[0] = terminal_tree(t1);
          curr->children[1] = nonterminal_tree(CLASSnt, 0);
          curr->nchildren = 2;
          curr->production = 3;
          curr = curr->children[1];
        }
      } else {
        // just a character
        curr->children[0] = terminal_tree(t1);
        curr->children[1] = nonterminal_tree(CLASSnt, 0);
        curr->nchildren = 2;
        curr->production = 3;
        curr = curr->children[1];
      }
    } else if (accept(Minus, l)) {
      // just a minus
      prev = curr;
      curr->children[0] = terminal_tree(l->prev);
      curr->nchildren = 1;
      curr->production = 5;
      break;
    } else {
      free(curr);
      prev->nchildren--;
      prev->production++;
      break;
    }
  }
  return result;
}

static PTree *reparse_internal(struct Input input)
{
  Lexer l;

  // Initialize the lexer.
  l.input = input;
  l.index = 0;
  l.nbuf = 0;
  l.tok = (Token){.sym=0, .c=0};

  // Create a parse tree!
  //printf(";; TOKENS:\n");
  nextsym(&l);
  PTree *tree = REGEX(&l);
  expect(Eof, &l);

  return tree;
}

PTree *reparse(const char *input)
{
  struct Input in = {.str=input, .wstr=NULL};
  return reparse_internal(in);
}

PTree *reparsew(const wchar_t *winput)
{
  struct Input in = {.str=NULL, .wstr=winput};
  return reparse_internal(in);
}

Regex recomp(const char *regex)
{
  PTree *tree = reparse(regex);
  Regex code = codegen(tree);
  free_tree(tree);
  return code;
}

Regex recompw(const wchar_t *regex)
{
  PTree *tree = reparsew(regex);
  Regex code = codegen(tree);
  free_tree(tree);
  return code;
}
