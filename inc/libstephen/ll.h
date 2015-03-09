/***************************************************************************//**

  @file         ll.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Linked Lists.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

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
DATA ll_pop_back(smb_ll *list, smb_status *status);
DATA ll_peek_back(smb_ll *list, smb_status *status);
void ll_push_front(smb_ll *list, DATA newData);
DATA ll_pop_front(smb_ll *list, smb_status *status);
DATA ll_peek_front(smb_ll *list, smb_status *status);
DATA ll_get(const smb_ll *list, int index, smb_status *status);
void ll_remove(smb_ll *list, int index, smb_status *status);
void ll_insert(smb_ll *list, int index, DATA newData);
void ll_set(smb_ll *list, int index, DATA newData, smb_status *status);
int  ll_length(const smb_ll *list);
int  ll_index_of(const smb_ll *list, DATA d, DATA_COMPARE comp);

smb_iter ll_get_iter(const smb_ll *list);

#endif // LIBSTEPHEN_LL_H
