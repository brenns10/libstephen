/***************************************************************************//**

  @file         regparse.h

  @author       Stephen Brennan

  @date         Created Friday, 29 January 2016

  @brief        Private lex/parse/codegen declarations.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_REGEX_REGPARSE_H
#define SMB_REGEX_REGPARSE_H

#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>

#include "re.h"

enum code {
  Char, Match, Jump, Split, Save, Any, Range, NRange
};

struct Instr {
  enum code code; // opcode
  wchar_t c;      // character
  size_t s;       // slot for "saving" a string index
  Instr *x, *y;   // targets for jump and split
  size_t lastidx; // used by Pike VM for fast membership testing
};

/**
   @brief Types of terminal symbols!
 */
enum TSym {
  CharSym, Special, Eof, LParen, RParen, LBracket, RBracket, Plus, Minus,
  Star, Question, Caret, Pipe, Dot
};
typedef enum TSym TSym;

// Lookup the name of a terminal symbol.
extern char *names[];

/**
   @brief Types of non-terminal symbols!
 */
enum NTSym {
  TERMnt, EXPRnt, REGEXnt, CLASSnt, SUBnt
};
typedef enum NTSym NTSym;

// Lookup the name of a non-terminal symbol.
extern char *ntnames[];

/**
   @brief Tokens are simply a combination of a symbol type and a character.

   This means that you can associate the CharSym with the particular character,
   and you can tell the difference between different Special's (like \b, \w, \s,
   etc).  The character field may be useless for symbols like LParen.  A token
   goes into a parse tree, and should contain all the information necessary for
   code generation from a parse tree.
 */
typedef struct Token Token;
struct Token {
  TSym sym;
  wchar_t c;
};

/**
   @brief Data structure that allows for simple swapping of input types.

   This data structure allows functions to be written to not care whether they
   are receiving wide character strings or "narrow" (or ascii, aka naive)
   strings.  Which is useful.
*/
struct Input {
  const char *str;
  const wchar_t *wstr;
};

/**
   @brief Read input from an existing index, regardless of string type.
*/
wchar_t InputIdx(struct Input in, size_t idx);

/**
   @brief Tree data structure to store information parsed out of a regex.
 */
typedef struct PTree PTree;
struct PTree {
  unsigned short nchildren; // a nonterminal symbol may have no children
  unsigned short production; // 0 -> terminal, anything else -> nonterminal

  NTSym nt;
  Token tok;

  struct PTree *children[4];
};

#define LEXER_BUFSIZE 4
/**
   @brief Data structure containing lexer information.
 */
typedef struct Lexer Lexer;
struct Lexer {
  struct Input input;
  size_t index;
  Token tok, prev;
  Token buf[LEXER_BUFSIZE];
  size_t nbuf;
};

/* Lexing */
void escape(Lexer *l);
Token nextsym(Lexer *l);
void unget(Token t, Lexer *l);
Regex codegen(PTree *tree);

/* Parsing */
PTree *TERM(Lexer *l);
PTree *EXPR(Lexer *l);
PTree *REGEX(Lexer *l);
PTree *CLASS(Lexer *l);
PTree *SUB(Lexer *l);
PTree *reparse(const char *regex);
PTree *reparsew(const wchar_t *winput);

/* Utitlites */
void free_tree(PTree *tree);
char *char_to_string(char c);

#endif // SMB_REGEX_REGPARSE_H
