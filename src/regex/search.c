/***************************************************************************//**

  @file         regex/search.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Regular expression search functions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include "libstephen/regex.h" // functions we're implementing
#include "libstephen/fsm.h"   // for FSM's
#include "libstephen/al.h"    // for array lists

smb_al *fsm_search(fsm *regex_fsm, const wchar_t *srchText, bool greedy,
                   bool overlap)
{
  fsm_sim *curr_sim;
  int start = 0, length, last_length, res;
  smb_al *results = al_create();
  DATA d;

  SMB_DP("STARTING FSM SEARCH\n");

  // OUTER WHILE: iterate over starting characters.
  while (srchText[start] != L'\0') {
    // Simulate the FSM at each character.
    curr_sim = fsm_sim_nondet_begin(regex_fsm);
    length = 0;
    last_length = -1;
    res = -1;

    SMB_DP("=> Begin simulation at index %d: '%lc'.\n", start, srchText[start]);

    // INNER WHILE: iterate over lengths.
    while (srchText[start + length] != L'\0' && res != FSM_SIM_REJECTED) {
      // Step through the FSM simulation until we hit the end of the string, are
      // rejected, or are accepted.
      fsm_sim_nondet_step(curr_sim, srchText[start + length]);
      res = fsm_sim_nondet_state(curr_sim, srchText[start + length]);
      length++;
      SMB_DP("   => On step (length %d), res=%d.\n", length, res);

      // When we encounter a possible match, mark it.  We will only return the
      // largest (last) match for each simulation.
      if (res == FSM_SIM_ACCEPTING || res == FSM_SIM_ACCEPTED) {
        last_length = length;
        SMB_DP("      Currently accepting!\n");
      }
      // If the search reports input exhausted, we leave the loop.
      if (res == FSM_SIM_ACCEPTED || res == FSM_SIM_REJECTED) {
        break;
      }
    } // END INNER WHILE

    // Clean up the simulation.
    fsm_sim_delete(curr_sim, true);

    if (last_length != -1) {
      // If we encounter a match during this simulation, record it.
      SMB_DP("=> Found match of length %d.\n", last_length);
      d.data_ptr = (void *) regex_hit_create(start, last_length);
      al_append(results, d);

      if (greedy) {
        SMB_DP("=> Greedy return.\n");
        return results; // return after first hit
      }

      if (overlap) {
        // If we allow overlapping hits, we only need to move one character
        // over.
        start++;
      } else {
        // If we don't allow overlapping hits, the next search has to start
        // after the end of this hit.
        start += last_length;
      }
    } else {
      start++;
    }
  } // END OUTER WHILE
  return results;
}

smb_al *regex_search(const wchar_t *regex, const wchar_t *srchText, bool greedy,
                     bool overlap)
{
  fsm *regex_fsm = regex_parse(regex);
  smb_al *return_value = fsm_search(regex_fsm, srchText, greedy, overlap);
  fsm_delete(regex_fsm, true);
  return return_value;
}
