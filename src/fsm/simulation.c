/***************************************************************************//**

  @file         fsm/simulation.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Simulation functions for FSMs.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>      // for fprintf
#include <stdbool.h>    // for bool
#include <assert.h>

#include "libstephen/fsm.h"// funtions we're defining
#include "libstephen/al.h" // al_*, SMB_DP
#include "libstephen/log.h"

smb_logger fsm_sim_log = {
  .format = SMB_DEFAULT_LOGFORMAT,
  .num = 0,
};

/*******************************************************************************

                               Private Functions

******************************************************************************/

static void add_eps_to_curr(fsm_sim *fs, int state, smb_al *cap)
{
  smb_status status = SMB_SUCCESS;
  smb_al visit_queue;
  smb_al cap_queue;
  smb_al *trans_list;
  smb_al *new_cap;
  fsm_trans *ft;
  int i;
  DATA d;

  al_init(&visit_queue);
  al_init(&cap_queue);
  al_push_back(&visit_queue, LLINT(state));
  al_push_back(&cap_queue, PTR(cap));

  // THIS IS BREADTH FIRST SEARCH
  while (al_length(&visit_queue) > 0) {

    // GET THE NEXT ITEM IN THE BFS VISIT QUEUE
    d = al_pop_front(&visit_queue, &status);
    assert(status == SMB_SUCCESS);
    cap = al_pop_front(&cap_queue, &status).data_ptr;
    LDEBUG(&fsm_sim_log, "expanding state %ld", d.data_llint);

    // GET THE OUTGOING TRANSITION OF THE ITEM
    trans_list = al_get(&fs->f->transitions, d.data_llint, &status).data_ptr;
    assert(status == SMB_SUCCESS);

    // FOR EACH OUTGOING TRANSITION FROM THE STATE
    for (i = 0; i < al_length(trans_list); i++) {

      // GET THE TRANSITION'S DESTINATION
      ft = al_get(trans_list, i, &status).data_ptr;
      assert(status == SMB_SUCCESS);
      d.data_llint = ft->dest;
      LDEBUG(&fsm_sim_log, " expanding transition to %ld", d.data_llint);

      // IF THE DESTINATION STATE IS REACHABLE VIA EPSILON,
      // AND NOT IN THE VISIT QUEUE,
      // AND NOT ALREADY IN THE CURRENT STATE LIST...
      if (fsm_trans_check(ft, EPSILON) &&
          al_index_of(&visit_queue, d, &data_compare_int) == -1 &&
          al_index_of(fs->curr, d, &data_compare_int) == -1) {

        LDEBUG(&fsm_sim_log, "  adding to epsilon closure!");
        // CREATE A NEW CAPTURE LIST WITH THE OLD ITEMS
        new_cap = al_create();
        al_copy_all(new_cap, cap);

        // AND THE CURRENT INDEX, IF THERE IS A CAPTURE IN THIS TRANSITION
        if (FLAG_CHECK(ft->flags, FSM_TRANS_CAPTURE)) {
          al_append(new_cap, LLINT(fs->index));
          LDEBUG(&fsm_sim_log, "  adding capture at the current index");
        }

        // FINALLY, ADD THE DESTINATION AND ITS CAPTURE LIST TO THE CURRENT
        // STATE LIST
        al_append(fs->curr, d);
        al_append(fs->cap, PTR(new_cap));

        // ALSO, ADD TO THE VISIT QUEUE SO WE CAN EXPAND ITS NEXT STATES
        al_append(&visit_queue, d);
        al_append(&cap_queue, PTR(new_cap));
      }
    }
  }

  al_destroy(&visit_queue);
  al_destroy(&cap_queue);
}

/**
   @brief Test if the intersection between the two lists is non-empty.
   @param first The first list
   @param second The second list
   @retval true when the intersection is NOT empty
   @retval false when the intersection IS empty
 */
static bool fsm_sim_nondet_non_empty_intersection(smb_al *first, smb_al *second)
{
  smb_status status;
  int i;
  DATA d;
  for (i = 0; i < al_length(first); i++) {
    d = al_get(first, i, &status);
    if (al_index_of(second, d, &data_compare_int) != -1)
      return true;
  }
  return false;
}

/*******************************************************************************

                                Public Functions

*******************************************************************************/

void al_copy_all(smb_al *dest, const smb_al *from)
{
  smb_status status;
  int i;
  for (i = 0; i < al_length(from); i++) {
    al_append(dest, al_get(from, i, &status));
  }
}

bool fsm_sim_det(fsm *f, const wchar_t *input)
{
  smb_status status;
  DATA d;
  int i, next, state;
  smb_al *list;
  fsm_trans *trans;
  state = f->start;

  // While we have not exhausted our input
  while (*input != '\0') {
    SMB_DP("State: %d, Input %Lc\n", state, *input);
    // Get the list of transitions from this state
    list = (smb_al *) al_get(&f->transitions, state, &status).data_ptr;
    next = -1;
    // Look for transitions that match this character
    for (i = 0; i < al_length(list); i++) {
      trans = (fsm_trans *) al_get(list, i, &status).data_ptr;
      if (fsm_trans_check(trans, *input)) {
        if (next == -1) {
          next = trans->dest;
        } else {
          // If there are two matching transitions, error.
          fprintf(stderr, "Error: non-deterministic FSM simulated as "
                  "deterministic.\n");
        }
      }
    }
    // If there are no transitions, fail.
    if (next == -1) {
      SMB_DP("No available transitions, reject.\n");
      return false;
    }
    state = next;
    input++;
  }

  SMB_DP("State: %d, Input finished.\n", state);

  // If the state is in the accepting states, we accept, else reject
  d.data_llint = state;
  return al_index_of(&f->accepting, d, &data_compare_int) != -1;
}

fsm_sim *fsm_sim_nondet_begin(fsm *f)
{
  smb_status status = SMB_SUCCESS;
  smb_al *curr = al_create();
  fsm_sim *fs = fsm_sim_create(f, curr);
  int i; //delete
  fs->index = 0;
  al_append(curr, LLINT(f->start));
  // For diagnostics, print out the entire current state set
  for (i = 0; i < al_length(fs->curr); i++) {
    LDEBUG(&fsm_sim_log, "state[%d] = %Ld", i, al_get(fs->curr, i, &status).data_llint);
  }

  add_eps_to_curr(fs, f->start, al_get(fs->cap, 0, &status).data_ptr);
  // For diagnostics, print out the entire current state set
  for (i = 0; i < al_length(fs->curr); i++) {
    LDEBUG(&fsm_sim_log, "state[%d] = %Ld ", i, al_get(fs->curr, i, &status).data_llint);
  }
  assert(status == SMB_SUCCESS);
  return fs;
}

int fsm_sim_nondet_state(const fsm_sim *s, wchar_t input)
{
  // If the current state is empty, REJECT
  if (al_length(s->curr) == 0) {
    LDEBUG(&fsm_sim_log, "current state is empty, reject");
    return FSM_SIM_REJECTED;
  }
  if (fsm_sim_nondet_non_empty_intersection(&s->f->accepting, s->curr)) {
    LDEBUG(&fsm_sim_log, "in an accepting state");
    // If one of our current states is accepting...
    if (input == L'\0') {
      // ... and input is exhausted, ACCEPT
      return FSM_SIM_ACCEPTED;
    } else {
      // ... and input remains, we are accepting, but not done
      return FSM_SIM_ACCEPTING;
    }
  } else {
    LDEBUG(&fsm_sim_log, "in a rejecting state");
    // If no current state is accepting ...
    if (input == L'\0') {
      // ... and the input is exhausted, REJECT
      return FSM_SIM_REJECTED;
    } else {
      // ... and input remains, we are not accepting, but not done
      return FSM_SIM_NOT_ACCEPTING;
    }
  }
}

void fsm_sim_nondet_step(fsm_sim *s, wchar_t input)
{
  smb_status status;
  int i, j, state, original;
  DATA d;
  fsm_trans *t;
  smb_al *next = al_create(&status);
  smb_al *next_cap = al_create(&status);
  smb_al *cap;
  smb_al *trans;

  //// FIRST, we find every matching transition out of any current state.

  // For each current state:
  for (i = 0; i < al_length(s->curr); i++) {
    state = (int) al_get(s->curr, i, &status).data_llint;
    trans = (smb_al *) al_get(&s->f->transitions, state, &status).data_ptr;

    // For each transition out:
    for (j = 0; j < al_length(trans); j++) {
      t = (fsm_trans *)al_get(trans, j, &status).data_ptr;
      d.data_llint = t->dest;

      // If the transition contains the current input, and it's not already in
      // the next state list, add it to the next state list.
      if (fsm_trans_check(t, input)) {
        assert(!FLAG_CHECK(t->flags, FSM_TRANS_CAPTURE));
        if (al_index_of(next, d, &data_compare_int) == -1) {
          cap = al_create();
          al_copy_all(cap, al_get(s->cap, i, &status).data_ptr);
          al_append(next, d);
          al_append(next_cap, PTR(cap));
        }
      }
    }
  }

  //// NEXT, these next states become our new states

  // The current state is now the next state!
  for (i = 0; i < al_length(s->cap); i++) {
    al_delete(al_get(s->cap, i, &status).data_ptr);
  }
  al_delete(s->curr);
  al_delete(s->cap);
  s->cap = next_cap;
  s->curr = next;
  s->index++;  // index MUST be incremented here

  //// FINALLY, we expand our current state list to be the "epsilon closure" of
  //// all its elements (i.e., all states reachable via epsilon-transition from
  //// any current state).

  original = al_length(s->curr);
  for (i = 0; i < original; i++) {
    smb_al *cap;
    state = al_get(s->curr, i, &status).data_llint;
    cap = al_get(s->cap, i, &status).data_ptr;
    add_eps_to_curr(s, state, cap);
  }

  // For diagnostics, print the new state
  for (i = 0; i < al_length(s->curr); i++) {
    LDEBUG(&fsm_sim_log, "state[%d] = %Ld ", i, al_get(s->curr, i, &status).data_llint);
  }
}

smb_al *fsm_sim_get_captures(fsm_sim *sim)
{
  smb_iter it = al_get_iter(sim->cap);
  smb_status status = SMB_SUCCESS;
  smb_al *captures, *retval = NULL;
  int max = -1, max_idx = -1;
  while (it.has_next(&it)) {
    captures = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    //iter_print(al_get_iter(captures), stdout, data_printer_int);
    if (al_length(captures) > max && al_length(captures) % 2 == 0) {
      max = al_length(captures);
      max_idx = it.index - 1;
    }
  }
  if (max_idx >= 0 && max > 0) {
    retval = al_create();
    al_copy_all(retval, al_get(sim->cap, max_idx, &status).data_ptr);
    assert(status == SMB_SUCCESS);
  }
  return retval;
}

bool fsm_sim_nondet_capture(fsm *f, const wchar_t *input, smb_al **capture)
{
  LDEBUG(&fsm_sim_log, "STARTING FSM SIMULATION");
  fsm_sim *sim = fsm_sim_nondet_begin(f);
  int i = 0;
  int res = fsm_sim_nondet_state(sim, input[i]);
  bool accepted;

  while (res != FSM_SIM_REJECTED && res != FSM_SIM_ACCEPTED) {
    LDEBUG(&fsm_sim_log, "PROCESS INPUT '%lc'", input[i]);
    fsm_sim_nondet_step(sim, input[i]);
    i++;
    res = fsm_sim_nondet_state(sim, input[i]);
    LDEBUG(&fsm_sim_log, "Current result: %d\n", res);
  }
  accepted = res != FSM_SIM_REJECTED;

  if (accepted && capture != NULL) {
    *capture = fsm_sim_get_captures(sim);
  }

  fsm_sim_delete(sim, true);
  return accepted;
}

bool fsm_sim_nondet(fsm *f, const wchar_t *input)
{
  return fsm_sim_nondet_capture(f, input, NULL);
}
