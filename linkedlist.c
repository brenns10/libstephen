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
void ll_remove_node(LINKED_LIST *list, NODE *theNode)
{
  NODE *previous = theNode->prev;
  NODE *next = theNode->next;
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
  SMB_DECREMENT_MALLOC_COUNTER(1);
}

/**
   Navigates to the given index in the list.

   LINKED_LIST *list - the list

   int index - the index to find in the list

   returns - a pointer to that node. null if the index was out of range.
 */
NODE * ll_navigate(LINKED_LIST *list, int index)
{
  NODE *node = list->head;
  if (index < 0) return NULL;
  else if (index >= list->length) return NULL;
  int i = 0;
  while (i < index) {
    node = node->next;
    i++;
  }
  return node;
}

/**
   Allocates space for a node with the given data.  Takes care of the malloc
   counter stuff too.
 */
NODE *ll_create_node(DATA data)
{
  NODE *newNode = (NODE*) malloc(sizeof(NODE));
  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = NULL;
  SMB_INCREMENT_MALLOC_COUNTER(1);
  return newNode;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

LINKED_LIST *ll_create(DATA newData)
{
  // Allocate space for the struct that represents the list.
  LINKED_LIST *newList = (LINKED_LIST *) malloc(sizeof(LINKED_LIST));
  SMB_INCREMENT_MALLOC_COUNTER(1);

  // Create the first node
  NODE *newNode = ll_create_node(newData);

  // Link things together and setup variables
  newList->head = newNode;
  newList->tail = newNode;
  newList->length = 1;

  return newList;
}

LINKED_LIST *ll_create_empty()
{
  LINKED_LIST *newList = (LINKED_LIST *) malloc(sizeof(LINKED_LIST));
  SMB_INCREMENT_MALLOC_COUNTER(1);
  newList->length = 0;
  newList->head = NULL;
  newList->tail = NULL;
  return newList;
}

void ll_append(LINKED_LIST *list, DATA newData)
{
  // Create the new node
  NODE *newNode = ll_create_node(newData);

  // Get the last node in the list
  NODE *lastNode = list->tail;

  // Link them together
  if (lastNode)
    lastNode->next = newNode;
  else
    list->head = newNode;
  newNode->prev = lastNode;
  list->tail = newNode;
  list->length++;
}

void ll_prepend(LINKED_LIST *list, DATA newData)
{
  // Create the new NODE
  NODE *newNode = ll_create_node(newData);
  
  NODE *firstNode = list->head;
  newNode->next = firstNode;
  if (firstNode)
    firstNode->prev = newNode;
  else
    list->tail = newNode;
  list->head = newNode;
  list->length++;
}

void ll_push_back(LINKED_LIST *list, DATA newData)
{
  ll_append(list, newData);
}

DATA ll_pop_back(LINKED_LIST *list)
{
  NODE *lastNode = list->tail;
  if (lastNode) {
    DATA toReturn = lastNode->data;
    ll_remove_node(list, lastNode);
    list->length--;
    return toReturn;
  } else {
    DATA mockData;
    return mockData;
  }
}

DATA ll_peek_back(LINKED_LIST *list)
{
  NODE *lastNode = list->tail;
  if (lastNode) {
    return lastNode->data;
  } else {
    DATA mockData;
    return mockData;
  }
}

void ll_push_front(LINKED_LIST *list, DATA newData)
{
  ll_prepend(list, newData);
}

DATA ll_pop_front(LINKED_LIST *list)
{
  NODE *firstNode = list->head;
  if (firstNode) {
    DATA toReturn = firstNode->data;
    ll_remove_node(list, firstNode);
    list->length--;
    return toReturn;
  } else {
    DATA mockData;
    return mockData;
  }
}

DATA ll_peek_front(LINKED_LIST *list)
{
  NODE *firstNode = list->head;
  if (firstNode) {
    return firstNode->data;
  } else {
    DATA mockData;
    return mockData;
  }
}

DATA ll_get(LINKED_LIST *list, int index)
{
  // Navigate to that position in the node.
  NODE * theNode = ll_navigate(list, index);
  if (theNode)
    return theNode->data;

  // Unfortunately, there is no good return value to signify that the index was
  // out of bounds.  So we just create a "mock" data thing and return that.
  DATA mockData;
  return mockData;
}

void ll_remove(LINKED_LIST *list, int index)
{
  // Fond the node
  NODE *theNode = ll_navigate(list, index);
  if (!theNode) {
    RAISE(INDEX_ERROR);
    return;
  }
  // Remove it (managing the links and the list header)
  ll_remove_node(list, theNode);
  list->length--;
}

void ll_insert(LINKED_LIST *list, int index, DATA newData)
{
  if (index <= 0) {
    ll_prepend(list, newData);
  } else if (index >= list->length) {
    ll_append(list, newData);
  } else {
    // Valid territory
    NODE *newNode = ll_create_node(newData);
    NODE *current = ll_navigate(list, index);
    current->prev->next = newNode;
    newNode->prev = current->prev;
    newNode->next = current;
    current->prev = newNode;
    list->length++;
  }
}

void ll_delete(LINKED_LIST *list)
{
  // Iterate through each node, deleting them as we go
  NODE *iter = list->head;
  NODE *temp;
  while (iter) {
    temp = iter->next;
    ll_remove_node(list, iter);
    iter = temp;
  }
  // Free the list header
  free(list);
  SMB_DECREMENT_MALLOC_COUNTER(1);
}

void ll_set(LINKED_LIST *list, int index, DATA newData)
{
  NODE *current = ll_navigate(list, index);
  if (current) {
    current->data = newData;
  } else {
    RAISE(INDEX_ERROR);
  }
}

LL_ITERATOR ll_get_iter(LINKED_LIST *list)
{
  LL_ITERATOR iter;
  iter.list = list;
  iter.current = list->head;
  iter.index = 0;
  return iter;
}

DATA ll_iter_next(LL_ITERATOR *iter)
{
  iter->current = iter->current->next;
  iter->index++;
  return ll_iter_curr(iter);
}

DATA ll_iter_prev(LL_ITERATOR *iter)
{
  iter->current = iter->current->prev;
  iter->index--;
  return ll_iter_curr(iter);
}

DATA ll_iter_curr(LL_ITERATOR *iter)
{
  if (iter && iter->current)
    return iter->current->data;
  
  DATA mockData;
  return mockData;
}

int ll_iter_has_next(LL_ITERATOR *iter)
{
  if (iter && iter->current && iter->current->next)
    return 1;

  return 0;
}

int ll_iter_has_prev(LL_ITERATOR *iter)
{
  if (iter && iter->current && iter->current->prev)
    return 1;
  
  return 0;
}

int ll_iter_valid(LL_ITERATOR *iter)
{
  if (iter && iter->current)
    return 1;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Adapter functions for generic list structure

void ll_append_adapter(LIST *l, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_append(list, newData);
}

void ll_prepend_adapter(LIST *l, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_prepend(list, newData);
}

DATA ll_get_adapter(LIST *l, int index)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_get(list, index);
}

void ll_remove_adapter(LIST *l, int index)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_remove(list, index);
}

void ll_insert_adapter(LIST *l, int index, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_insert(list, index, newData);
}

void ll_delete_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  ll_delete(list);
  l->data = NULL;
  return;
}

void ll_set_adapter(LIST *l, int index, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_set(list, index, newData);
}

void ll_push_back_adapter(LIST *l, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_push_back(list, newData);
}

DATA ll_pop_back_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_pop_back(list);
}

DATA ll_peek_back_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_peek_back(list);
}

void ll_push_front_adapter(LIST *l, DATA newData)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return  ll_push_front(list, newData);
}

DATA ll_pop_front_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_pop_front(list);
}

DATA ll_peek_front_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return ll_peek_front(list);
}

int ll_length_adapter(LIST *l)
{
  LINKED_LIST *list = (LINKED_LIST*) (l->data);
  return list->length;
}

void ll_fill_functions(LIST *genericList)
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

LIST ll_cast_to_list(LINKED_LIST *list)
{
  LIST genericList;
  genericList.data = list;
  
  ll_fill_functions(&genericList);

  return genericList;
}

LIST ll_create_list(DATA newData)
{
  LINKED_LIST *list = ll_create(newData);

  return ll_cast_to_list(list);
}

LIST ll_create_empty_list()
{
  LINKED_LIST *list = ll_create_empty();

  return ll_cast_to_list(list);
}
