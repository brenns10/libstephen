/***************************************************************************//**

  @file         fsm.h

  @author       Stephen Brennan

  @date         Created Wednesday, 14 May 2014

  @brief        Finite state machine definitions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef SMB_FSM_H
#define SMB_FSM_H

#include <stdbool.h>
#include <wchar.h>

#include "libstephen/al.h"

/**
   @brief A value for fsm_trans.type indicating that the ranges are positive.

   That is, the characters within the ranges are considered valid.

   @see fsm_trans.type
   @see FSM_TRANS_NEGATIVE
 */
#define FSM_TRANS_POSITIVE 0
/**
   @brief A value for fsm_trans.type indicating that the ranges are negative.

   That is, the characters within the ranges not considered valid (but
   everything else is).

   @see fsm_trans.type
   @see FSM_TRANS_POSITIVE
 */
#define FSM_TRANS_NEGATIVE 1

/**
   @brief A possible return value for fsm_sim_nondet_state().

   This return value indicates that there is a part of the simulation that is
   currently in an accepting state, but that not all input has been exhausted
   yet.

   @see fsm_sim_nondet_state()
   @see FSM_SIM_NOT_ACCEPTING
   @see FSM_SIM_REJECTED
   @see FSM_SIM_ACCEPTED
 */
#define FSM_SIM_ACCEPTING 0
/**
   @brief A possible return value for fsm_sim_nondet_state().

   This return value indicates that there no part of the simulation that is
   currently in an accepting state, but that not all input has been exhausted
   yet.

   @see fsm_sim_nondet_state()
   @see FSM_SIM_ACCEPTING
   @see FSM_SIM_REJECTED
   @see FSM_SIM_ACCEPTED
 */
#define FSM_SIM_NOT_ACCEPTING 1
/**
   @brief A possible return value for fsm_sim_nondet_state().

   This return value indicates that the simulation is complete, and that all
   possible runs of the NDFSM have rejected the input.

   @see fsm_sim_nondet_state()
   @see FSM_SIM_ACCEPTING
   @see FSM_SIM_NOT_ACCEPTING
   @see FSM_SIM_ACCEPTED
 */
#define FSM_SIM_REJECTED 2
/**
   @brief A possible return value for fsm_sim_nondet_state().

   This return value indicates that the simulation is complete, and that there
   is at least one possible run of the NDFSM that accepts the input.

   @see fsm_sim_nondet_state()
   @see FSM_SIM_ACCEPTING
   @see FSM_SIM_NOT_ACCEPTING
   @see FSM_SIM_REJECTED
 */
#define FSM_SIM_ACCEPTED 3

/**
   @brief A value of type `wchar_t` that is used to represent the empty string.
   @todo Research a better value for EPSILON.
 */
#define EPSILON ((wchar_t)-2)

/**
   @brief Contains information regarding a FSM transition.

   The contained information includes the destination, the ranges of characters
   that are included in the transition, and the way to interpret these ranges.
*/
typedef struct {

  /**
     @brief Tells how to interpret the range.

     If it has value FSM_TRANS_POSITIVE, then everything within the range is
     accepted in the transition.  If it has value FSM_TRANS_NEGATIVE, then
     everything not within the range is accepted in the transition.

     @see FSM_TRANS_POSITIVE
     @see FSM_TRANS_NEGATIVE
   */
  int type;

  /**
     @brief An array of characters that form the starts of the ranges.

     This array (string) MUST be null-terminated.  The number of ranges is not
     stored.
   */
  wchar_t *start;

  /**
     @brief An array of characters that form the ends of the ranges.

     This array (string) MUST be null-terminated.  The number of ranges is not
     stored.  Furthermore, the number of elements MUST match the number of
     elements in start.  This is all handled by fsm_trans_init() and
     fsm_trans_create(), of course.
   */
  wchar_t *end;

  /**
     @brief The index of the destination of this transition.
   */
  int dest;

} fsm_trans;

/**
   @brief A structure representing an FSM.

   The machine represented may be deterministic or nondeterministic.  There is
   not an implemented function to distinguish between the two, but it is
   possible.

   In this representation of the FSM, states are represented only by indices.
   They have no associated string value.
 */
typedef struct {

  /**
     @brief The index of the start states.
   */
  int start;

  /**
     @brief The transitions in the machine.

     The transitions are stored as an array list (`smb_al`) of array lists
     (`smb_al *`).  The outer list is indexed by state number.  The inner list
     is a list of all transitions (`fsm_trans *`) out of that particular state.

     The length of the outer list is the same as the number of states in the
     FSM.
   */
  smb_al transitions;

  /**
     @brief The accepting states for this machine.

     These accepting states are stored as integer values in the list.
   */
  smb_al accepting;

} fsm;

/**
   @brief Holds state for a nondeterministic finite state machine simulation.

   None of these members are guaranteed to remain; they are to be considered
   implementation details of the function fsm_sim_nondet() and friends.
 */
typedef struct {

  /**
     @brief The FSM being simulated.
   */
  fsm *f;

  /**
     @brief The current "state", as a list of FSM states.
   */
  smb_al *curr;

} fsm_sim;

// Error constant declarations:
#define CKY_TOO_FEW_LINES   (SMB_EXTERNAL_EXCEPTION_START + 0)
#define CKY_MALFORMED_TRANS (SMB_EXTERNAL_EXCEPTION_START + 1)

// datastructs.c
void fsm_trans_init(fsm_trans *ft, int n, int type, int dest);
fsm_trans *fsm_trans_create(int n, int type, int dest);
void fsm_trans_destroy(fsm_trans *ft);
void fsm_trans_delete(fsm_trans *ft);
void fsm_trans_init_single(fsm_trans *ft, wchar_t start, wchar_t end, int type,
                           int dest);
fsm_trans *fsm_trans_create_single(wchar_t start, wchar_t end, int type,
                                   int dest);
fsm_trans *fsm_trans_copy(const fsm_trans *ft);
bool fsm_trans_check(const fsm_trans *ft, wchar_t c);

void fsm_init(fsm *f);
fsm *fsm_create(void);
void fsm_destroy(fsm *f, bool free_transitions);
void fsm_delete(fsm *f, bool free_transitions);
fsm *fsm_create_single_char(wchar_t character);
int fsm_add_state(fsm *f, bool accepting);
void fsm_add_trans(fsm *f, int state, const fsm_trans *ft);
fsm_trans *fsm_add_single(fsm *f, int from, int to, wchar_t start, wchar_t end,
                          int type);

void fsm_sim_init(fsm_sim *fs, fsm *f, smb_al *curr);
fsm_sim *fsm_sim_create(fsm *f, smb_al *curr);
void fsm_sim_destroy(fsm_sim *fs, bool free_curr);
void fsm_sim_delete(fsm_sim *fs, bool free_curr);

// io.c
int fsm_read_get_int(const wchar_t **start, const wchar_t *prefix);
fsm_trans *fsm_read_trans(const wchar_t **source, int *start);
void fsm_print_char(FILE *dest, wchar_t input);
void fsm_dot_char(FILE * dest, wchar_t c);

fsm *fsm_read(const wchar_t *source, smb_status *status);
wchar_t *fsm_str(const fsm *f);
void fsm_print(const fsm *f, FILE *dest);
void fsm_dot(fsm *f, FILE *dest);

// simulation.c
void al_copy_all(smb_al *dest, const smb_al *from);
smb_al *fsm_sim_nondet_epsilon_closure(fsm *f, int state);
void fsm_sim_nondet_union_and_delete(smb_al *first, smb_al *second);
bool fsm_sim_nondet_non_empty_intersection(smb_al *first, smb_al *second);

bool fsm_sim_det(fsm *f, const wchar_t *input);
fsm_sim *fsm_sim_nondet_begin(fsm *f);
int fsm_sim_nondet_state(const fsm_sim *s, wchar_t input);
void fsm_sim_nondet_step(fsm_sim *s, wchar_t input);
bool fsm_sim_nondet(fsm *f, const wchar_t *input);

// operations.c
fsm *fsm_copy(const fsm *f);
void fsm_copy_trans(fsm *dest, const fsm *src);
void fsm_concat(fsm *first, const fsm *second);
void fsm_union(fsm *first, const fsm *second);
void fsm_kleene(fsm *f);

#endif
