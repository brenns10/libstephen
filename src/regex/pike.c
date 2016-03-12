/***************************************************************************//**

  @file         pike.c

  @author       Stephen Brennan, based on code by Russ Cox:
                https://swtch.com/~rsc/regexp/regexp2.html

  @date         Created Wednesday, 20 January 2016

  @brief        Regex implementation using virtual machines.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

                Portions of this code are based on code by Russ Cox, Copyright
                2007-2009.

*******************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

// Declarations:

typedef struct thread thread;
struct thread {
  Instr *pc;
  size_t *saved;
};

typedef struct thread_list thread_list;
struct thread_list {
  thread *t;
  size_t n;
};

// Printing, for diagnostics

void printthreads(thread_list *tl, Instr *prog, size_t nsave) {
  for (size_t i = 0; i < tl->n; i++) {
    printf("T%zu@pc=%lu{", i, (intptr_t) (tl->t[i].pc - prog));
    for (size_t j = 0; j < nsave; j++) {
      printf("%lu,", tl->t[i].saved[j]);
    }
    printf("} ");
  }
  printf("\n");
}

// Helper evaluation functions for instructions

bool range(Instr in, char test) {
  if (test == '\0') {
    return false;
  }
  bool result = false;
  char *block = (char *) in.x;

  // use in.s for number of ranges, in.x as char* for ranges.
  for (size_t i = 0; i < in.s; i++) {
    if (block[i*2] <= test && test <= block [i*2 + 1]) {
      result = true;
      break; // short circuit yo!
    }
  }

  // negate result for negative ranges
  if (in.code == Range) {
    return result;
  } else {
    return !result;
  }
}

// Pike VM functions:

thread_list newthread_list(size_t n)
{
  thread_list tl;
  tl.t = calloc(n, sizeof(thread));
  tl.n = 0;
  return tl;
}

void addthread(thread_list *threads, Instr *pc, size_t *saved, size_t nsave,
               size_t sp)
{
  //printf("addthread(): pc=%d, saved={%u, %u}, sp=%u, lastidx=%u\n", pc - extprog,
  //       saved[0], saved[1], sp, pc->lastidx);
  if (pc->lastidx == sp) {
    // we've executed this instruction on this string index already
    free(saved);
    return;
  }
  pc->lastidx = sp;

  size_t *newsaved;
  switch (pc->code) {
  case Jump:
    addthread(threads, pc->x, saved, nsave, sp);
    break;
  case Split:
    newsaved = calloc(nsave, sizeof(size_t));
    memcpy(newsaved, saved, nsave * sizeof(size_t));
    addthread(threads, pc->x, saved, nsave, sp);
    addthread(threads, pc->y, newsaved, nsave, sp);
    break;
  case Save:
    saved[pc->s] = sp;
    addthread(threads, pc + 1, saved, nsave, sp);
    break;
  default:
    threads->t[threads->n].pc = pc;
    threads->t[threads->n].saved = saved;
    threads->n++;
    break;
  }
}

/**
   @brief "Stash" a list of captures into the "out" pointer.
   @param new The new list of captures encountered by the Match instruction.
   @param destination The out pointer where the caller wants the captures.
 */
void stash(size_t *new, size_t **destination)
{
  if (!destination) {
    /* If the user wants to discard the captures, they'll pass NULL.  This means
       we need to get rid of the capture list, or we'll leak the memory. */
    free(new);
    return;
  }
  if (*destination) {
    /* If we have already stored a capture list, we should free that. */
    free(*destination);
  }
  /* Finally, stash the pointer away. */
  *destination = new;
}

ssize_t execute(Regex r, char *input, size_t **saved)
{
  // Can have at most n threads, where n is the length of the program.  This is
  // because (as it is now) the thread state is simply a program counter.
  thread_list curr = newthread_list(r.n);
  thread_list next = newthread_list(r.n);
  thread_list temp;
  size_t nsave = 0;
  ssize_t match = -1;

  // Set the out pointer to NULL so that stash() knows whether we've already
  // stashed away a capture list.
  if (saved) {
    *saved = NULL;
  }

  // Need to initialize lastidx to something that will never be used.
  for (size_t i = 0; i < r.n; i++) {
    r.i[i].lastidx = (size_t)-1;
    if (r.i[i].code == Save) {
      nsave++;
    }
  }

  // Start with a single thread and add more as we need.  Note that addthread()
  // will execute instructions that don't consume input (i.e. epsilon closure).
  addthread(&curr, r.i, calloc(nsave, sizeof(size_t)), nsave, 0);

  size_t sp;
  for (sp = 0; curr.n > 0; sp++) {

    //printf("consider input %c\nthreads: ", input[sp]);
    //printthreads(&curr, r.i, nsave);

    // Execute each thread (this will only ever reach instructions that consume
    // input, since addthread() stops with those).
    for (size_t t = 0; t < curr.n; t++) {
      Instr *pc = curr.t[t].pc;

      switch (pc->code) {
      case Char:
        if (input[sp] != pc->c) {
          free(curr.t[t].saved);
          break; // fail, don't continue executing this thread
        }
        // add thread containing the next instruction to the next thread list.
        addthread(&next, pc+1, curr.t[t].saved, nsave, sp+1);
        break;
      case Any:
        if (input[sp] == '\0') {
          free(curr.t[t].saved);
          break; // dot can't match end of string!
        }
        // add thread containing the next instruction to the next thread list.
        addthread(&next, pc+1, curr.t[t].saved, nsave, sp+1);
        break;
      case Range:
      case NRange:
        if (!range(*pc, input[sp])) {
          free(curr.t[t].saved);
          break;
        }
        addthread(&next, pc+1, curr.t[t].saved, nsave, sp+1);
        break;
      case Match:
        stash(curr.t[t].saved, saved);
        match = sp;
        goto cont;
      default:
        assert(false);
        break;
      }
    }

  cont:
    // Swap the curr and next lists.
    temp = curr;
    curr = next;
    next = temp;

    // Reset our new next list.
    next.n = 0;
  }

  free(curr.t);
  free(next.t);
  return match;
}

int numsaves(Regex r)
{
  int ns = 0;
  for (size_t i = 0; i < r.n; i++) {
    if (r.i[i].code == Save && r.i[i].s > ns) {
      ns = r.i[i].s;
    }
  }
  return ns + 1;
}
