/***************************************************************************//**

  @file         libstephen/fsm.h

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
   @brief A flag for fsm_trans.flags indicating that the ranges are negative.

   That is, the characters within the ranges not considered valid (but
   everything else is).
   @see fsm_trans.flags
 */
#define FSM_TRANS_NEGATIVE 0x0001
/**
   @brief A flag for fsm_trans.flags indicating that a capture starts here.
 */
#define FSM_TRANS_CAPTURE 0x0002

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
  unsigned int flags;

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

  /**
     @brief Each current state's list of captures.
   */
  smb_al *cap;

  /**
     @brief The current index!
   */
  int index;

} fsm_sim;

/**
   @brief Error returned by fsm_read() if there aren't any lines.
 */
#define CKY_TOO_FEW_LINES   (SMB_EXTERNAL_EXCEPTION_START + 0)
/**
   @brief Error returned by fsm_read() if the input is malformed.
 */
#define CKY_MALFORMED_TRANS (SMB_EXTERNAL_EXCEPTION_START + 1)

/*******************************************************************************

                               fsm/datastructs.c

*******************************************************************************/

/**
   @brief Initialize an already-allocated FSM transition unit.

   An FSM transition unit defines what characters can and can't be accepted for
   a transition, similar to a regular expression's character class.  It can
   either be a positive transition (any character between start and end), or a
   negative transition (any character not between start and end).  It can have
   more than one transition criteria (i.e character range), but they all have to
   be either positive or negative.
   @param ft The pre-allocated transition
   @param n The number of transition criteria
   @param flags Transition flags (0 is default)
   @param dest The destination of the transition
 */
void fsm_trans_init(fsm_trans *ft, int n, unsigned int flags, int dest);
/**
   @brief Allocate and initialize a fsm_trans object.
   @param n The number of ranges
   @param flags Transition flags (0 is default)
   @param dest The destination of the transition
 */
fsm_trans *fsm_trans_create(int n, unsigned int flags, int dest);
/**
   @brief Clean up the fsm_trans object.  Do not free it.
   @param ft The object to clean up
 */
void fsm_trans_destroy(fsm_trans *ft);
/**
   @brief Free the fsm_trans object.
   @param ft The object to free
 */
void fsm_trans_delete(fsm_trans *ft);
/**
   @brief Initialize a fsm_trans object with a single range.
   @param ft The object to initialize
   @param start The beginning of the range
   @param end The end of the range
   @param flags The flags of the range
   @param dest The destination of the transition
 */
void fsm_trans_init_single(fsm_trans *ft, wchar_t start, wchar_t end, unsigned int flags,
                           int dest);
/**
   @brief Allocate and initialize a fsm_trans object with a single range.
   @param start The beginning of the range
   @param end The end of the range
   @param flags The flags of the object
   @param dest The destination of the transition
 */
fsm_trans *fsm_trans_create_single(wchar_t start, wchar_t end, unsigned int flags,
                                   int dest);
/**
   @brief Check whether the character is accepted by the transition.
   @param ft The transition to check
   @param c The character to check
 */
bool fsm_trans_check(const fsm_trans *ft, wchar_t c);
/**
   @brief Allocate an entirely new copy of an existing FSM transition.
   @param ft The transition to copy
   @return The copy of ft
 */
fsm_trans *fsm_trans_copy(const fsm_trans *ft);

/**
   @brief Initialize a FSM.
   @param f The FSM to Initialize
 */
void fsm_init(fsm *f);
/**
   @brief Allocate and initialize an FSM.
 */
fsm *fsm_create(void);
/**
   @brief Clean up a FSM, but do not free it.
   @param f The FSM to clean up
   @param free_transitions Do we free the transitions too?
 */
void fsm_destroy(fsm *f, bool free_transitions);
/**
   @brief Clean up and free an FSM.
   @param f The FSM to free
   @param free_transitions Do we free the transitions too?
 */
void fsm_delete(fsm *f, bool free_transitions);
/**
   @brief Creates a FSM for a single character.
   @param character The character to make a transition for.
   @returns FSM for the character.
 */
fsm *fsm_create_single_char(wchar_t character);
/**
   @brief Add a state to the FSM
   @param f The FSM to add to
   @param accepting Whether to add the state to the accepting list
   @return The index of the state
 */
int fsm_add_state(fsm *f, bool accepting);
/**
   @brief Add a transition to the FSM.
   @param f The FSM to add the transition to
   @param state The state the transition is from
   @param ft The transition to add
 */
void fsm_add_trans(fsm *f, int state, const fsm_trans *ft);
/**
   @brief Add a single range transition to the FSM.

   This function simplifies the allocation and addition of the more common
   single range transitions.  It returns the fsm_trans pointer if the programmer
   wishes to do manual memory management.  However, the return value can be
   ignored, and false passed to fsm_delete() in order to have the transition
   freed automatically.
   @param f The FSM to add to
   @param from The state the transition is from
   @param to The state the transition is to
   @param start The character at the start of the range
   @param end The character at the end of the range
   @param flags The flags of range (positive or negative)
   @return Pointer to the fsm_trans created by the function.
 */
fsm_trans *fsm_add_single(fsm *f, int from, int to, wchar_t start, wchar_t end,
                          unsigned int flags);

/**
   @brief Initialize an fsm_sim struct
   @param fs The struct to init
   @param f The FSM to simulate
   @param curr The current state of the simulation
 */
void fsm_sim_init(fsm_sim *fs, fsm *f, smb_al *curr);
/**
   @brief Allocate and initialize an fsm_sim struct
   @param f The FSM to simulate
   @param curr The current state of the simulation
   @return The allocated simulation
 */
fsm_sim *fsm_sim_create(fsm *f, smb_al *curr);
/**
   @brief Clean up any resources held by the fsm_sim
   @param fs The simulation
   @param free_curr Do we free the state list (generally, true)
 */
void fsm_sim_destroy(fsm_sim *fs, bool free_curr);
/**
   @brief Clean up and any resources held by the fsm_sim, and free it.
   @param fs The simulation
   @param free_curr Do we free the state list (generally, true)
 */
void fsm_sim_delete(fsm_sim *fs, bool free_curr);

/*******************************************************************************

                                    fsm/io.c

*******************************************************************************/

/**
   @brief Read a Finite State Machine from its text representation.

   All fsm's can be printed using fsm_print().  This function *should* be able
   to take any such string representation and convert it back into an equivalent
   fsm again.
   @param source The string to read in.
   @param[out] status Status variable for error reporting.
   @returns New FSM, or NULL on error.
   @exception CKY_TOO_FEW_LINES if there isn't at least one line.
   @exception CKY_MALFORMED_TRANS if there is a problem reading a transition.
 */
fsm *fsm_read(const wchar_t *source, smb_status *status);
/**
   @brief Return a wchar_t* representation of the fsm (suitable for printing).
   @param f The FSM to print
   @return The string version of the FSM.
 */
wchar_t *fsm_str(const fsm *f);
/**
   @brief Print the FSM to the desired output stream.
   @param f FSM to print.
   @param dest Output stream/file to print to.
 */
void fsm_print(const fsm *f, FILE *dest);
/**
   @brief Print an FSM to graphviz dot format.

   Graphviz is a neat CLI tool to create diagrams of graphs.  The dot format is
   an input language for graphs.  This function takes a FSM and prints it to a
   file as a dot file, so that it can then be converted to a image by graphviz.
   @param f The machine to print.
   @param dest The destination file.
 */
void fsm_dot(fsm *f, FILE *dest);

/*******************************************************************************

                                fsm/simulation.c

******************************************************************************/

/**
   @brief Copy all the values from one array list into another.
   @param dest The destination array list
   @param from The source array list
 */
void al_copy_all(smb_al *dest, const smb_al *from);

/**
   @brief Run the finite state machine as a deterministic one.

   Simulates a DFSM.  Any possible input at any given state should have exactly
   one transition.  If there are no transitions for an input, this is treated as
   an implicit transition to a rejecting state, and the simulation rejects
   immediately.

   You should be aware that this simulation is really not all that useful,
   unless you constructed the FSM manually, and therefore know that it is
   deterministic.  Any FSM returned by regex_parse(), for instance, is *very*
   nondeterministic, and can only be run using fsm_sim_nondet() or its siblings.
   Also, this simulation completely ignores capture group information in the
   FSM, since capturing really only works when you nondeterministically
   simulate.
   @param f The FSM to simulate
   @param input The string of input
   @returns Whether the machine accepts the string or not.
 */
bool fsm_sim_det(fsm *f, const wchar_t *input);
/**
   @brief Begin a non-deterministic simulation of this finite state machine.

   This simulation is designed to be run in steps.  First, call this function.
   Then, loop through many calls to fsm_sim_nondet_step().  When
   fsm_sim_nondet_state() returns a value indicating that the simulation is
   complete, you will have your answer.  Note that you should also call
   fsm_sim_nondet_state() after a call to this function, because the input may
   be empty.

   @param f The FSM to simulate
   @returns A `fsm_sim*` containing all of the simulation state.  This must be
   freed with fsm_sim_delete() when you're done with it.
 */
fsm_sim *fsm_sim_nondet_begin(fsm *f);
/**
   @brief Check the state of the simulation.
   @param s The simulation to check.
   @param input The character that will be the next input.  This is used to
   determine whether or not the decision is final (ACCEPTED/REJECTED), or may
   change if you continue the simulation.
   @retval FSM_SIM_ACCEPTING when the simulation has not ended, but is accepting
   @retval FSM_SIM_NOT_ACCEPTING when the simulation has not ended, and is not
   accepting
   @retval FSM_SIM_REJECTED when the simulation has ended and rejected
   @retval FSM_SIM_ACCEPTED when the simulation has ended and accepted
   @see FSM_SIM_ACCEPTING
   @see FSM_SIM_NOT_ACCEPTING
   @see FSM_SIM_REJECTED
   @see FSM_SIM_ACCEPTED
 */
int fsm_sim_nondet_state(const fsm_sim *s, wchar_t input);
/**
   @brief Perform a single step in the non-deterministic simulation.

   This function performs a single iteration of the simulation algorithm.  It
   takes the current states, finds next states with the input character, and
   then adds in the epsilon closures of all the states.  You should use
   fsm_sim_nondet_state() to determine the state of the simulation after a step.
   @param s The simulation state struct
   @param input The next input character to the simulation.
 */
void fsm_sim_nondet_step(fsm_sim *s, wchar_t input);
/**
   @brief Return the "correct" capture list for this simulation.

   Since an NDFSM has a number of possible states it could be in, there is more
   than one set of possible captures.  This function looks through the list of
   current states' captures, and selects the one with the most captured groups
   (also, it will only select even sized lists, since odd ones don't make
   sense).  It then returns a copy of it (so that you can free the whole
   simulation and keep the capture list).
   @param sim The simulation to return captures from.
   @returns Reference to list of captures (indexes) which you must free
   separately from the simulation.
 */
smb_al *fsm_sim_get_captures(fsm_sim *sim);
/**
   @brief Simulate a NDFSM without capturing.
   @param f The NDFSM to simulate
   @param input The input string to run with
   @retval true if the machine accepts
   @retval false if the machine rejects
 */
bool fsm_sim_nondet(fsm *f, const wchar_t *input);
/**
   @brief Simulate NDFSM, optionally returning captured groups.

   This function is a convenience function that completely simulates the NDFSM.
   The "sub"-functions, fsm_sim_nondet_begin(), fsm_sim_nondet_step(),
   fsm_sim_nondet_state(), fsm_sim_get_captures(), and fsm_sim_delete(), can be
   used together to perform a more customizable simulation.  For an example, you
   may want to see fsm_search(), which performs a search through a body of text.
   @param f The FSM to simulate
   @param input The input string to run with
   @param[out] capture Pointer to `smb_al*` where to store capture list.
   Ignored when NULL.  Pointer is set to NULL if there are no matches, and not
   touched when the simulation rejects.
   @retval true if the machine accepts
   @retval false if the machine rejects
 */
bool fsm_sim_nondet_capture(fsm *f, const wchar_t *input, smb_al **capture);

/*******************************************************************************

                                fsm/operations.c

*******************************************************************************/

/**
   @brief Allocate an entirely new copy of an existing FSM.

   The transitions are completely disjoint, so freeing them in the original will
   not affect the copy.
   @param f The FSM to copy
   @return A copy of the FSM
 */
fsm *fsm_copy(const fsm *f);
/**
   @brief Copy the transitions and states from src into dest.

   All the states are added, and the transitions are modified with the offset
   determined from the initial number of states in the destination.
 */
void fsm_copy_trans(fsm *dest, const fsm *src);
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
void fsm_concat(fsm *first, const fsm *second);
/**
   @brief Concatenate two FSMs into the first, such that the second is captured.
   @see fsm_concat()
 */
void fsm_concat_capture(fsm *first, const fsm *second);
/**
   @brief Construct the union of the two FSMs into the first one.

   This function creates an FSM that will accept any string accepted by the
   first OR the second one.  The FSM is constructed in place of the first
   machine, modifying the parameter.  If this is not what you want, make a copy
   of the first one, and pass the copy instead.
   @param first The first FSM to union.  Will be modified with the result.
   @param second The second FSM to union
 */
void fsm_union(fsm *first, const fsm *second);
/**
   @brief Modify an FSM to accept 0 or more occurrences of it.

   This modifies the existing FSM to accept L*, where L is the language accepted
   by the machine.
   @param f The FSM to perform Kleene star on
 */
void fsm_kleene(fsm *f);

#endif
