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

/*******************************************************************************

                              regex/datastructs.c

*******************************************************************************/

/**
   @brief Initialize a regex_hit.
   @param obj The (pre-allocated) regex_hit.
   @param start The start location of the hit.
   @param length The length of the hit.
 */
void regex_hit_init(regex_hit *obj, int start, int length);
/**
   @brief Allocate a regex_hit.
   @param start The start location of the hit.
   @param length The end location of the hit.
   @return Pointer to the regex_hit.  Must be freed with regex_hit_delete().
 */
regex_hit *regex_hit_create(int start, int length);
/**
   @brief Clean up a regex_hit object.
   @param obj The regex_hit object to clean up.
 */
void regex_hit_destroy(regex_hit *obj);
/**
   @brief Clean up and free a regex_hit object.
   @param obj The regex_hit to free.
 */
void regex_hit_delete(regex_hit *obj);

/*******************************************************************************

                                 regex/parse.c

*******************************************************************************/

/**
   @brief Construct a FSM to accept the given regex.
   @param regex Regular expression string.
   @return A FSM that can be used to decide the language of the regex.
 */
fsm *regex_parse(const wchar_t *regex);

/*******************************************************************************

                                 regex/search.c

*******************************************************************************/

/**
   @brief Perform a regex-style search with an FSM on a search text.

   The "regex-style search" runs the FSM starting at each character in the text.
   If the FSM ever enters an accepting state, then that is a potential match.
   The last potential match (if any) for every character in the text makes up
   the list of hits.
   @param regex_fsm The FSM to search with.  Not necessarily generated from a
   regular expression.
   @param srchText The text to search the FSM on.
   @param greedy When set to true, only returns one hit.  Otherwise, returns all
   hits.
   @param overlap When set to true, allows overlapping hits.  For instance, a
   search for "\w+" on "blah" would return blah, lah, ah, and h.  Since this is
   usually undesirable, the recommended value is false.  When false, the search
   will skip to the end of each hit and resume searching there.
   @return An array list (smb_al) of `regex_hit *` full of hits.  Each hit must
   be individually freed when you're finished, as well as the array list itself.
 */
smb_al *fsm_search(fsm *regex_fsm, const wchar_t *srchText, bool greedy,
                   bool overlap);
/**
   @brief Searches for a regular expression on a search text.
   @see fsm_search() For full details on the operation of the function.
   @param regex The regular expression to search for.
   @param srchText The text to search in.
   @param greedy Whether to return first result only.
   @param overlap Whether to allow overlapping matches.
 */
smb_al *regex_search(const wchar_t *regex, const wchar_t *srchText, bool greedy,
                     bool overlap);

#endif
