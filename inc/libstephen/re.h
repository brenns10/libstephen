/***************************************************************************//**

  @file         regex.h

  @author       Stephen Brennan

  @date         Created Thursday, 21 January 2016

  @brief        Declarations of data structures for regex and vm bytecode.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_PIKE_REGEX_H
#define SMB_PIKE_REGEX_H

#include <stdio.h>
#include <unistd.h>

// DEFINITIONS

enum code {
  Char, Match, Jump, Split, Save, Any, Range, NRange
};

typedef struct instr instr;
struct instr {
  enum code code; // opcode
  char c;         // character
  size_t s;       // slot for "saving" a string index
  instr *x, *y;   // targets for jump and split
  size_t lastidx; // used by Pike VM for fast membership testing
};

// Read/Write Programs
instr *read_prog(char *str, size_t *ninstr);
instr *fread_prog(FILE *f, size_t *ninstr);
void write_prog(instr *prog, size_t n, FILE *f);
void free_prog(instr *prog, size_t n);

// parser.c
instr *recomp(char *regex, size_t *n);

// pike.c
ssize_t execute(instr *prog, size_t proglen, char *input, size_t **saved);
int numsaves(instr *code, size_t ncode);

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

#endif // SMB_PIKE_REGEX_H
