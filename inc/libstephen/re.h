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

struct Instr;
typedef struct Instr Instr;

typedef struct Regex Regex;
struct Regex {
  size_t n;
  Instr *i;
};

// Read/Write Programs
Regex read_prog(char *str);
Regex fread_prog(FILE *f);
void write_prog(Regex r, FILE *f);
void free_prog(Regex r);

// parser.c
Regex recomp(char *regex);

// pike.c
ssize_t execute(Regex r, char *input, size_t **saved);
int numsaves(Regex r);

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

#endif // SMB_PIKE_REGEX_H
