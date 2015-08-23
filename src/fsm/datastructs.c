/***************************************************************************//**

  @file         fsm/datastructs.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Data structure function declarations for FSMs.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdlib.h>     // free
#include <assert.h>     // assert()

#include "libstephen/fsm.h"
#include "libstephen/al.h"
#include "libstephen/log.h"

////////////////////////////////////////////////////////////////////////////////
// fsm_trans Fundamental Functions
////////////////////////////////////////////////////////////////////////////////

void fsm_trans_init(fsm_trans *ft, int n, unsigned int flags, int dest)
{
  int i;
  ft->flags = flags;

  // Allocate space for the range plus null terminator
  ft->start = smb_new(wchar_t, n + 1);
  ft->end = smb_new(wchar_t, n + 1);

  for (i = 0; i < n; i++) {
    ft->start[i] = WEOF;
    ft->end[i] = WEOF;
  }
  ft->start[n] = L'\0';
  ft->end[n] = L'\0';

  ft->dest = dest;
}

fsm_trans *fsm_trans_create(int n, unsigned int flags, int dest)
{
  fsm_trans *ft = smb_new(fsm_trans, 1);
  fsm_trans_init(ft, n, flags, dest);
  return ft;
}

void fsm_trans_destroy(fsm_trans *ft)
{
  smb_free(ft->start);
  smb_free(ft->end);
}

void fsm_trans_delete(fsm_trans *ft)
{
  fsm_trans_destroy(ft);
  smb_free(ft);
}

////////////////////////////////////////////////////////////////////////////////
// fsm_trans Utilities
////////////////////////////////////////////////////////////////////////////////

void fsm_trans_init_single(fsm_trans *ft, wchar_t start, wchar_t end,
                           unsigned int flags, int dest)
{
  fsm_trans_init(ft, 1, flags, dest);
  ft->start[0] = start;
  ft->end[0] = end;
}

fsm_trans *fsm_trans_create_single(wchar_t start, wchar_t end,
                                   unsigned int flags, int dest)
{
  fsm_trans *ft = fsm_trans_create(1, flags, dest);
  ft->start[0] = start;
  ft->end[0] = end;
  return ft;
}

bool fsm_trans_check(const fsm_trans *ft, wchar_t c)
{
  wchar_t *start = ft->start, *end = ft->end;

  while (start && end && *start != '\0' && *end != '\0') {
    if (c >= *start && c <= *end) {
      return !FLAG_CHECK(ft->flags, FSM_TRANS_NEGATIVE);
    }
    start++; end++;
  }

  return FLAG_CHECK(ft->flags, FSM_TRANS_NEGATIVE);
}

fsm_trans *fsm_trans_copy(const fsm_trans *ft)
{
  int i, rangeSize = wcslen(ft->start);
  fsm_trans *new = fsm_trans_create(rangeSize, ft->flags, ft->dest);
  for (i = 0; i < rangeSize; i++) {
    new->start[i] = ft->start[i];
    new->end[i] = ft->end[i];
  }
  return new;
}

////////////////////////////////////////////////////////////////////////////////
// fsm Fundamental Functions
////////////////////////////////////////////////////////////////////////////////

void fsm_init(fsm *f)
{
  f->start = -1;
  al_init(&f->transitions);
  al_init(&f->accepting);
}

fsm *fsm_create(void)
{
  fsm *f = smb_new(fsm, 1);
  fsm_init(f);
  return f;
}

void fsm_destroy(fsm *f, bool free_transitions)
{
  int i, j;
  smb_status status = SMB_SUCCESS;
  smb_al *list;

  for (i = 0; i < al_length(&f->transitions); i++) {
    list = (smb_al *) al_get(&f->transitions, i, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    if (free_transitions) {
      for (j = 0; j < al_length(list); j++) {
        fsm_trans_delete((fsm_trans *) al_get(list, j, &status).data_ptr);
        assert(status == SMB_SUCCESS);
      }
    }
    al_delete(list);
  }

  al_destroy(&f->transitions);
  al_destroy(&f->accepting);
}

void fsm_delete(fsm *f, bool free_transitions)
{
  fsm_destroy(f, free_transitions);
  smb_free(f);
}

////////////////////////////////////////////////////////////////////////////////
// fsm Utilities
////////////////////////////////////////////////////////////////////////////////

fsm *fsm_create_single_char(wchar_t character)
{
  fsm *f = fsm_create();
  int s0 = fsm_add_state(f, false);
  int s1 = fsm_add_state(f, true);
  fsm_add_single(f, s0, s1, character, character, 0);
  f->start= s0;
  return f;
}

int fsm_add_state(fsm *f, bool accepting)
{
  DATA d;
  int index;
  d.data_ptr = (void *) al_create();
  al_append(&f->transitions, d);
  index = al_length(&f->transitions) - 1;
  if (accepting) {
    d.data_llint = index;
    al_append(&f->accepting, d);
  }
  return index;
}

void fsm_add_trans(fsm *f, int state, const fsm_trans *ft)
{
  DATA d;
  smb_status status = SMB_SUCCESS;
  smb_al *list = (smb_al *) al_get(&f->transitions, state, &status).data_ptr;
  assert(status == SMB_SUCCESS);
  // TODO: Change this from an assertion to an error.
  d.data_ptr = (void *) ft;
  al_append(list, d);
}

fsm_trans *fsm_add_single(fsm *f, int from, int to, wchar_t start, wchar_t end,
                          unsigned int flags)
{
  fsm_trans *new = fsm_trans_create_single(start, end, flags, to);
  fsm_add_trans(f, from, new);
  return new;
}

////////////////////////////////////////////////////////////////////////////////
// fsm_sim Fundamental Functions
////////////////////////////////////////////////////////////////////////////////

void fsm_sim_init(fsm_sim *fs, fsm *f, smb_al *curr)
{
  fs->f = f;
  fs->curr = curr;
  fs->cap = al_create();
  al_append(fs->cap, PTR(al_create()));
}

fsm_sim *fsm_sim_create(fsm *f, smb_al *curr)
{
  fsm_sim *fs = smb_new(fsm_sim, 1);
  fsm_sim_init(fs, f, curr);
  return fs;
}

void fsm_sim_destroy(fsm_sim *fs, bool free_curr)
{
  smb_iter it;
  smb_status status = SMB_SUCCESS;
  smb_al *cap;
  it = al_get_iter(fs->cap);
  while (it.has_next(&it)) {
    cap = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    al_delete(cap);
  }
  al_delete(fs->cap);
  if (free_curr) {
    al_delete(fs->curr);
  }
}

void fsm_sim_delete(fsm_sim *fs, bool free_curr)
{
  fsm_sim_destroy(fs, free_curr);
  smb_free(fs);
}
