/***************************************************************************//**

  @file         regex/parse.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Regular expression parsing functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdbool.h>          // true, false
#include <assert.h>           // assert()

#include "libstephen/regex.h" // functions we're implementing
#include "libstephen/fsm.h"   // for fsm operations
#include "libstephen/ll.h"    // for linked lists
#include "str.h"              // for get_escape

/**
   @brief Adjust the FSM according to its modifier, if any.

   When a character, character class, or parenthesized regex is read in, it
   could be followed by the modifiers `*`, `+`, or `?`.  This function adjusts
   the FSM for those modifiers, and adjusts the location pointer if one was
   present.

   @param new The newly read in FSM.
   @param regex The pointer to the pointer to the location in the regex.
 */
void regex_parse_check_modifier(fsm *new, const wchar_t **regex)
{
  fsm *f;

  switch ((*regex)[1]) {

  case L'*':
    fsm_kleene(new);
    (*regex)++;
    break;

  case L'+':
    f = fsm_copy(new);
    fsm_kleene(f);
    fsm_concat(new, f);
    fsm_delete(f, true);
    (*regex)++;
    break;

  case L'?':
    // Create the machine that accepts the empty string
    f = fsm_create();
    f->start = fsm_add_state(f, true);
    fsm_union(new, f);
    fsm_delete(f, true);
    (*regex)++;
    break;

  /* case L'{': */
  /*   // A  */
  /*   (*regex)++; */
  }
}

/**
   @brief Return a FSM that accepts any whitespace.
   @param type Positive or negative.
 */
fsm *regex_parse_create_whitespace_fsm(int type)
{
  fsm *f = fsm_create();
  int src = fsm_add_state(f, false);
  int dest = fsm_add_state(f, true);
  fsm_trans *ft = fsm_trans_create(6, type, dest);
  ft->start[0] = L' ';
  ft->end[0] = L' ';
  ft->start[1] = L'\f';
  ft->end[1] = L'\f';
  ft->start[2] = L'\n';
  ft->end[2] = L'\n';
  ft->start[3] = L'\r';
  ft->end[3] = L'\r';
  ft->start[4] = L'\t';
  ft->end[4] = L'\t';
  ft->start[5] = L'\v';
  ft->end[5] = L'\v';
  fsm_add_trans(f, src, ft);
  f->start = src;
  return f;
}

/**
   @brief Return a FSM for word characters (letters, numbers, and underscore).
 */
fsm *regex_parse_create_word_fsm(int type)
{
  fsm *f = fsm_create();
  int src = fsm_add_state(f, false);
  int dest = fsm_add_state(f, true);
  fsm_trans *ft = fsm_trans_create(4, type, dest);
  ft->start[0] = L'a';
  ft->end[0] = L'z';
  ft->start[1] = L'A';
  ft->end[1] = L'Z';
  ft->start[2] = L'_';
  ft->end[2] = L'_';
  ft->start[3] = L'0';
  ft->end[3] = L'9';
  fsm_add_trans(f, src, ft);
  f->start = src;
  return f;
}

/**
   @brief Return a FSM for digits.
 */
fsm *regex_parse_create_digit_fsm(int type)
{
  fsm *f = fsm_create();
  int src = fsm_add_state(f, false);
  int dest = fsm_add_state(f, true);
  fsm_trans *ft = fsm_trans_create(1, type, dest);
  ft->start[0] = L'0';
  ft->end[0] = L'9';
  fsm_add_trans(f, src, ft);
  f->start = src;
  return f;
}

/**
   @brief Create the 'dot' FSM, i.e. the FSM that accepts 'almost' anything.

   The dot should accept 'almost' anything.  The one thing that my dot character
   will never accept is L'\0' (the null terminator).  The dot should not match
   WEOF, but since wchar_t can't hold WEOF, I'm good there.  Generally, the dot
   is not supposed to match L'\n' either, so I will also add that, but
   optionally.
 */
fsm *regex_parse_create_dot_fsm(bool newline_accepted)
{
  fsm_trans *ft;
  fsm *f = fsm_create();
  int src = fsm_add_state(f, false);
  int dest = fsm_add_state(f, true); // accepting
  f->start = src;
  if (!newline_accepted) {
    ft = fsm_trans_create(2, FSM_TRANS_NEGATIVE, dest);
    ft->start[1] = L'\n';
    ft->end[1] = L'\n';
  } else {
    ft = fsm_trans_create(1, FSM_TRANS_NEGATIVE, dest);
  }

  // No matter what, we can't allow EPSILON transitions for a dot.
  ft->start[0] = EPSILON;
  ft->start[0] = EPSILON;

  fsm_add_trans(f, src, ft);
  return f;
}

/**
   @brief Returns an FSM for an escape sequence, outside of a character class.

   Basically, adds the `\W`, `\w`, `\D`, `\d`, `\S`, `\s` character classes to
   the already existing character escape sequences covered by get_escape().

   Expects that `*regex` points to the backslash in the escape sequence.  Always
   returns such that `*regex` points to the LAST character in the escape
   sequence.

   @param regex Pointer to the pointer to the backslash escape.
   @return FSM to accept the backslash escape sequence.
 */
fsm *regex_parse_outer_escape(const wchar_t **regex)
{
  char c;

  (*regex)++; // advance to the specifier
  switch (**regex) {
  case L's':
    return regex_parse_create_whitespace_fsm(FSM_TRANS_POSITIVE);
  case L'S':
    return regex_parse_create_whitespace_fsm(FSM_TRANS_NEGATIVE);
  case L'w':
    return regex_parse_create_word_fsm(FSM_TRANS_POSITIVE);
  case L'W':
    return regex_parse_create_word_fsm(FSM_TRANS_NEGATIVE);
  case L'd':
    return regex_parse_create_digit_fsm(FSM_TRANS_POSITIVE);
  case L'D':
    return regex_parse_create_digit_fsm(FSM_TRANS_NEGATIVE);
  default:
    c = get_escape(regex);
    // get_escape() leaves the pointer AFTER the last character in the escape.
    // We want it ON the last one.
    (*regex)--;
    return fsm_create_single_char(c);
  }
}

/**
   @brief State for parsing character classes when not part of a range.
*/
#define NORMAL 0

/**
   @brief State for parsing character classes when part of a range.
*/
#define RANGE 1

/**
   @brief Create a FSM for a character class.

   Reads a character class (pointed by `*regex`), which it then converts to a
   single transition FSM.

   @param regex Pointer to pointer to location in string!
   @returns FSM for character class.
 */
fsm *regex_parse_char_class(const wchar_t **regex)
{
  smb_ll start, end;
  smb_status status = SMB_SUCCESS;
  DATA d;
  int type = FSM_TRANS_POSITIVE, state = NORMAL;
  smb_iter iter;
  fsm *f;
  int src, dest;
  fsm_trans *ft;

  ll_init(&start);
  ll_init(&end);

  // Detect whether the character class is positive or negative
  (*regex)++;
  if (**regex == L'^') {
    type = FSM_TRANS_NEGATIVE;
    (*regex)++;
  }

  // Loop through characters in the character class, recording each start-end
  // pair for the transition.
  for ( ; **regex != L']'; (*regex)++) {
    if (**regex == L'-') {
      state = RANGE;
    } else {
      // Get the correct character
      if (**regex == L'\\') {
        (*regex)++;
        d.data_llint = get_escape(regex);
        (*regex)--;
      } else {
        d.data_llint = **regex;
      }
      // Put it in the correct place
      if (state == NORMAL) {
        ll_append(&start, d);
        ll_append(&end, d);
      } else {
        // Modify the last transition if this is a range
        ll_set(&end, ll_length(&end)-1, d, &status);
        state = NORMAL;
      }
    }
  }

  if (state == RANGE) {
    // The last hyphen was meant to be literal.  Yay!
    d.data_llint = L'-';
    ll_append(&start, d);
    ll_append(&end, d);
  }

  // Now, create an fsm and fsm_trans, and write the recorded pairs into the
  // allocated start and end buffers.
  f = fsm_create();
  src = fsm_add_state(f, false);
  dest = fsm_add_state(f, true);
  f->start = src;
  ft = fsm_trans_create(ll_length(&start) + (type == FSM_TRANS_NEGATIVE ? 1 : 0),
                        type, dest);

  iter = ll_get_iter(&start);
  while (iter.has_next(&iter)) {
    d = iter.next(&iter, &status);
    assert(status == SMB_SUCCESS);
    ft->start[iter.index-1] = (wchar_t) d.data_llint;
  }
  iter = ll_get_iter(&end);
  while (iter.has_next(&iter)) {
    d = iter.next(&iter, &status);
    assert(status == SMB_SUCCESS);
    ft->end[iter.index-1] = (wchar_t) d.data_llint;
  }
  if (type == FSM_TRANS_NEGATIVE) {
    // We cannot allow epsilon to be matched in an actual character class.
    ft->start[ll_length(&start)] = EPSILON;
    ft->end[ll_length(&start)] = EPSILON;
  }

  fsm_add_trans(f, src, ft);

  ll_destroy(&start);
  ll_destroy(&end);
  return f;
}

/**
   @brief The workhorse recursive helper to regex_parse().

   Crawls through a regex, holding a FSM of the expression so far.  Single
   characters are concatenated as simple 0 -> 1 machines.  Parenthesis initiate
   a recursive call.  Pipes union the current regex with everything after.

   Needs to be called with a non-NULL value for both, which is why there is a
   top level function.

   @param regex Local pointer to the location in the regular expression.
   @param final Pointer to the parent's location.
   @return An FSM for the regular expression.
 */
fsm *regex_parse_recursive(const wchar_t *regex, const wchar_t **final)
{
  // Initial FSM is a machine that accepts the empty string.
  fsm *curr = fsm_create();
  fsm *new;
  curr->start = fsm_add_state(curr, true);

  // ASSUME THAT ALL PARENS, ETC ARE BALANCED!

  for ( ; *regex; regex++) {
    switch (*regex) {

    case L'(':
      new = regex_parse_recursive(regex + 1, &regex);
      regex_parse_check_modifier(new, &regex);
      fsm_concat(curr, new);
      fsm_delete(new, true);
      break;

    case L')':
      *final = regex;
      return curr;
      break;

    case L'|':
      // Need to pass control back up without attempting to check modifiers.
      new = regex_parse_recursive(regex + 1, &regex);
      fsm_union(curr, new);
      fsm_delete(new, true);
      *final = regex;
      return curr;
      break;

    case L'[':
      new = regex_parse_char_class(&regex);
      regex_parse_check_modifier(new, &regex);
      fsm_concat(curr, new);
      fsm_delete(new, true);
      break;

    case L'\\':
      new = regex_parse_outer_escape(&regex);
      regex_parse_check_modifier(new, &regex);
      fsm_concat(curr, new);
      fsm_delete(new, true);
      break;

    case L'.':
      new = regex_parse_create_dot_fsm(false);
      regex_parse_check_modifier(new, &regex);
      fsm_concat(curr, new);
      fsm_delete(new, true);
      break;

    default:
      // A regular letter
      new = fsm_create_single_char(*regex);
      regex_parse_check_modifier(new, &regex);
      fsm_concat(curr, new);
      fsm_delete(new, true);
      break;
    }
  }
  *final = regex;
  return curr;
}

/**
   @brief Construct a FSM to accept the given regex.
   @param regex Regular expression string.
   @return A FSM that can be used to decide the language of the regex.
 */
fsm *regex_parse(const wchar_t *regex){
  return regex_parse_recursive(regex, &regex);
}
