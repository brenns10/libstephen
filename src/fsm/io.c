/***************************************************************************//**

  @file         fsm/io.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        IO Functions for FSMs.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <stdbool.h>
#include <assert.h>

#include "str.h"
#include "libstephen/al.h"
#include "libstephen/ll.h"
#include "libstephen/cb.h"
#include "libstephen/str.h"
#include "libstephen/fsm.h"

/**
   @brief Expand f to have as many states as referenced in the transition.

   If either dst or src is greater than the number of states in f, add more
   states to f to match this transition.
 */
static void fsm_extend_states(fsm *f, int src, int dst)
{
  int max = src > dst ? src : dst;
  while (al_length(&f->transitions) < max + 1) {
    fsm_add_state(f, false);
  }
}

/**
   @brief Parse the "accept: %d" lines in a fsm spec.

   @param lines List of lines in the spec.
   @param f FSM we are currently building.
   @param status Status variable.
 */
static void fsm_parseacceptlines(smb_ll *lines, fsm *f, smb_status *status)
{
  wchar_t *line;
  DATA d;
  int state;

  // Iterate through lines
  d = ll_pop_front(lines, status);
  while (*status == SMB_SUCCESS) {
    // Get the next line.
    line = d.data_ptr;
    if (swscanf(line, L"accept:%d", &state) == 1) {
      // If we can successfully read an accept state, add it.
      d.data_llint = state;
      al_append(&f->accepting, d);
    } else {
      // Otherwise, we must be done with accept states, so put the line back
      // into the list and break out of the loop.
      ll_push_front(lines, d);
      break;
    }
    // Pop the next line off, if it exists.
    d = ll_pop_front(lines, status);
  }
  *status = SMB_SUCCESS; // if we ran out of lines, it's not an error
}

/**
   @brief Parse a transition from a single line.
   @param line The line to parse.
   @param f The FSM to put it in when we are done.
   @param status Status variable.
 */
static void fsm_parsetrans(wchar_t *line, fsm *f, smb_status *status)
{
  int src_state, dst_state, type, numread, i, length;
  wchar_t src_char, dst_char, type_char;
  smb_ll sources, destinations;
  DATA d;
  smb_iter src_it, dst_it;
  fsm_trans *t;
  ll_init(&sources);
  ll_init(&destinations);

  // First, get the source state, destination state, and type.
  if (swscanf(line, L"%d-%d:%lc%n", &src_state, &dst_state,
              &type_char, &numread) != 3) {
    goto error;
  }
  if (type_char != L'+' && type_char != L'-') goto error;
  type = type_char == L'+' ? 0 : FSM_TRANS_NEGATIVE;
  length = wcslen(line);

  // Now, we initialize some lists to contain the range sources and
  // destinations, and begin reading these ranges.
  while (numread < length) {
    // add one to skip the hyphen! ------------------------------------v
    numread += read_wchar(line + numread, length-numread, &src_char) + 1;
    d.data_llint = src_char;
    ll_append(&sources, d);
    if (numread >= length) goto error;
    numread += read_wchar(line + numread, length-numread,  &dst_char);
    d.data_llint = dst_char;
    ll_append(&destinations, d);
    if (dst_char == WEOF || src_char == WEOF) goto error;
  }

  // Now that we've read them all, we can copy them into a new fsm_trans.
  src_it = ll_get_iter(&sources);
  dst_it = ll_get_iter(&destinations);
  t = fsm_trans_create(ll_length(&sources), type, dst_state);
  i = 0;
  while (src_it.has_next(&src_it)) {
    d = src_it.next(&src_it, status);
    assert(*status == SMB_SUCCESS); // should be true
    t->start[i] = d.data_llint;
    d = dst_it.next(&dst_it, status);
    assert(*status == SMB_SUCCESS); // should be true
    t->end[i] = d.data_llint;
    i++;
  }

  ll_destroy(&sources);
  ll_destroy(&destinations);
  fsm_extend_states(f, src_state, dst_state);
  fsm_add_trans(f, src_state, t);
  return;

 error:
  *status = CKY_MALFORMED_TRANS;
  ll_destroy(&sources);
  ll_destroy(&destinations);
}

/**
   @brief Parse the remaining lines from the line list as transitions.
   @param lines The line list.
   @param f The FSM we're building.
   @param status Status variable.
 */
static void fsm_parsetranslines(smb_ll *lines, fsm *f, smb_status *status)
{
  wchar_t *line;
  DATA d;

  d = ll_pop_front(lines, status);
  while (*status == SMB_SUCCESS) {
    line = d.data_ptr;
    fsm_parsetrans(line, f, status);
    if (*status != SMB_SUCCESS) return;
    d = ll_pop_front(lines, status);
  }
  *status = SMB_SUCCESS;  // running out of lines is not an error!
}

/**
   @brief Return a FSM parsed from a list of lines.
   @param lines The list of lines.
   @param status Status variable
 */
static fsm *fsm_parselines(smb_ll *lines, smb_status *status)
{
  wchar_t *line;
  fsm *f;
  int state;
  DATA d;

  // TODO: probably use an smb_status for the error handling.
  if (ll_length(lines) < 1) {
    *status = CKY_TOO_FEW_LINES;
    return NULL;
  }

  // Allocate FSM and get the first line.
  f = fsm_create();
  d = ll_pop_front(lines, status);
  assert(*status == SMB_SUCCESS); // must be at least one line
  line = d.data_ptr;

  // The first line may be a "start:" line, or else we assume start:0.
  if (swscanf(line, L"start:%d", &state) == 1) {
    f->start = state;
  } else {
    // This must not have been a start state line. We'll assume the start state
    // is 0 and continue.
    f->start = 0;
    ll_push_front(lines, d);
  }

  fsm_parseacceptlines(lines, f, status);
  fsm_parsetranslines(lines, f, status);
  if (*status != SMB_SUCCESS) {
    fsm_delete(f, true);
    f = NULL;
  }
  return f;
}

/**
   @brief Read a Finite State Machine from its text representation.

   All fsm's can be printed using fsm_print().  This function *should* be able
   to take any such string representation and convert it back into an equivalent
   fsm again.
   @param source The string to read in.
   @param status Status variable for error reporting.
   @returns New FSM, or NULL on error.
   @exception CKY_TOO_FEW_LINES if there isn't at least one line.
   @exception CKY_MALFORMED_TRANS if there is a problem reading a transition.
 */
fsm *fsm_read(const wchar_t *source, smb_status *status)
{
  smb_ll *lines;
  fsm *f;

  // Duplicate the string (wcsdup is pretty nonstandard...)
  wchar_t *copy = smb_new(wchar_t, wcslen(source) + 1);
  wcscpy(copy, source);

  // Split the string.
  lines = split_linesw(copy);

  // could do some further removal of empty lines, etc. here

  // Parse the fsm and return it.
  f = fsm_parselines(lines, status);
  smb_free(copy);
  ll_delete(lines);
  return f;
}

/**
   @brief Return a wchar_t* representation of the fsm (suitable for printing).
   @param f The FSM to print
   @return The string version of the FSM.
 */
wchar_t *fsm_str(const fsm *f)
{
  #define CHARS_PER_DECLARATION 8
  #define CHARS_PER_TRANSITION  12
  #define TRANSITIONS_PER_STATE  2
  smb_status status;
  int i, j;
  smb_al *list;
  wchar_t *start, *end;
  fsm_trans *ft;
  wcbuf wc;
  // Initialize our output buffer to have approximately enough space.  This is
  // strictly an estimate, but the character buffer will handle reallocation if
  // we're wrong.
  wcb_init(&wc, CHARS_PER_DECLARATION * (1 + al_length(&f->accepting)) +
           CHARS_PER_TRANSITION * al_length(&f->transitions) *
           TRANSITIONS_PER_STATE);

  // Print the start state
  wcb_printf(&wc, L"start:%d\n", f->start);

  // Print all the accepting states
  for (i = 0; i < al_length(&f->accepting); i++) {
    wcb_printf(&wc, L"accept:%Ld\n",
               al_get(&f->accepting, i, &status).data_llint);
  }

  // For each state's transition list
  for (i = 0; i < al_length(&f->transitions); i++) {

    // For each transition in that list
    list = (smb_al *) al_get(&f->transitions, i, &status).data_ptr;
    for (j = 0; j < al_length(list); j++) {

      // Print the transition and whether it's positive or negative.
      ft = (fsm_trans *) al_get(list, j, &status).data_ptr;
      wcb_printf(&wc, L"%d-%d:%c", i, ft->dest,
                 (FLAG_CHECK(ft->flags, FSM_TRANS_NEGATIVE) ? '-' : '+'));

      // For every range in the transition, print it followed by a space.
      for (start = ft->start, end = ft->end; *start != L'\0'; start++, end++) {
        wcb_printf(&wc, L"%ls-%ls", escape_wchar(*start), escape_wchar(*end));
      }
      wcb_concat(&wc, L"\n");
    }
  }
  // Reallocate to exactly the right size for the string.
  wc.buf = smb_renew(wchar_t, wc.buf, wc.length + 1);
  // Since we're returning the string, don't wcb_destroy()!
  return wc.buf;
}

/**
   @brief Print the FSM to the desired output stream.
   @param f FSM to print.
   @param dest Output stream/file to print to.
 */
void fsm_print(const fsm *f, FILE *dest)
{
  wchar_t *str = fsm_str(f);
  fprintf(dest, "%ls", str);
  smb_free(str);
}

/**
   @brief Prints a character in an FSM specification to dot format.

   This function allows for printing out some characters differently than
   others.  In dot format, quotes should be escaped.  Also, epsilon transitions
   should be labelled something readable.

   @param dest Destination file.
   @param c The character to print
 */
void fsm_dot_char(FILE * dest, wchar_t c)
{
  switch (c) {

  case EPSILON:
    // Print epsilon as a string, not a non-existant character!
    fprintf(dest, "eps");
    break;
  case L'\"':
    // Escape quotes.
    fprintf(dest, "\"");
    break;
  default:
    // Print other characters vebatim.
    fprintf(dest, "%lc", c);
    break;
  }
}

/**
   @brief Print an FSM to graphviz dot format.

   Graphviz is a neat CLI tool to create diagrams of graphs.  The dot format is
   an input language for graphs.  This function takes a FSM and prints it to a
   file as a dot file, so that it can then be converted to a image by graphviz.

   @param f The machine to print.
   @param dest The destination file.
 */
void fsm_dot(fsm *f, FILE *dest)
{
  smb_status status;
  int i, j;
  smb_al *list;
  wchar_t *start, *end;
  fsm_trans *ft;

  // Declare a digraph, where the nodes are all boxes.
  fprintf(dest, "digraph regex {\n");
  fprintf(dest, "  node [shape=box];\n");

  // Declare start state as oval
  fprintf(dest, "  s%d [shape=oval];\n", f->start);

  // Declare accepting states as octagons
  for (i = 0; i < al_length(&f->accepting); i++) {
    fprintf(dest, "  s%d [shape=octagon];\n",
            (int)al_get(&f->accepting, i, &status).data_llint);
  }

  // For each state's transition list
  for (i = 0; i < al_length(&f->transitions); i++) {

    // For each transition in that list
    list = (smb_al *) al_get(&f->transitions, i, &status).data_ptr;
    for (j = 0; j < al_length(list); j++) {

      // Declare an edge from source to destination.
      ft = (fsm_trans *) al_get(list, j, &status).data_ptr;
      fprintf(dest, "  s%d -> s%d ", i, ft->dest);

      // Begin writing the label for the transition
      fprintf(dest, "[label=\"(%c) ",
              (FLAG_CHECK(ft->flags, FSM_TRANS_NEGATIVE) ? '-' : '+'));

      // For every character range in the transition description.
      for (start = ft->start, end = ft->end; *start != L'\0'; start++, end++) {

        // Print the range into the label using the filter.
        fsm_dot_char(dest, *start);
        fprintf(dest, "-");
        fsm_dot_char(dest, *end);

        // If this isn't the last range, print a space to separate.
        if (*(start+1) != L'\0') {
          fprintf(stdout, " ");
        }
      }

      // Finish up the label and transition declaration.
      fprintf(dest, "\"];\n");
    }
  }

  // Finish up the entire digraph declaration.
  fprintf(dest, "}\n");
}
