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
  void (*append)(struct smb_list *l, DATA newData);

  /**
     @see ll_prepend @see al_prepend
   */
  void (*prepend)(struct smb_list *l, DATA newData);

  /**
     @see ll_get @see al_get
   */
  DATA (*get)(const struct smb_list *l, int index);

  /**
     @see ll_set @see al_set
   */
  void (*set)(struct smb_list *l, int index, DATA newData);

  /**
     @see ll_remove @see al_remove
   */
  void (*remove)(struct smb_list *l, int index);

  /**
     @see ll_insert @see al_insert
   */
  void (*insert)(struct smb_list *l, int index, DATA newData);

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
  void (*push_back)(struct smb_list *l, DATA newData);

  /**
     @see ll_pop_back @see al_pop_back
   */
  DATA (*pop_back)(struct smb_list *l);

  /**
     @see ll_peek_back @see al_peek_back
   */
  DATA (*peek_back)(struct smb_list *l);

  /**
     @see ll_push_front @see al_push_front
   */
  void (*push_front)(struct smb_list *l, DATA newData);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*pop_front)(struct smb_list *l);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*peek_front)(struct smb_list *l);

} smb_list;

#endif // LIBSTEPHEN_LIST_H
