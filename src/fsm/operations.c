/***************************************************************************//**

  @file         fsm/operations.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Operations on FSMs.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include "libstephen/fsm.h"// functions we are implementing
#include "libstephen/al.h" // array lists

/**
   @brief Allocate an entirely new copy of an existing FSM.

   The transitions are completely disjoint, so freeing them in the original will
   not affect the copy.

   @param f The FSM to copy
   @return A copy of the FSM
 */
fsm *fsm_copy(const fsm *f)
{
  int i, j;
  smb_al *old;
  fsm_trans *ft;
  fsm *new = fsm_create();
  smb_status status;

  // Copy basic members
  new->start = f->start;
  al_copy_all(&new->accepting, &f->accepting);

  // Initialise the same number of states
  for (i = 0; i < al_length(&f->transitions); i++) {
    fsm_add_state(new, false);
  }

  // Copy all transitions.
  for (i = 0; i < al_length(&f->transitions); i++) {
    old = (smb_al *) al_get(&f->transitions, i, &status).data_ptr;
    for (j = 0; j < al_length(old); j++) {
      ft = (fsm_trans *) al_get(old, j, &status).data_ptr;
      ft = fsm_trans_copy(ft);
      fsm_add_trans(new, i, ft);
    }
  }

  return new;
}

/**
   @brief Copy the transitions and states from src into dest.

   All the states are added, and the transitions are modified with the offset
   determined from the initial number of states in the destination.
 */
void fsm_copy_trans(fsm *dest, const fsm *src)
{
  smb_status status;
  int offset = al_length(&dest->transitions);
  int i,j;
  // Copy the transitions from the source one, altering their start and
  // destination to fit with the new state numbering.
  for (i = 0; i < al_length(&src->transitions); i++) {
    // Add new state corresponding to this one
    fsm_add_state(dest, false);
    // For each transition out of this state, add a copy to dest.
    const smb_al *list = al_get(&src->transitions, i, &status).data_ptr;
    for (j = 0; j < al_length(list); j++) {
      const fsm_trans *old = al_get(list, j, &status).data_ptr;
      fsm_trans *new = fsm_trans_copy(old);
      new->dest = new->dest + offset;
      fsm_add_trans(dest, i + offset, new);
    }
  }
}

/**
   @brief Concatenate two FSMs into the first FSM object.

   This function concatenates two FSMs.  This means it constructs a machine that
   will accept a string where the first part is accepted by the first machine,
   and the second part is accepted by the second machine.  It is done by hooking
   up the accepting states of the first machine to the start state of the second
   (via epsilon transition).  Then, only the second machines's accepting states
   are used for the final product.

   Note that the concatenation is done in-place into the first FSM.  If you
   don't wish for that to happen, use fsm_copy() to copy the first one, and then
   use fsm_concat() to concatenate into the copy.

   @param first The first FSM.  Is modified to contain the concatenation.
   @param second The second FSM.  It is not modified.
 */
void fsm_concat(fsm *first, const fsm *second)
{
  smb_status status;
  int i, offset = al_length(&first->transitions);
  DATA d;
  // Create a transition from each accepting state into the second machine's
  // start state.
  for (i = 0; i < al_length(&first->accepting); i++) {
    int start = (int)al_get(&first->accepting, i, &status).data_llint;
    // This could be outside the loop, but we need a new one for each instance
    fsm_trans *ft = fsm_trans_create_single(EPSILON, EPSILON,
                                            0,
                                            second->start + offset);
    fsm_add_trans(first, start, ft);
  }

  fsm_copy_trans(first, second);

  // Replace the accepting states of the first with the second.
  al_destroy(&first->accepting);
  al_init(&first->accepting);
  for (i = 0; i < al_length(&second->accepting); i++) {
    d = al_get(&second->accepting, i, &status);
    d.data_llint += offset;
    al_append(&first->accepting, d);
  }
}

/**
   @brief Construct the union of the two FSMs into the first one.

   This function creates an FSM that will accept any string accepted by the
   first OR the second one.  The FSM is constructed in place of the first
   machine, modifying the parameter.  If this is not what you want, make a copy
   of the first one, and pass the copy instead.

   @param first The first FSM to union.  Will be modified with the result.
   @param second The second FSM to union
 */
void fsm_union(fsm *first, const fsm *second)
{
  smb_status status;
  int newStart, i, offset = al_length(&first->transitions);
  fsm_trans *fsTrans, *ssTrans;
  DATA d;

  // Add the transitions from the second into the first.
  fsm_copy_trans(first, second);

  // Create a new start state!
  newStart = fsm_add_state(first, false);

  // Add epsilon-trans from new start to first start
  fsTrans = fsm_trans_create_single(EPSILON, EPSILON, 0,
                                    first->start);
  fsm_add_trans(first, newStart, fsTrans);

  // Add epsilon-trans from new start to second start
  ssTrans = fsm_trans_create_single(EPSILON, EPSILON, 0,
                                    second->start + offset);
  fsm_add_trans(first, newStart, ssTrans);

  // Accept from either the first or the second
  for (i = 0; i < al_length(&second->accepting); i++) {
    d = al_get(&second->accepting, i, &status);
    d.data_llint += offset;
    al_append(&first->accepting, d);
  }

  // Change the start state
  first->start = newStart;
}

/**
   @brief Modify an FSM to accept 0 or more occurrences of it.

   This modifies the existing FSM to accept L*, where L is the language accepted
   by the machine.

   @param f The FSM to perform Kleene star on
 */
void fsm_kleene(fsm *f)
{
  smb_status status;
  int i, newStart = fsm_add_state(f, false); // not accepting yet
  fsm_trans *newTrans;
  DATA d;

  // Add epsilon-trans from new start to first start
  newTrans = fsm_trans_create_single(EPSILON, EPSILON, 0,
                                     f->start);
  fsm_add_trans(f, newStart, newTrans);

  // For each accepting state, add a epsilon-trans to the new start
  for (i = 0; i < al_length(&f->accepting); i++) {
    newTrans = fsm_trans_create_single(EPSILON, EPSILON, 0,
                                       newStart);
    fsm_add_trans(f, (int) al_get(&f->accepting, i, &status).data_llint,
                  newTrans);
  }

  // Make the new start accepting
  d.data_llint = newStart;
  al_append(&f->accepting, d);

  // Change the start state (officially)
  f->start = newStart;
}
