/***************************************************************************//**

  @file         ll.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Linked Lists.

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

#ifndef LIBSTEPHEN_LL_H
#define LIBSTEPHEN_LL_H

#include <stdbool.h> /* bool */

#include "base.h"  /* DATA     */
#include "list.h"  /* smb_list */

/**
   @brief Node structure for linked list.

   This must be exposed in order for other data types to be public.  This should
   not be used by users of the library.
 */
typedef struct smb_ll_node
{
  /**
     @brief The previous node in the linked list.
   */
  struct smb_ll_node *prev;

  /**
     @brief The next node in the linked list.
   */
  struct smb_ll_node *next;

  /**
     @brief The data stored in this node.
   */
  DATA data;

} smb_ll_node;

/**
   @brief The actual linked list data type.  "Bare" functions return a pointer
   to this structure.
 */
typedef struct smb_ll
{
  /**
     @brief A pointer to the head of the list.
   */
  struct smb_ll_node *head;

  /**
     @brief A pointer to the tail of the list.  Useful for speeding up
     navigation to a particular spot.
   */
  struct smb_ll_node *tail;

  /**
     @brief The number of items stored in the list.
   */
  int length;

} smb_ll;

void ll_init(smb_ll *newList);
smb_ll *ll_create();
void ll_destroy(smb_ll *list);
void ll_delete(smb_ll *list);

smb_list ll_create_list();
smb_list ll_cast_to_list(smb_ll *list);

void ll_append(smb_ll *list, DATA newData);
void ll_prepend(smb_ll *list, DATA newData);
void ll_push_back(smb_ll *list, DATA newData);
DATA ll_pop_back(smb_ll *list);
DATA ll_peek_back(smb_ll *list);
void ll_push_front(smb_ll *list, DATA newData);
DATA ll_pop_front(smb_ll *list);
DATA ll_peek_front(smb_ll *list);
DATA ll_get(const smb_ll *list, int index);
void ll_remove(smb_ll *list, int index);
void ll_insert(smb_ll *list, int index, DATA newData);
void ll_set(smb_ll *list, int index, DATA newData);
int  ll_length(const smb_ll *list);
void ll_print(const smb_ll *list, FILE *f, DATA_PRINTER printer);

smb_iter ll_get_iter(const smb_ll *list);
DATA ll_iter_next(smb_iter *iter);
bool ll_iter_has_next(smb_iter *iter);
void ll_iter_destroy(smb_iter *iter);
void ll_iter_delete(smb_iter *iter);

#endif // LIBSTEPHEN_LL_H
