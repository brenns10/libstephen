/***************************************************************************//**

  @file         codegen.c

  @author       Stephen Brennan

  @date         Created Saturday, 30 January 2016

  @brief        Generating code from parse trees.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

  Notes on how code generation works:

  The difficult part of code generation is that you need to generate code with
  "jump" and "split" instructions that point to other instructions.  But since
  the jump and split targets are just (Instr*) pointers, and these frequently
  get moved around or freed during code generation, a naive implementation will
  end up with a bunch of dangling pointers.

  My solution to this issue is to use "Fragments."  Fragments are stored in a
  singly linked list, and they contain an instruction along with an ID.  As code
  is generated, jump and split targets are stored using this ID rather than a
  raw pointer.  Once you have a final Fragment list that contains all of your
  code, it will be turned into an array, and all the IDs will be resolved
  efficiently to locations in the final array using a table.

*******************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

typedef struct Fragment Fragment;
struct Fragment {
  Instr in;
  intptr_t id;
  Fragment *next;
};

typedef struct State State;
struct State {
  intptr_t id; // "global" id counter
  size_t capture; // capture parentheses counter
};

static Fragment *last(Fragment *f)
{
  while (f->next) {
    f = f->next;
  }
  return f;
}

/**
   @brief "Join" a fragment to the given ID.

   This replaces each "match" instruction with a "jump" instruction to the given
   ID.  Since it's assumed that the instruction with that ID will be placed
   after this one, if the last instruction of this fragment is a "match", it
   will just be deleted.
 */
static void join(Fragment *a, Fragment *b)
{
  Fragment *prev = NULL;
  Fragment *l = last(a);
  Instr *lastid;

  // lastid will be set in case the last instruction is a Match, which will be
  // deleted later.
  if (l->in.code == Match) {
    lastid = (Instr*) l->id;
  } else {
    lastid = (Instr*) -1;
  }

  while (a->next != NULL) {
    // Matches should be replaced with jumps to the next fragment.
    if (a->in.code == Match) {
      a->in.code = Jump;
      a->in.x = (Instr*)b->id;
    }
    // Jumps to the final match instruction should instead be targeted to the
    // next fragment.
    if ((a->in.code == Jump || a->in.code == Split) && a->in.x == lastid) {
      a->in.x = (Instr*) b->id;
    }
    if (a->in.code == Split && a->in.y == lastid) {
      a->in.y = (Instr*) b->id;
    }
    prev = a;
    a = a->next;
  }

  // If the last Instruction is a Match, delete it.
  if (prev != NULL && a->in.code == Match) {
    free(a);
    prev->next = b;
  }
}

static size_t fraglen(Fragment *f)
{
  size_t len = 0;
  while (f) {
    len++;
    f = f->next;
  }
  return len;
}

static Fragment *newfrag(enum code code, State *s)
{
  Fragment *new = calloc(1, sizeof(Fragment));
  new->in.code = code;
  new->id = s->id++;
  return new;
}

static void freefraglist(Fragment *f)
{
  Fragment *next;
  while (f) {
    next = f->next;
    free(f);
    f = next;
  }
}

static Fragment *regex(PTree *t, State *s);
static Fragment *term(PTree *t, State *s);
static Fragment *expr(PTree *t, State *s);
static Fragment *class(PTree *t, State *s, bool is_negative);
static Fragment *sub(PTree *t, State *s);

static Fragment *special(char type, State *s)
{
  Fragment *f;

  char whitespace[] = "  \t\t\n\n\r\r\f\f\v\v";
  char word[] = "azAZ09__";
  char number[] = "09";

  switch (type) {
  case 's':
  case 'S':
    f = (type == 's') ? newfrag(Range, s) : newfrag(NRange, s);
    f->in.s = nelem(whitespace) / 2;
    f->in.x = calloc(nelem(whitespace), sizeof(char));
    memcpy(f->in.x, whitespace, nelem(whitespace));
    break;
  case 'w':
  case 'W':
    f = (type == 'w') ? newfrag(Range, s) : newfrag(NRange, s);
    f->in.s = nelem(word) / 2;
    f->in.x = calloc(nelem(word), sizeof(char));
    memcpy(f->in.x, word, nelem(word));
    break;
  case 'd':
  case 'D':
    f = (type == 'd') ? newfrag(Range, s) : newfrag(NRange, s);
    f->in.s = nelem(number) / 2;
    f->in.x = calloc(nelem(number), sizeof(char));
    memcpy(f->in.x, number, nelem(number));
    break;
  default:
    fprintf(stderr, "not implemented: special character class '%c'\n", type);
    exit(EXIT_FAILURE);
    break;
  }

  f->next = newfrag(Match, s);
  return f;
}

static Fragment *term(PTree *t, State *s)
{
  Fragment *f = NULL;

  assert(t->nt == TERMnt);

  if (t->production == 1) {
    if (t->children[0]->tok.sym == CharSym || t->children[0]->tok.sym == Caret
        || t->children[0]->tok.sym == Minus) {
      // Character
      f = newfrag(Char, s);
      f->in.c = t->children[0]->tok.c;
      f->next = newfrag(Match, s);
    } else if (t->children[0]->tok.sym == Dot) {
      // Dot
      f = newfrag(Any, s);
      f->next = newfrag(Match, s);
    } else if (t->children[0]->tok.sym == Special) {
      // Special
      f = special(t->children[0]->tok.c, s);
    }
  } else if (t->production == 2) {
    // Parenthesized expression
    f = newfrag(Save, s);
    f->in.s = s->capture++;
    size_t nextsave = s->capture++;
    f->next = regex(t->children[1], s);
    Fragment *n = newfrag(Save, s);
    n->in.s = nextsave;
    n->next = newfrag(Match, s);
    join(f, n);
  } else {
    // Character class
    f = class(t->children[1], s, (t->production == 4));
  }
  return f;
}

static Fragment *expr(PTree *t, State *s)
{
  Fragment *f = NULL, *a = NULL, *b = NULL, *c = NULL;

  assert(t->nt == EXPRnt);

  f = term(t->children[0], s);
  if (t->nchildren == 1) {
    return f;
  } else {
    if (t->children[1]->tok.sym == Star) {
      /*
        L1:
            split L2 L3   ;; this is "a"  [ non-greedy: split L3 L2 ]
        L2:
            BLOCK from f
            jump L1       ;; this is "b"
        L3:
            match         ;; this is "c"
       */
      a = newfrag(Split, s);
      b = newfrag(Jump, s);
      c = newfrag(Match, s);
      if (t->nchildren == 3) {
        // Non-greedy
        a->in.x = (Instr*) c->id;
        a->in.y = (Instr*) f->id;
      } else {
        // Greedy
        a->in.x = (Instr*) f->id;
        a->in.y = (Instr*) c->id;
      }
      b->in.x = (Instr*) a->id;
      a->next = f;
      b->next = c;
      join(a, b);
      return a;
    } else if (t->children[1]->tok.sym == Plus) {
      /*
        L1:
            BLOCK from f
            split L1 L2   ;; this is "a"  [ non-greedy: split L2 L1 ]
        L2:
            match         ;; this is "b"
       */
      a = newfrag(Split, s);
      b = newfrag(Match, s);
      if (t->nchildren == 3) {
        // Non-greedy
        a->in.x = (Instr*) b->id;
        a->in.y = (Instr*) f->id;
      } else {
        // Greedy
        a->in.x = (Instr*) f->id;
        a->in.y = (Instr*) b->id;
      }
      join(f, a);
      a->next = b;
      return f;
    } else if (t->children[1]->tok.sym == Question) {
      /*
            split L1 L2   ;; this is "a"  [ non-greedy: split L2 L1 ]
        L1:
            BLOCK from f
        L2:
            match         ;; this is "b"
       */
      a = newfrag(Split, s);
      b = newfrag(Match, s);
      if (t->nchildren == 3) {
        // Non-greedy
        a->in.x = (Instr*) b->id;
        a->in.y = (Instr*) f->id;
      } else {
        // Greedy
        a->in.x = (Instr*) f->id;
        a->in.y = (Instr*) b->id;
      }
      a->next = f;
      join(f, b);
      return a;
    } else {
      assert(false);
    }
  }
}

static Fragment *sub(PTree *tree, State *state)
{
  assert(tree->nt == SUBnt);
  Fragment *e = expr(tree->children[0], state);
  if (tree->nchildren == 2) {
    /*
      BLOCK from e
      BLOCK from s
     */
    Fragment *s = sub(tree->children[1], state);
    join(e, s);
  }
  return e;
}

static Fragment *regex(PTree *tree, State *state)
{
  assert(tree->nt == REGEXnt);
  Fragment *s = sub(tree->children[0], state);
  if (tree->nchildren == 3) {
    /*
          split L1 L2     ;; this is "pre"
      L1:
          BLOCK from s
          jump L3         ;; this is "j"
      L2:
          BLOCK from r
      L3:
          match           ;; this is "m"
     */

    Fragment *r = regex(tree->children[2], state);

    Fragment *pre = newfrag(Split, state);
    pre->in.x = (Instr*) s->id;
    pre->in.y = (Instr*) r->id;
    pre->next = s;

    Fragment *m = newfrag(Match, state);
    Fragment *j = newfrag(Jump, state);
    j->in.x = (Instr*) m->id;
    j->next = r;
    join(j, m);
    join(pre, j);

    return pre;
  }
  return s;
}

static Fragment *class(PTree *tree, State *state, bool is_negative)
{
  size_t nranges = 0;
  PTree *curr;
  Fragment *f;

  for (curr = tree; curr->nt == CLASSnt; curr = curr->children[curr->nchildren-1]) {
    nranges++;
  }

  if (is_negative) {
    f = newfrag(NRange, state);
  } else {
    f = newfrag(Range, state);
  }

  f->in.s = nranges;
  f->in.x = calloc(nranges*2, sizeof(char));
  char *block = (char*)f->in.x;

  curr = tree;
  nranges = 0;
  while (curr->nt == CLASSnt) {
    if (curr->production == 1 || curr->production == 2) {
      // Range
      block[2*nranges] = curr->children[0]->tok.c;
      block[2*nranges+1] = curr->children[1]->tok.c;
    } else {
      // Single
      block[2*nranges] = curr->children[0]->tok.c;
      block[2*nranges+1] = curr->children[0]->tok.c;
    }
    curr = curr->children[curr->nchildren-1];
    nranges++;
  }

  f->next = newfrag(Match, state);
  return f;
}

Regex codegen(PTree *tree)
{
  // Generate code.
  State s = {0, 0};
  Fragment *f = regex(tree, &s);
  size_t n;

  // Get the length of the code
  n = fraglen(f);

  // Allocate buffers for the code, and for a lookup table of targets for jumps.
  Instr *code = calloc(n, sizeof(Instr));
  size_t *targets = calloc(s.id, sizeof(size_t));

  // Fill up the lookup table.
  size_t i = 0;
  Fragment *curr;
  for (curr = f; curr; curr = curr->next, i++) {
    targets[curr->id] = i;
  }

  // Now, copy in the Instructions, replacing the jump targets from the table.
  for (curr = f, i = 0; curr; curr = curr->next, i++) {
    code[i] = curr->in;
    if (code[i].code == Jump || code[i].code == Split) {
      code[i].x = code + targets[(intptr_t)code[i].x];
    }
    if (code[i].code == Split) {
      code[i].y = code + targets[(intptr_t)code[i].y];
    }
  }

  free(targets);
  freefraglist(f);
  return (Regex){.n=n, .i=code};
}
