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

/**
   An instruction for the virtual machine.  The details of this struct aren't
   important for client code, but a declaration is necessary for defining the
   Regex type.
 */
typedef struct Instr Instr;
struct Instr;

/**
   This typedef is for convenience.  See the documentation for struct Regex.
 */
typedef struct Regex Regex;
/**
   Represents a compiled regular expression.  This consists of a pointer to the
   instructions, and a counter for how many instructions there are.  This is not
   a large struct, so you shouldn't pass around pointers to it.  You might as
   well just pass around copies of this struct.
 */
struct Regex {
  /**
     Number of instructions.
   */
  size_t n;
  /**
     Pointer to instruction buffer.
   */
  Instr *i;
};

/**
   Read in a program from a string.  This takes the "assembly like"
   representation and turns it into compiled instructions.  Every instruction
   must be on a single line, and spaces are used as delimiters.  Also, labels
   must be on their own lines.  Here is an example of code in this format:

           save 0
       L1:
           char a
           split L1 L2
       L2:
           match

   @param str The text representation of the program.
   @returns The bytecode of the program.
 */
Regex read_prog(char *str);
/**
   Reads in a program from a file instead of a string.
   @param f File to read from.
   @returns The regex bytecode.
 */
Regex fread_prog(FILE *f);
/**
   Writes a program to the same format as the read_prog() functions do.
   @param r The regex to write.
   @param f The file to write to.
 */
void write_prog(Regex r, FILE *f);
/**
   Free a Regex object.  You must do this when you're done with it.
   @param r Regex to free.
 */
void free_prog(Regex r);

/**
   Compile a regular expression!
   @param regex The text form of the regular expression.
   @returns The compiled bytecode for the regex.
 */
Regex recomp(char *regex);


/**
   Execute a regex on a string.
   @param r Compiled regular expression bytecode to execute.
   @param input Text to use as input.
   @param saved Out pointer for captured indices.
   @returns Length of match, or -1 if no match.
 */
ssize_t execute(Regex r, char *input, size_t **saved);
/**
   Return the number of saved index slots required by a regex.
   @param r The regular expression bytecode.
   @returns Number of slots.
 */
size_t numsaves(Regex r);

/**
   Macro for the number of elements of a statically allocated array.
 */
#define nelem(x) (sizeof(x)/sizeof((x)[0]))

#endif // SMB_PIKE_REGEX_H
