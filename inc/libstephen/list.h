/***************************************************************************//**

  @file         list.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: List Interface

  @copyright    Copyright (c) 2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef LIBSTEPHEN_LIST_H
#define LIBSTEPHEN_LIST_H

#include <stdbool.h>

#include "base.h"  /* DATA */

/**
   @brief A generic list data structure.

   Can represent an array list or a linked list.  Uses function pointers to hide
   the implementation.  Has heavyweight memory requirements (many pointers).
   Function calls must be made like this:

       list->functionName(list, <params...>)
 */
typedef struct smb_list
{
  /**
     @brief A pointer to implementation-specific data.

     This data is used by the rest of the functions in the struct to perform all
     required actions.
   */
  void *data;

  /**
     @see ll_append @see al_append
   */
  void (*append)(struct smb_list *l, DATA newData, smb_status *status);

  /**
     @see ll_prepend @see al_prepend
   */
  void (*prepend)(struct smb_list *l, DATA newData, smb_status *status);

  /**
     @see ll_get @see al_get
   */
  DATA (*get)(const struct smb_list *l, int index, smb_status *status);

  /**
     @see ll_set @see al_set
   */
  void (*set)(struct smb_list *l, int index, DATA newData, smb_status *status);

  /**
     @see ll_remove @see al_remove
   */
  void (*remove)(struct smb_list *l, int index, smb_status *status);

  /**
     @see ll_insert @see al_insert
   */
  void (*insert)(struct smb_list *l, int index, DATA newData, smb_status *status);

  /**
     @see ll_delete @see al_delete
   */
  void (*delete)(struct smb_list *l);

  /**
     @see ll_length @see al_length
   */
  int (*length)(const struct smb_list *l);

  /**
     @see ll_push_back @see al_push_back
   */
  void (*push_back)(struct smb_list *l, DATA newData, smb_status *status);

  /**
     @see ll_pop_back @see al_pop_back
   */
  DATA (*pop_back)(struct smb_list *l, smb_status *status);

  /**
     @see ll_peek_back @see al_peek_back
   */
  DATA (*peek_back)(struct smb_list *l, smb_status *status);

  /**
     @see ll_push_front @see al_push_front
   */
  void (*push_front)(struct smb_list *l, DATA newData, smb_status *status);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*pop_front)(struct smb_list *l, smb_status *status);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*peek_front)(struct smb_list *l, smb_status *status);

} smb_list;

/**
   @brief A generic iterator type.

   This is a generic iterator type.  It should be implemented for any data
   structure that has some semblance of sequential access.  It can also be used
   to implement a (lazy) generator.  It only provides one directional, read-only
   access, which makes it apply to a more general set of data structures and
   generators.

   The semantics of the iterator are as follows: An iterator points *between*
   any two elements in the sequential list.  Its current index is the index of
   the element it will return next.
 */
typedef struct smb_iter {

  /**
     @brief The data structure this iterator refers to.

     This field is implementation-specific to any iterator.  Behavior is
     undefined when it is modified.  It is intended (but not required) to be
     used by the iterator to store a reference to the data structure.
   */
  const void *ds;

  /**
     @brief The state of the iterator.

     This field is implementation-specific to any iterator.  Behavior is
     undefined when it is modified.  It is intended (but not required) to store
     a state variable of the iterator.
   */
  DATA state;

  /**
     @brief The zero-based index of the iterator.

     This field should be automatically updated by the iterator implementation
     as it operates, requiring no programmer intervention.  It should not be
     modified, but may be accessed.  It stores the index of the element that
     will be returned next.
   */
  int index;

  /**
     @brief Returns the next element of the iteration.
     @param iter The iterator being used.
     @return The next element of the iteration.
   */
  DATA (*next)(struct smb_iter *iter, smb_status *status);

  /**
     @brief Returns whether the iteration has a next element.
     @param iter The iterator being used.
     @return Whether the iteration has a next element.
   */
  bool (*has_next)(struct smb_iter *iter);

  /**
     @brief Frees any resources held by the iterator (but not the iterator).
     @param iter The iterator being used.
     @param free_src Whether to free the data structure used as the source.
   */
  void (*destroy)(struct smb_iter *iter);

  /**
     @brief Frees any resources held by the iterator, and the iterator.
     @param iter The iterator being used.
     @param free_src Whether to free the data structure used as the source.
   */
  void (*delete)(struct smb_iter *iter);

} smb_iter;

void iter_print(smb_iter it, FILE *f, DATA_PRINTER printer);

#endif // LIBSTEPHEN_LIST_H
