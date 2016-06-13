/***************************************************************************//**

  @file         instr.c

  @author       Stephen Brennan

  @date         Created Thursday, 21 January 2016

  @brief        Functions for reading and writing virtual machine instructions.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

#define COMMENT ';'

/*
  Declarations for parsing.
 */

enum linetype {
  Blank, Label, Code
};
typedef enum linetype linetype;

char *Opcodes[] = {
  "char", "match", "jump", "split", "save", "any", "range", "nrange"
};

/*
  Utilities for input/output
 */
char *char_to_string(char c)
{
  #define CTS_BUFSIZE 5
  static char buffer[CTS_BUFSIZE];
  if (c == ' ') {
    buffer[0] = '\\';
    buffer[1] = 'x';
    buffer[2] = '2';
    buffer[3] = '0';
    buffer[4] = '\0'; // space = 0x20
  } else if (isspace(c)) {
    switch (c) {
    case '\n':
      buffer[1] = 'n';
      break;
    case '\f':
      buffer[1] = 'n';
      break;
    case '\r':
      buffer[1]= 'r';
      break;
    case '\t':
      buffer[1] = 't';
      break;
    case '\v':
      buffer[1] = 'v';
      break;
    }
    buffer[0] = '\\';
    buffer[2] = '\0';
  } else if (c == '\0') {
    buffer[0] = '\0'; // empty string...
  } else {
    buffer[0] = c;
    buffer[1] = '\0';
  }
  return buffer;
}

void fprintc(FILE *f, char c)
{
  fprintf(f, "%s", char_to_string(c));
}

char string_to_char(char *s) {
  unsigned int c;
  if (s[0] == '\\') {
    switch (s[1]) {
    case 'n':
      return '\n';
    case 'f':
      return '\f';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case 'x':
      sscanf(s + 2, "%x", &c);
      return (char)c;
    default:
      return s[1];
    }
  } else {
    return s[0];
  }
}


/**
   @brief Remove leading and trailing whitespace and comments from a line.
 */
static char *trim(char *line, char *lastchar_out)
{
  ssize_t firstnonwhite = -1;
  ssize_t lastnonwhite = -1;
  size_t i;

  for (i = 0; line[i]; i++) {
    if (isspace(line[i])) {
      continue;
    }

    if (line[i] == COMMENT) {
      line[i] = '\0';
      break;
    }

    if (firstnonwhite == -1) {
      firstnonwhite = i;
    }
    lastnonwhite = i;
  }

  if (firstnonwhite == -1) { // all whitespace
    *lastchar_out = '\0';
    return line + i;
  } else {
    line[lastnonwhite + 1] = '\0';
    *lastchar_out = line[lastnonwhite];
    return line + firstnonwhite;
  }
}

static char **tokenize(char *line, size_t *ntok)
{
  #define SEP " \n\t\v\f"
  size_t alloc = 16;
  char **buf = calloc(alloc, sizeof(char*));

  buf[0] = strtok(line, SEP);
  *ntok = 0;
  while (buf[*ntok] != NULL) {
    *ntok += 1;

    if (*ntok >= alloc) {
      alloc *= 2;
      buf = realloc(buf, alloc * sizeof(char*));
    }

    buf[*ntok] = strtok(NULL, SEP);
  }
  return buf;
}

/**
   @brief Parse and return an instruction from a line.

   Input line should have no preceding or trailing whitespace (see trim()).
   This function will not return on error - it will just fprintf() the error
   message and exit with an error code.  This will change eventually.
   @param line Text to parse.
   @param lineno Line number (just used for error msg).
 */
static Instr read_instr(char *line, int lineno)
{
  // First, we're going to tokenize the string into a statically allocated
  // buffer.  We know we don't need more than like TODO
  size_t ntok;
  char **tokens = tokenize(line, &ntok);
  Instr inst = {.code=0, .c=0, .s=0, .x=NULL, .y=NULL, .lastidx=0};

  if (strcmp(tokens[0], Opcodes[Char]) == 0) {
    if (ntok != 2) {
      fprintf(stderr, "line %d: require 2 tokens for char\n", lineno);
      exit(1);
    }
    inst.code = Char;
    inst.c = tokens[1][0];
  } else if (strcmp(tokens[0], Opcodes[Match]) == 0) {
    if (ntok != 1) {
      fprintf(stderr, "line %d: require 1 token for match\n", lineno);
      exit(1);
    }
    inst.code = Match;
  } else if (strcmp(tokens[0], Opcodes[Jump]) == 0) {
    if (ntok != 2) {
      fprintf(stderr, "line %d: require 2 tokens for jump\n", lineno);
      exit(1);
    }
    inst.code = Jump;
    inst.x = (Instr*)tokens[1];
  } else if (strcmp(tokens[0], Opcodes[Split]) == 0) {
    if (ntok != 3) {
      fprintf(stderr, "line %d: require 3 tokens for split\n", lineno);
      exit(1);
    }
    inst.code = Split;
    inst.x = (Instr*)tokens[1];
    inst.y = (Instr*)tokens[2];
  } else if (strcmp(tokens[0], Opcodes[Save]) == 0) {
    if (ntok != 2) {
      fprintf(stderr, "line %d: require 2 tokens for save\n", lineno);
      exit(1);
    }
    inst.code = Save;
    sscanf(tokens[1], "%zu", &inst.s);
  } else if (strcmp(tokens[0], Opcodes[Any]) == 0) {
    if (ntok != 1) {
      fprintf(stderr, "line %d: require 1 token for any\n", lineno);
      exit(1);
    }
    inst.code = Any;
  } else if (strcmp(tokens[0], Opcodes[ Range]) == 0 ||
             strcmp(tokens[0], Opcodes[NRange]) == 0) {
    if (ntok % 2 == 0) {
      fprintf(stderr, "line %d, require even number of character tokens\n",
              lineno);
      exit(1);
    }
    inst.code = (strcmp(tokens[0], Opcodes[Range]) == 0) ? Range : NRange;
    inst.s = (size_t) (ntok - 1) / 2;
    char *block = calloc(ntok - 1, sizeof(char));
    inst.x = (Instr*)block;
    for (size_t i = 0; i < ntok - 1; i++) {
      block[i] = string_to_char(tokens[i+1]);
    }
  } else {
    fprintf(stderr, "line %d: unknown opcode \"%s\"\n", lineno, tokens[0]);
  }
  return inst;
}

/**
   @brief Return the instruction index corresponding to a label.
   @param labels Array of labels
   @param lablindices Corresponding indices for the labels
   @param nlabels Number of label/index pairs in the arrays
   @param label Label to search for
   @param line Line number (for error messages)
 */
static size_t gettarget(char **labels, size_t *labelindices, size_t nlabels,
                        char *label, size_t line)
{
  for (size_t i = 0; i < nlabels; i++) {
    if (strcmp(labels[i], label) == 0) {
      return labelindices[i];
    }
  }
  fprintf(stderr, "line %zu: label \"%s\" not found\n", line, label);
  exit(1);
}

/**
   @brief Return a block of instructions from some text code.
   @param str Code
   @param[out] ninstr Where to put the number of instructions
 */
Regex reread(char *str)
{
  size_t nlines = 1;
  Instr *rv = NULL;

  // Count newlines.
  for (size_t i = 0; str[i]; i++) {
    if (str[i] == '\n') {
      nlines++;
    }
  }

  // Create an array of lines.
  char **lines = calloc(nlines, sizeof(char*));
  size_t j = 0;
  lines[j++] = str;
  for (size_t i = 0; str[i]; i++) {
    if (str[i] == '\n') {
      str[i] = '\0';
      lines[j++] = str + i + 1;
    }
  }

  // Trim whitespace of each line, and categorize the lines.
  size_t nlabels = 0;
  size_t ncode = 0;
  linetype *types = calloc(nlines, sizeof(linetype));
  for (size_t i = 0; i < nlines; i++) {
    char last;
    lines[i] = trim(lines[i], &last);
    if (last == '\0') {
      types[i] = Blank;
    } else if (last == ':') {
      types[i] = Label;
      nlabels++;
    } else {
      types[i] = Code;
      ncode++;
    }
  }

  // Associate each label with the next line of code.
  char **labels = calloc(nlabels, sizeof(char *));
  size_t *labelindices = calloc(nlabels, sizeof(size_t));
  ssize_t codeidx = -1;
  size_t labelidx = 0;
  for (size_t i = 0; i < nlines; i++) {
    if (types[i] == Label) {
      size_t len = strlen(lines[i]);
      lines[i][len-1] = '\0'; // remove the colon, we don't want it
      labels[labelidx] = lines[i];
      labelindices[labelidx] = codeidx + 1;
      labelidx++;
    } else if (types[i] == Code) {
      codeidx++;
    }
  }
  // we'll assume that the labels are valid for now

  rv = calloc(ncode, sizeof(Instr));
  codeidx = 0;
  for (size_t i = 0; i < nlines; i++) {
    if (types[i] != Code) {
      continue;
    }

    rv[codeidx] = read_instr(lines[i], i+1);

    // lookup labels and point them correctly
    if (rv[codeidx].code == Jump || rv[codeidx].code == Split) {
      rv[codeidx].x = rv + gettarget(labels, labelindices, nlabels, (char*)rv[codeidx].x, i+1);
    }
    if (rv[codeidx].code == Split) {
      rv[codeidx].y = rv + gettarget(labels, labelindices, nlabels, (char*)rv[codeidx].y, i+1);
    }

    codeidx++;
  }

  free(types);
  free(lines);
  free(labels);
  free(labelindices);
  return (Regex){.n=codeidx, .i=rv};
}

/**
   @brief Read a program from a file.
 */
Regex refread(FILE *f) {
  size_t alloc = 4096;
  char *buf = malloc(alloc);
  size_t start = 0;
  while (true) {
    start += fread(buf, 1, alloc - start, f);
    if (start == alloc) {
      alloc *= 2;
      buf = realloc(buf, alloc);
    } else {
      break;
    }
  }

  Regex rv = reread(buf);
  free(buf);
  return rv;
}

/**
   @brief Write a program to a file.
 */
void rewrite(Regex r, FILE *f)
{
  size_t *labels = calloc(r.n, sizeof(size_t));

  // Find every instruction that needs a label.
  for (size_t i = 0; i < r.n; i++) {
    if (r.i[i].code == Jump || r.i[i].code == Split) {
      labels[r.i[i].x - r.i] = 1;
    }
    if (r.i[i].code == Split) {
      labels[r.i[i].y - r.i] = 1;
    }
  }

  size_t l = 1;
  for (size_t i = 0; i < r.n; i++) {
    if (labels[i] > 0) {
      labels[i] = l++;
    }
  }

  for (size_t i = 0; i < r.n; i++) {
    if (labels[i] > 0) {
      fprintf(f, "L%zu:\n", labels[i]);
    }
    char *block = (char*) r.i[i].x;
    switch (r.i[i].code) {
    case Char:
      fprintf(f, "    char %s\n", char_to_string(r.i[i].c));
      break;
    case Match:
      fprintf(f, "    match\n");
      break;
    case Jump:
      fprintf(f, "    jump L%zu\n", labels[r.i[i].x - r.i]);
      break;
    case Split:
      fprintf(f, "    split L%zu L%zu\n", labels[r.i[i].x - r.i],
              labels[r.i[i].y - r.i]);
      break;
    case Save:
      fprintf(f, "    save %zu\n", r.i[i].s);
      break;
    case Any:
      fprintf(f, "    any\n");
      break;
    case NRange:
      fprintf(f, "    nrange");
      for (size_t j = 0; j < r.i[i].s; j++) {
        fprintf(f, " %s", char_to_string(block[2*j]));
        fprintf(f, " %s", char_to_string(block[2*j + 1]));
      }
      fprintf(f, "\n");
      break;
    case Range:
      fprintf(f, "    range");
      for (size_t j = 0; j < r.i[i].s; j++) {
        fprintf(f, " %s", char_to_string(block[2*j]));
        fprintf(f, " %s", char_to_string(block[2*j + 1]));
      }
      fprintf(f, "\n");
      break;
    }
  }

  free(labels);
}

void refree(Regex r)
{
  for (size_t i = 0; i < r.n; i++) {
    if (r.i[i].code == Range || r.i[i].code == NRange) {
      free(r.i[i].x);
    }
  }
  free(r.i);
}
