/*******************************************************************************

  File:         linkedlist.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  A linked list data structure

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "../common_include.h"

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
  int i = 0;
  while (node && i < index) {
    node = node->next;
    i++;
  }
  return node;
}

/**
   Navigates to the end of the list.  Returns a pointer to the last node.
 */
NODE * ll_navigate_end(LINKED_LIST *list)
{
  return list->tail;
}

NODE *ll_create_node(DATA data)
{
  NODE *newNode = (NODE*) malloc(sizeof(NODE));
  newNode->data = data;
  SMB_INCREMENT_MALLOC_COUNTER(1);
  return newNode;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

LINKED_LIST * ll_create(DATA newData)
{
  LINKED_LIST *newList = (LINKED_LIST *) malloc(sizeof(LINKED_LIST));
  SMB_INCREMENT_MALLOC_COUNTER(1);
  NODE *newNode = ll_create_node(newData);
  newList->head = newNode;
  newList->tail = newNode;
  newList->length = 1;
  return newList;
}

void ll_append(LINKED_LIST *list, DATA newData)
{
  NODE *newNode = ll_create_node(newData);
  NODE *lastNode = ll_navigate_end(list);
  lastNode->next = newNode;
  newNode->prev = lastNode;
  list->tail = newNode;
  list->length++;
}

DATA ll_get(LINKED_LIST *list, int index)
{
  NODE * theNode = ll_navigate(list, index);
  if (theNode)
    return theNode->data;
  DATA mockData;
  return mockData;
}

int ll_remove(LINKED_LIST *list, int index)
{
  NODE *theNode = ll_navigate(list, index);
  if (!theNode)
    return 0;
  ll_remove_node(list, theNode);
  list->length--;
  return 1;
}

void ll_delete(LINKED_LIST *list)
{
  NODE *iter = list->head;
  NODE *temp;
  while (iter) {
    temp = iter->next;
    ll_remove_node(list, iter);
    iter = temp;
  }
  free(list);
  SMB_DECREMENT_MALLOC_COUNTER(1);
}
