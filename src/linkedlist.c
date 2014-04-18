/*******************************************************************************

  File:         linkedlist.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  A linked list data structure

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "libstephen.h"

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
   Removes the given node, reassigning the links to and from it.
 */
void ll_remove_node(struct smb_ll *list, struct smb_ll_node *theNode)
{
  struct smb_ll_node *previous = theNode->prev;
  struct smb_ll_node *next = theNode->next;
  if (previous) {
    previous->next = next;
  } else {
    list->head = next;
  }
  if (next) {
    next->prev = previous;
  } else {
    list->tail = previous;
  }
  free(theNode);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(struct smb_ll_node));
}

/**
   ll_navigate:

   Navigates to the given index in the list.

  # Parameters #

  - struct smb_ll *list: the list

  - int index: the index to find in the list

  # Returns #

  A pointer to the node navigated to. NULL if the index was out of range.

  # Raises #

  Does not clear errors.  Raises INDEX_ERROR if the given index was out of
  range.
 */
struct smb_ll_node * ll_navigate(struct smb_ll *list, int index)
{
  struct smb_ll_node *node = list->head;
  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    return NULL;
  }
  int i = 0;
  while (i < index) {
    node = node->next;
    i++;
  }
  return node;
}

/**
   ll_create_node:

   Allocates space for a node with the given data.  Takes care of the malloc
   counter stuff too.

   # Parameters #
   
   - DATA data: the data to insert into the new node.

   # Returns #
 
   A pointer to the node created.  If the node could not be created, returns
   null.

   # Raises #

   Does not clear errors.  Raises ALLOCATION_ERROR if malloc() fails.  In that
   case, return value is NULL.
 */
struct smb_ll_node *ll_create_node(DATA data)
{
  struct smb_ll_node *newNode = (struct smb_ll_node*) malloc(sizeof(struct smb_ll_node));
  
  if (!newNode) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }

  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = NULL;
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(struct smb_ll_node));
  return newNode;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

void ll_init(struct smb_ll *newList)
{
  newList->length = 0;
  newList->head = NULL;
  newList->tail = NULL;
}

struct smb_ll *ll_create()
{
  CLEAR_ALL_ERRORS;

  struct smb_ll *newList = (struct smb_ll *) malloc(sizeof(struct smb_ll));

  if (!newList) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(struct smb_ll));

  ll_init(newList);

  return newList;
}

void ll_append(struct smb_ll *list, DATA newData)
{
  CLEAR_ALL_ERRORS;
  
  // Create the new node
  struct smb_ll_node *newNode = ll_create_node(newData);

  if (!newNode) {
    // ALLOCATION_ERROR has been raised by ll_create_node.  Pass it on to the
    // user.
    return;
  }

  // Get the last node in the list
  struct smb_ll_node *lastNode = list->tail;

  // Link them together
  if (lastNode)
    lastNode->next = newNode;
  else
    list->head = newNode;
  newNode->prev = lastNode;
  list->tail = newNode;
  list->length++;
}

void ll_prepend(struct smb_ll *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  // Create the new struct smb_ll_node
  struct smb_ll_node *newNode = ll_create_node(newData);
  if (!newNode) {
    // ALLOCATION_ERROR has been raised by ll_create_node.  Pass it on to the
    // user.
    return;
  }

  struct smb_ll_node *firstNode = list->head;
  newNode->next = firstNode;
  if (firstNode)
    firstNode->prev = newNode;
  else
    list->tail = newNode;
  list->head = newNode;
  list->length++;
}

void ll_push_back(struct smb_ll *list, DATA newData)
{
  ll_append(list, newData);
}

DATA ll_pop_back(struct smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  struct smb_ll_node *lastNode = list->tail;
  if (lastNode) {
    DATA toReturn = lastNode->data;
    ll_remove_node(list, lastNode);
    list->length--;
    return toReturn;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

DATA ll_peek_back(struct smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  struct smb_ll_node *lastNode = list->tail;
  if (lastNode) {
    return lastNode->data;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

void ll_push_front(struct smb_ll *list, DATA newData)
{
  ll_prepend(list, newData);
}

DATA ll_pop_front(struct smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  struct smb_ll_node *firstNode = list->head;
  if (firstNode) {
    DATA toReturn = firstNode->data;
    ll_remove_node(list, firstNode);
    list->length--;
    return toReturn;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

DATA ll_peek_front(struct smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  struct smb_ll_node *firstNode = list->head;
  if (firstNode) {
    return firstNode->data;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

DATA ll_get(struct smb_ll *list, int index)
{
  CLEAR_ALL_ERRORS;

  // Navigate to that position in the node.
  struct smb_ll_node * theNode = ll_navigate(list, index);
  if (theNode)
    return theNode->data;

  // There is no good return value to signify that the index was out of range.
  // However, the ll_navigate function raised INDEX_ERROR, so calling code can
  // check that.  We still need to return a "mock" set of data though.
  DATA mockData;
  return mockData;
}

void ll_remove(struct smb_ll *list, int index)
{
  // Fond the node
  struct smb_ll_node *theNode = ll_navigate(list, index);
  if (!theNode) {
    return; // Return the INDEX_ERROR
  }
  // Remove it (managing the links and the list header)
  ll_remove_node(list, theNode);
  list->length--;
}

void ll_insert(struct smb_ll *list, int index, DATA newData)
{
  CLEAR_ALL_ERRORS;

  if (index <= 0) {
    ll_prepend(list, newData);
  } else if (index >= list->length) {
    ll_append(list, newData);
  } else {
    // Valid territory
    struct smb_ll_node *newNode = ll_create_node(newData);
    if (!newNode) {
      return; // Return the allocation error.
    }
    struct smb_ll_node *current = ll_navigate(list, index);
    current->prev->next = newNode;
    newNode->prev = current->prev;
    newNode->next = current;
    current->prev = newNode;
    list->length++;
  }
}

void ll_destroy(struct smb_ll *list)
{
  // Iterate through each node, deleting them as we go
  struct smb_ll_node *iter = list->head;
  struct smb_ll_node *temp;
  while (iter) {
    temp = iter->next;
    ll_remove_node(list, iter);
    iter = temp;
  }
}

void ll_delete(struct smb_ll *list)
{
  ll_destroy(list);
  // Free the list header
  free(list);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(struct smb_ll));
}

void ll_set(struct smb_ll *list, int index, DATA newData)
{
  struct smb_ll_node *current = ll_navigate(list, index);
  if (current) {
    current->data = newData;
  } else {
    RAISE(INDEX_ERROR);
  }
}

int ll_length(struct smb_ll *list)
{
  return list->length;
}

struct smb_ll_iter ll_get_iter(struct smb_ll *list)
{
  struct smb_ll_iter iter;
  iter.list = list;
  iter.current = list->head;
  iter.index = 0;
  return iter;
}

DATA ll_iter_next(struct smb_ll_iter *iter)
{
  iter->current = iter->current->next;
  iter->index++;
  return ll_iter_curr(iter);
}

DATA ll_iter_prev(struct smb_ll_iter *iter)
{
  iter->current = iter->current->prev;
  iter->index--;
  return ll_iter_curr(iter);
}

DATA ll_iter_curr(struct smb_ll_iter *iter)
{
  if (iter && iter->current)
    return iter->current->data;
  
  DATA mockData;
  return mockData;
}

int ll_iter_has_next(struct smb_ll_iter *iter)
{
  if (iter && iter->current && iter->current->next)
    return 1;

  return 0;
}

int ll_iter_has_prev(struct smb_ll_iter *iter)
{
  if (iter && iter->current && iter->current->prev)
    return 1;
  
  return 0;
}

int ll_iter_valid(struct smb_ll_iter *iter)
{
  if (iter && iter->current)
    return 1;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Adapter functions for generic list structure

void ll_append_adapter(struct smb_list *l, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_append(list, newData);
}

void ll_prepend_adapter(struct smb_list *l, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_prepend(list, newData);
}

DATA ll_get_adapter(struct smb_list *l, int index)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_get(list, index);
}

void ll_remove_adapter(struct smb_list *l, int index)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_remove(list, index);
}

void ll_insert_adapter(struct smb_list *l, int index, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_insert(list, index, newData);
}

void ll_delete_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  ll_delete(list);
  l->data = NULL;
  return;
}

void ll_set_adapter(struct smb_list *l, int index, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_set(list, index, newData);
}

void ll_push_back_adapter(struct smb_list *l, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_push_back(list, newData);
}

DATA ll_pop_back_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_pop_back(list);
}

DATA ll_peek_back_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_peek_back(list);
}

void ll_push_front_adapter(struct smb_list *l, DATA newData)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return  ll_push_front(list, newData);
}

DATA ll_pop_front_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_pop_front(list);
}

DATA ll_peek_front_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_peek_front(list);
}

int ll_length_adapter(struct smb_list *l)
{
  struct smb_ll *list = (struct smb_ll*) (l->data);
  return ll_length(list);
}

void ll_fill_functions(struct smb_list *genericList)
{
  genericList->append = ll_append_adapter;
  genericList->prepend = ll_prepend_adapter;
  genericList->get = ll_get_adapter;
  genericList->remove = ll_remove_adapter;
  genericList->insert = ll_insert_adapter;
  genericList->delete = ll_delete_adapter;
  genericList->push_back = ll_push_back_adapter;
  genericList->pop_back = ll_pop_back_adapter;
  genericList->peek_back = ll_peek_back_adapter;
  genericList->push_front = ll_push_front_adapter;
  genericList->pop_front = ll_pop_front_adapter;
  genericList->peek_front = ll_peek_front_adapter;
  genericList->length = ll_length_adapter;
  genericList->set = ll_set_adapter;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GENERIC LIST FUNCTIONS

struct smb_list ll_cast_to_list(struct smb_ll *list)
{
  struct smb_list genericList;
  genericList.data = list;
  
  ll_fill_functions(&genericList);

  return genericList;
}

struct smb_list ll_create_list()
{
  struct smb_ll *list = ll_create();

  return ll_cast_to_list(list);
}
