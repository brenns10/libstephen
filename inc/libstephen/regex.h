/***************************************************************************//**

  @file         regex.h

  @author       Stephen Brennan

  @date         Created Sunday, 18 May 2014

  @brief        Declarations for regular expression routines.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef SMB_REGEX_H
#define SMB_REGEX_H

#include "libstephen/al.h"
#include "fsm.h"

/**
   @brief A struct to hold regular expression search hits.
 */
typedef struct {

  /**
     @brief The location the hit starts at.  This is inclusive.
   */
  int start;

  /**
     @brief Number of characters in the hit.
   */
  int length;

} regex_hit;

// datastructs.c
void regex_hit_init(regex_hit *obj, int start, int length);
regex_hit *regex_hit_create(int start, int length);
void regex_hit_destroy(regex_hit *obj);
void regex_hit_delete(regex_hit *obj);

// parse.c
void regex_parse_check_modifier(fsm *new, const wchar_t **regex);
fsm *regex_parse_create_whitespace_fsm(int type);
fsm *regex_parse_create_word_fsm(int type);
fsm *regex_parse_create_digit_fsm(int type);
fsm *regex_parse_outer_escape(const wchar_t **regex);
fsm *regex_parse_char_class(const wchar_t **regex);
fsm *regex_parse(const wchar_t *regex);

// search.c
smb_al *fsm_search(fsm *regex_fsm, const wchar_t *srchText, bool greedy,
                   bool overlap);
smb_al *regex_search(const wchar_t *regex, const wchar_t *srchText, bool greedy,
                     bool overlap);

#endif
