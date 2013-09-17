/*******************************************************************************

  File:         linkedlist.h

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  A linked list data structure

*******************************************************************************/

#include "../generic.h"

typedef struct ll_node 
{
  struct ll_node *prev;
  struct ll_node *next;
  DATA data;

} NODE;

typedef struct ll_obj
{
  NODE *head;
  NODE *tail;
  int length;

} LINKED_LIST;

typedef struct ll_iter
{
  LINKED_LIST *list;
  NODE *current;
  int index;

} LL_ITERATOR;

/**
   Creates a new list with the given data.

   DATA newData - the data to store.

   return - a pointer to the list.
 */
LINKED_LIST *ll_create(DATA newData);

/**
   Creates a new list empty.
 */
LINKED_LIST *ll_create_empty();

/**
   Append the given data to the end of the list.
 */
void ll_append(LINKED_LIST *list, DATA newData);

/**
   Prepend the given data to the beginning of the list.
 */
void ll_prepend(LINKED_LIST *list, DATA newData);

/**
   Push the data to the back of the list.  An alias for ll_append.
 */
void ll_push_back(LINKED_LIST *list, DATA newData);

/**
   Pop data from the back of the list.
 */
DATA ll_pop_back(LINKED_LIST *list);

/**
   Peek at the back of the list.
 */
DATA ll_peek_back(LINKED_LIST *list);

/**
   Push the data to the front of the list.  An alias for ll_prepend.
 */
void ll_push_front(LINKED_LIST *list, DATA newData);

/**
   Pop the data from the front of the list.
 */
DATA ll_pop_front(LINKED_LIST *list);

/**
   Peek at the front of the list.
 */
DATA ll_peek_front(LINKED_LIST *list);

/**
   Gets the data from the given index.  However, there is no guarantee that the
   index was valid.  An empty DATA object is returned in that case.
 */
DATA ll_get(LINKED_LIST *list, int index);

/**
   Removes the node at the given index.
 */
int ll_remove(LINKED_LIST *list, int index);

/**
   Inserts the item at the specified location in the list, pushing back
   elements.
 */
void ll_insert(LINKED_LIST *list, int index, DATA newData);

/**
   Removes the linked list.
 */
void ll_delete(LINKED_LIST *list);

/**
   Get an iterator for the linked list.
 */
LL_ITERATOR ll_get_iter(LINKED_LIST *list);

/**
   Advance the iterator and return the data at it.
 */
DATA ll_iter_next(LL_ITERATOR *iterator);

/**
   Move the iterator back and return the data at it.
 */
DATA ll_iter_prev(LL_ITERATOR *iterator);

/**
   Get the current data.
 */
DATA ll_iter_curr(LL_ITERATOR *iterator);

/**
   Check if the iterator can be advanced.
 */
int ll_iter_has_next(LL_ITERATOR *iterator);

/**
  Check if the iterator can be moved back.
 */
int ll_iter_has_prev(LL_ITERATOR *iterator);

/**
   Check if the iterator is valid.
 */
int ll_iter_valid(LL_ITERATOR *iterator);
