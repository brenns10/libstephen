/***************************************************************************//**

  @file         linkedlist.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        A linked list data structure.

  This linked list data structure provides most basic features necessary in a
  linked list, along with some more advanced ones like iterators, stack
  functionality, and a generic list data structure that can use a different
  implementation.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "libstephen/ll.h"

/*******************************************************************************

                               Private Functions

*******************************************************************************/

/**
   @brief Removes the given node, reassigning the links to and from it.

   Frees the node, in addition no reassigning links.  Once this function is
   called, theNode is invlidated.

   This function is a *private* function, not declared in libstephen.h for a
   reason.  It is only necessary for the implementation functions within this
   file.

   @param list The list that contains this node.
   @param theNode The node to delete.
 */
void ll_remove_node(smb_ll *list, smb_ll_node *theNode)
{
  smb_ll_node *previous = theNode->prev;
  smb_ll_node *next = theNode->next;
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
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ll_node));
}

/**
   @brief Navigates to the given index in the list, returning the correct node,
   or NULL.

   This function is a *private* function, not declared in libstephen.h for a
   reason.

   @param list The list to navigate within.
   @param index The index to find in the list.
   @returns A pointer to the node navigated to.
   @retval NULL if the index was out of range.
   @exception INDEX_ERROR if the given index was out of range.
 */
smb_ll_node * ll_navigate(const smb_ll *list, int index)
{
  smb_ll_node *node = list->head;
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
   @brief Allocates and initializes a node with the given data.

   @param data The data to insert into the new node.

   @return A pointer to the node created.
   @retval NULL if the node couldn't be created.
   @exception ALLOCATION_ERROR if malloc() fails.  In that case, return value is
   NULL.
 */
smb_ll_node *ll_create_node(DATA data)
{
  smb_ll_node *newNode = (smb_ll_node*) malloc(sizeof(smb_ll_node));

  if (!newNode) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }

  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = NULL;
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ll_node));
  return newNode;
}

/*******************************************************************************

                                Public Functions

*******************************************************************************/

/**
   @brief Initializes a new list in memory which has already been allocated.

   @param newList A pointer to the memory to initialize.
 */
void ll_init(smb_ll *newList)
{
  newList->length = 0;
  newList->head = NULL;
  newList->tail = NULL;
}

/**
   @brief Allocates and initializes a new, empty linked list.

   @returns A pointer to the new list.
   @exception ALLOCATION_ERROR If memory allocation fails, returns NULL and
   raises error.
 */
smb_ll *ll_create()
{
  CLEAR_ALL_ERRORS;

  smb_ll *newList = (smb_ll *) malloc(sizeof(smb_ll));

  if (!newList) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ll));

  ll_init(newList);

  return newList;
}

/**
   @brief Frees all the resources held by the linked list without freeing the
   actual pointer to the list.

   If you create a list on the stack and use ll_init to initialize it, calling
   ll_delete will attempt to free your stack memory (bad).  Use this to free all
   the resources of the list without freeing the pointer.

   @param list The list to destroy.
 */
void ll_destroy(smb_ll *list)
{
  // Iterate through each node, deleting them as we go
  smb_ll_node *iter = list->head;
  smb_ll_node *temp;
  while (iter) {
    temp = iter->next;
    ll_remove_node(list, iter);
    iter = temp;
  }
}

/**
   @brief Frees the resources held by the linked list, and the memory allocated
   to the smb_ll object.

   This is equivalent to calling ll_destroy on the pointer, and then calling
   free() on the pointer (so long as you then decrement the malloc counter).

   @param list A pointer to the list to delete.
 */
void ll_delete(smb_ll *list);void ll_delete(smb_ll *list)
{
  ll_destroy(list);
  // Free the list header
  free(list);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ll));
}

/**
   @brief Append the given data to the end of the list.

   @param list A pointer to the list to append to.
   @param newData The data to append.
   @exception ALLOCATION_ERROR
 */
void ll_append(smb_ll *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  // Create the new node
  smb_ll_node *newNode = ll_create_node(newData);

  if (!newNode) {
    // ALLOCATION_ERROR has been raised by ll_create_node.  Pass it on to the
    // user.
    return;
  }

  // Get the last node in the list
  smb_ll_node *lastNode = list->tail;

  // Link them together
  if (lastNode)
    lastNode->next = newNode;
  else
    list->head = newNode;
  newNode->prev = lastNode;
  list->tail = newNode;
  list->length++;
}

/**
   @brief Prepend the given data to the beginning of the list.  All other data
   is shifted forward one index.

   @param list A pointer to the list.
   @param newData The data to prepend.
   @exception ALLOCATION_ERROR
 */
void ll_prepend(smb_ll *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  // Create the new smb_ll_node
  smb_ll_node *newNode = ll_create_node(newData);
  if (!newNode) {
    // ALLOCATION_ERROR has been raised by ll_create_node.  Pass it on to the
    // user.
    return;
  }

  smb_ll_node *firstNode = list->head;
  newNode->next = firstNode;
  if (firstNode)
    firstNode->prev = newNode;
  else
    list->tail = newNode;
  list->head = newNode;
  list->length++;
}

/**
   @brief Push the data to the back of the list.  An alias for ll_append. @see
   ll_append

   Push, of course, refers to its usage in the context of stacks.  In other
   words, the function appends an item at the end fo the list.

   @param list A pointer to the list to push to.
   @param newData The data to push.
   @exception ALLOCATION_ERROR
 */
void ll_push_back(smb_ll *list, DATA newData)
{
  ll_append(list, newData);
}

/**
   @brief Pop data from the back of the list.

   Pop, of course, refers to its usage in the context of stacks.  In other
   words, the function removes the item at the back/end of the list and returns
   it.

   @param list A pointer to the list to pop from.
   @returns The item from the back of the list.
   @exception INDEX_ERROR
 */
DATA ll_pop_back(smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  smb_ll_node *lastNode = list->tail;
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

/**
   @brief Peek at the back of the list.

   Peek, of course, refers to its usage in the context of stacks.  So this
   returns the item at the end/back of the list without removing it.

   @param list A pointer to the list to peek from.
   @returns The item from the back of the list.
   @exception INDEX_ERROR
 */
DATA ll_peek_back(smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  smb_ll_node *lastNode = list->tail;
  if (lastNode) {
    return lastNode->data;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

/**
   @brief Push the data to the front of the list.  An alias for ll_prepend.
   @see ll_push_back If the term 'push' is unfamiliar.

   @param list A pointer to the list.
   @param newData The data to push.
   @exception ALLOCATION_ERROR
 */

void ll_push_front(smb_ll *list, DATA newData)
{
  ll_prepend(list, newData);
}

/**
   @brief Pop the data from the front of the list.
   @see ll_pop_back If the term 'pop' is unfamiliar.

   @param list A pointer to the list to pop from
   @returns The data from the front of the list.
   @exception INDEX_ERROR
 */
DATA ll_pop_front(smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  smb_ll_node *firstNode = list->head;
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

/**
   @brief Peek at the front of the list.
   @see ll_peek_back If the term 'peek' is unfamiliar.

   @param list A pointer to the list to peek from.
   @return The data from the front of the list.
   @exception INDEX_ERROR
 */
DATA ll_peek_front(smb_ll *list)
{
  CLEAR_ALL_ERRORS;

  smb_ll_node *firstNode = list->head;
  if (firstNode) {
    return firstNode->data;
  } else {
    RAISE(INDEX_ERROR); // The list is empty
    DATA mockData;
    return mockData;
  }
}

/**
   @brief Gets the data from the given index.

   However, there is no guarantee that the index was valid.  An empty DATA
   object is returned in that case, and an INDEX_ERROR is raised.

   @param list A pointer to the list to get from.
   @param index The index to get from the list.
   @return The data at the specified index, if it exists.
   @exception INDEX_ERROR
 */
DATA ll_get(const smb_ll *list, int index)
{
  CLEAR_ALL_ERRORS;

  // Navigate to that position in the node.
  smb_ll_node * theNode = ll_navigate(list, index);
  if (theNode)
    return theNode->data;

  // There is no good return value to signify that the index was out of range.
  // However, the ll_navigate function raised INDEX_ERROR, so calling code can
  // check that.  We still need to return a "mock" set of data though.
  DATA mockData;
  return mockData;
}

/**
   @brief Removes the node at the given index, if the index exists.

   If the item is not at the end of the list, the index of every item after this
   one will be shifted down one.

   @param list A pointer to the list to remove from.
   @param index The index to remove from the list.
   @exception INDEX_ERROR
 */
void ll_remove(smb_ll *list, int index)
{
  // Fond the node
  smb_ll_node *theNode = ll_navigate(list, index);
  if (!theNode) {
    return; // Return the INDEX_ERROR
  }
  // Remove it (managing the links and the list header)
  ll_remove_node(list, theNode);
  list->length--;
}

/**
   @brief Inserts an item at the specified location in the list.

   If the location is not the end of the list, then every item at the given
   index and after will be shifted up one index.  If the provided location is
   less than 0, the location will be treated as 0.  If the provided location is
   greater than the length of the list, the item will be added to the end of the
   list.

   @param list A pointer to the list to insert into.
   @param index The index to insert at.
   @param newData The data to insert.
   @exception ALLOCATION_ERROR
 */
void ll_insert(smb_ll *list, int index, DATA newData)
{
  CLEAR_ALL_ERRORS;

  if (index <= 0) {
    ll_prepend(list, newData);
  } else if (index >= list->length) {
    ll_append(list, newData);
  } else {
    // Valid territory
    smb_ll_node *newNode = ll_create_node(newData);
    if (!newNode) {
      return; // Return the allocation error.
    }
    smb_ll_node *current = ll_navigate(list, index);
    current->prev->next = newNode;
    newNode->prev = current->prev;
    newNode->next = current;
    current->prev = newNode;
    list->length++;
  }
}

/**
   @brief Sets an existing element to a new value.

   It is illegal to call ll_set on an out of bounds index.  You must use
   ll_insert for that.

   @param list The list to modify.
   @param index The index of the item to set.
   @param newData The data to set the index to.
   @exception INDEX_ERROR If the index is out of bounds
 */
void ll_set(smb_ll *list, int index, DATA newData)
{
  smb_ll_node *current = ll_navigate(list, index);
  if (current) {
    current->data = newData;
  } else {
    RAISE(INDEX_ERROR);
  }
}

/**
   @brief Returns the length of the given list.

   @param list A pointer to the list
   @returns The length of the list.
 */
int ll_length(const smb_ll *list)
{
  return list->length;
}

/**
   @brief Get the next item in the linked list.

   @param iterator A pointer to the iterator.
   @returns The data at the new location of the iterator.
 */
DATA ll_iter_next(smb_iter *iter)
{
  smb_ll_node *node = iter->state.data_ptr;
  if (node == NULL) {
    return iter->state; // a DATA containing NULL
  }
  iter->state.data_ptr = node->next;
  iter->index++;
  return node->data;
  // TODO: Throw some sort of error when calling next() on finished iterator.
}

/**
   @brief Check if the iterator can be advanced.

   @param iter A pointer to the iterator.
   @returns Whether the iterator can be advanced.
 */
bool ll_iter_has_next(smb_iter *iter)
{
  bool node_clean, index_clean;
  smb_ll_node *node = iter->state.data_ptr;
  const smb_ll *ds = iter->ds;

  // Is there a node to return?
  node_clean = node != NULL;
  // Is the index valid?
  index_clean = iter->index < ll_length(ds);

  // Both should have the same value.  It is impossible for the node to be null
  // when the index is in bounds.  So, assert it.
  assert(node_clean == index_clean);

  return node_clean;
}

/**
   @brief Free the resources held by the iterator.
   @param iter A pointer to the iterator.
   @param free_src Whether to free the source list.
 */
void ll_iter_destroy(smb_iter *iter)
{
  // Nothing to destroy
}

/**
   @brief Free the iterator and its resources.
   @param iter A pointer to the iterator
   @param free_src Whether to free the source list.
 */
void ll_iter_delete(smb_iter *iter)
{
  iter->destroy(iter);
  smb_free(smb_iter, iter, 1);
}

/**
   @brief Get an iterator for the linked list.

   @param list A pointer to the list.
   @returns an iterator
 */
smb_iter ll_get_iter(const smb_ll *list)
{
  smb_iter iter = {
    // Data in the iterator
    .ds = list,
    .state = (DATA) { .data_ptr = list->head },
    .index = 0,

    // Functions
    .next = &ll_iter_next,
    .has_next = &ll_iter_has_next,
    .destroy = &ll_iter_destroy,
    .delete = &ll_iter_delete
  };

  return iter;
}

/*******************************************************************************

                         Linked List Adapter Functions

  These guys are used as the function pointers for the smb_list.  They really
  don't need any documentation.

*******************************************************************************/

void ll_append_adapter(smb_list *l, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_append(list, newData);
}

void ll_prepend_adapter(smb_list *l, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_prepend(list, newData);
}

DATA ll_get_adapter(const smb_list *l, int index)
{
  const smb_ll *list = (const smb_ll*) (l->data);
  return ll_get(list, index);
}

void ll_remove_adapter(smb_list *l, int index)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_remove(list, index);
}

void ll_insert_adapter(smb_list *l, int index, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_insert(list, index, newData);
}

void ll_delete_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_delete(list);
  l->data = NULL;
  return;
}

void ll_set_adapter(smb_list *l, int index, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_set(list, index, newData);
}

void ll_push_back_adapter(smb_list *l, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_push_back(list, newData);
}

DATA ll_pop_back_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_pop_back(list);
}

DATA ll_peek_back_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_peek_back(list);
}

void ll_push_front_adapter(smb_list *l, DATA newData)
{
  smb_ll *list = (smb_ll*) (l->data);
  return  ll_push_front(list, newData);
}

DATA ll_pop_front_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_pop_front(list);
}

DATA ll_peek_front_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_peek_front(list);
}

int ll_length_adapter(const smb_list *l)
{
  const smb_ll *list = (const smb_ll*) (l->data);
  return ll_length(list);
}

/**
   @brief Populate a generic smb_list with function pointers necessary to use
   smb_ll with it.

   Note that this is a *private* function, not defined in libstephen.h for a
   reason.  You shouldn't need the function, as there is library functionality
   that provides it for you.

   @param genericList The smb_list to populate with function pointers.
 */
void ll_fill_functions(smb_list *genericList)
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

/**
   @brief Creates a new, empty list, and returns it as an instance of the
   generic smb_list data structure. @see smb_list

   @returns A generic list interface object.

   @exception ALLOCATION_ERROR.
 */
smb_list ll_create_list()
{
  smb_ll *list = ll_create();

  return ll_cast_to_list(list);
}

/**
   @brief Cast a smb_ll pointer to an instance of the smb_list interface.  @see
   smb_list

   @param list The linked list to cast to a generic list.

   @return A generic list interface object.
 */
smb_list ll_cast_to_list(smb_ll *list)
{
  smb_list genericList;
  genericList.data = list;

  ll_fill_functions(&genericList);

  return genericList;
}
