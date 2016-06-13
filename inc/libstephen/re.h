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
#include <wchar.h>

// DEFINITIONS

/// @cond HIDDEN_SYMBOLS
/**
   An instruction for the virtual machine.  The details of this struct aren't
   important for client code, but a declaration is necessary for defining the
   Regex type.
 */
typedef struct Instr Instr;
struct Instr;
/// @endcond HIDDEN_SYMBOLS

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
   A convenience data structure for getting copies of captured strings.

   The recomp() function returns an array of start and end points for captured
   strings, which is technically all you need. Functionally, though, you would
   like a slightly more convenient way to access your captures. So, you can use
   the recap() to convert the string and capture list to a list of freshly
   allocated strings. Be sure to call recapfree() on the capture list when
   you're done (or manually clean up).
 */
typedef struct {
  /**
     The number of captured strings.
   */
  size_t n;
  /**
     An array of length n captured strings. Each one is independently allocated,
     so you'll need to free them when you're done.
   */
  char **cap;

} Captures;

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
Regex recomp(const char *regex);

/**
   Compile a wide regular expression!
   @param regex The text form of the regular expression.
   @returns The compiled bytecode for the regex.
*/
Regex recompw(const wchar_t *regex);

/**
   Execute a regex on a string.
   @param r Compiled regular expression bytecode to execute.
   @param input Text to use as input.
   @param saved Out pointer for captured indices.
   @returns Length of match, or -1 if no match.
 */
ssize_t reexec(Regex r, const char *input, size_t **saved);
/**
   Execute a regex on a string.
   @param r Compiled regular expression bytecode to execute.
   @param input Text to use as input.
   @param saved Out pointer for captured indices.
   @returns Length of match, or -1 if no match.
*/
ssize_t reexecw(Regex r, const wchar_t *input, size_t **saved);
/**
   Return the number of saved index slots required by a regex.
   @param r The regular expression bytecode.
   @returns Number of slots.
 */
size_t numsaves(Regex r);
/**
   Convert a string and a capture list into a list of strings.

   This copies each capture into a newly allocated string, and returns them all
   in a newly allocated array of strings. These things need to be freed when
   you're done with them. You can either manually free each string and then the
   array, or you can use recapfree() to do this for you.

   @param s String to get strings from.
   @param l List of captures returned from reexec().
   @param n Number of saves - use numsaves() if you don't know.
   @returns A new Capture object.
*/
Captures recap(const char *s, const size_t *l, size_t n);
/**
   Free a capture list from recap()

   Since the array and strings were all newly allocated by recap(), they need to
   be cleaned up. This function does the cleanup. It's nothing complicated - you
   can do it yourself, but it's convenient to have this to do it for you. Note
   that if you want to keep one of the strings from the capture list, you'll
   have to set its entry in the array to NULL (so free() does nothing), or else
   do manual cleanup.

   @param c Captures to free.
 */
void recapfree(Captures c);


/**
   Macro for the number of elements of a statically allocated array.
 */
#define nelem(x) (sizeof(x)/sizeof((x)[0]))

#endif // SMB_PIKE_REGEX_H
