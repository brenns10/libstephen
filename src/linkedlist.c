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
   called, the_node is invlidated.

   This function is a *private* function, not declared in libstephen.h for a
   reason.  It is only necessary for the implementation functions within this
   file.

   @param list The list that contains this node.
   @param the_node The node to delete.
 */
void ll_remove_node(smb_ll *list, smb_ll_node *the_node)
{
  smb_ll_node *previous = the_node->prev;
  smb_ll_node *next = the_node->next;
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
  free(the_node);
}

/**
   @brief Navigates to the given index in the list, returning the correct node,
   or NULL.

   This function is a *private* function, not declared in libstephen.h for a
   reason.

   @param list The list to navigate within.
   @param index The index to find in the list.
   @param[out] status Status variable.
   @returns A pointer to the node navigated to.
   @retval NULL if the index was out of range.
   @exception SMB_INDEX_ERROR if the given index was out of range.
 */
smb_ll_node * ll_navigate(const smb_ll *list, int index, smb_status *status)
{
  smb_ll_node *node = list->head;
  if (index < 0 || index >= list->length) {
    *status = SMB_INDEX_ERROR;
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
   @param[out] status Status variable.
   @return A pointer to the node created.
 */
smb_ll_node *ll_create_node(DATA data)
{
  smb_ll_node *new_node = smb_new(smb_ll_node, 1);
  new_node->data = data;
  new_node->next = NULL;
  new_node->prev = NULL;
  return new_node;
}

/*******************************************************************************

                                Public Functions

*******************************************************************************/

/**
   @brief Initializes a new list in memory which has already been allocated.

   No errors are defined for ll_init, but the parameter is used to allow for the
   possibility of future errors.  For now, you may safely `assert(status ==
   SMB_SUCCESS)`.

   @param new_list A pointer to the memory to initialize.
 */
void ll_init(smb_ll *new_list)
{
  new_list->length = 0;
  new_list->head = NULL;
  new_list->tail = NULL;
}

/**
   @brief Allocates and initializes a new, empty linked list.
   @returns A pointer to the new list.
 */
smb_ll *ll_create()
{
  smb_ll *new_list = smb_new(smb_ll, 1);
  ll_init(new_list);
  return new_list;
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
void ll_delete(smb_ll *list)
{
  ll_destroy(list);
  // Free the list header
  free(list);
}

/**
   @brief Append the given data to the end of the list.

   @param list A pointer to the list to append to.
   @param new_data The data to append.
   @param[out] status Status variable.
 */
void ll_append(smb_ll *list, DATA new_data)
{
  // Create the new node
  smb_ll_node *new_node = ll_create_node(new_data);

  // Get the last node in the list
  smb_ll_node *last_node = list->tail;

  // Link them together
  if (last_node)
    last_node->next = new_node;
  else
    list->head = new_node;
  new_node->prev = last_node;
  list->tail = new_node;
  list->length++;
}

/**
   @brief Prepend the given data to the beginning of the list.  All other data
   is shifted forward one index.

   @param list A pointer to the list.
   @param new_data The data to prepend.
   @param[out] status Status variable.
 */
void ll_prepend(smb_ll *list, DATA new_data)
{
  // Create the new smb_ll_node
  smb_ll_node *new_node = ll_create_node(new_data);
  smb_ll_node *first_node = list->head;
  new_node->next = first_node;
  if (first_node)
    first_node->prev = new_node;
  else
    list->tail = new_node;
  list->head = new_node;
  list->length++;
}

/**
   @brief Push the data to the back of the list.  An alias for ll_append. @see
   ll_append

   Push, of course, refers to its usage in the context of stacks.  In other
   words, the function appends an item at the end fo the list.

   @param list A pointer to the list to push to.
   @param new_data The data to push.
 */
void ll_push_back(smb_ll *list, DATA new_data)
{
  ll_append(list, new_data);
}

/**
   @brief Pop data from the back of the list.

   Pop, of course, refers to its usage in the context of stacks.  In other
   words, the function removes the item at the back/end of the list and returns
   it.

   @param list A pointer to the list to pop from.
   @param[out] status Status variable.
   @returns The item from the back of the list.
   @exception INDEX_ERROR If the list is empty.
 */
DATA ll_pop_back(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *last_node = list->tail;
  if (last_node) {
    DATA to_return = last_node->data;
    ll_remove_node(list, last_node);
    list->length--;
    return to_return;
  } else {
    *status = SMB_INDEX_ERROR;
    DATA mock_data;
    return mock_data;
  }
}

/**
   @brief Peek at the back of the list.

   Peek, of course, refers to its usage in the context of stacks.  So this
   returns the item at the end/back of the list without removing it.

   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @returns The item from the back of the list.
   @exception INDEX_ERROR If the list is empty.
 */
DATA ll_peek_back(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *last_node = list->tail;
  if (last_node) {
    return last_node->data;
  } else {
    *status = SMB_SUCCESS;
    DATA mock_data;
    return mock_data;
  }
}

/**
   @brief Push the data to the front of the list.  An alias for ll_prepend.
   @see ll_push_back If the term 'push' is unfamiliar.

   @param list A pointer to the list.
   @param new_data The data to push.
   @param[out] status Status variable.
 */

void ll_push_front(smb_ll *list, DATA new_data)
{
  ll_prepend(list, new_data);
}

/**
   @brief Pop the data from the front of the list.
   @see ll_pop_back If the term 'pop' is unfamiliar.

   @param list A pointer to the list to pop from
   @param[out] status Status variable.
   @returns The data from the front of the list.
   @exception INDEX_ERROR If the list is empty.
 */
DATA ll_pop_front(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *first_node = list->head;
  if (first_node) {
    DATA to_return = first_node->data;
    ll_remove_node(list, first_node);
    list->length--;
    return to_return;
  } else {
    *status = SMB_INDEX_ERROR; // The list is empty
    DATA mock_data;
    return mock_data;
  }
}

/**
   @brief Peek at the front of the list.
   @see ll_peek_back If the term 'peek' is unfamiliar.

   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @return The data from the front of the list.
   @exception INDEX_ERROR If the list is empty.
 */
DATA ll_peek_front(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *first_node = list->head;
  if (first_node) {
    return first_node->data;
  } else {
    *status = SMB_INDEX_ERROR; // The list is empty
    DATA mock_data;
    return mock_data;
  }
}

/**
   @brief Gets the data from the given index.

   However, there is no guarantee that the index was valid.  An empty DATA
   object is returned in that case, and an INDEX_ERROR is raised.

   @param list A pointer to the list to get from.
   @param index The index to get from the list.
   @param[out] status Status variable.
   @return The data at the specified index, if it exists.
   @exception INDEX_ERROR If the given index is out of range.
 */
DATA ll_get(const smb_ll *list, int index, smb_status *status)
{
  *status = SMB_SUCCESS;
  // Navigate to that position in the node.
  smb_ll_node * the_node = ll_navigate(list, index, status);
  if (*status == SMB_INDEX_ERROR) {
    // Return a dummy value.
    DATA mock_data;
    return mock_data;
  } else {
    return the_node->data;
  }
}

/**
   @brief Removes the node at the given index, if the index exists.

   If the item is not at the end of the list, the index of every item after this
   one will be shifted down one.

   @param list A pointer to the list to remove from.
   @param index The index to remove from the list.
   @param[out] status Status variable.
   @exception INDEX_ERROR If the given index is out of range.
 */
void ll_remove(smb_ll *list, int index, smb_status *status)
{
  // Fond the node
  smb_ll_node *the_node = ll_navigate(list, index, status);
  if (*status == SMB_INDEX_ERROR) {
    return; // Return the INDEX_ERROR
  }
  // Remove it (managing the links and the list header)
  ll_remove_node(list, the_node);
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
   @param new_data The data to insert.
 */
void ll_insert(smb_ll *list, int index, DATA new_data)
{
  if (index <= 0) {
    ll_prepend(list, new_data);
  } else if (index >= list->length) {
    ll_append(list, new_data);
  } else {
    smb_ll_node *new_node = ll_create_node(new_data);

    smb_status status = SMB_SUCCESS;
    smb_ll_node *current = ll_navigate(list, index, &status);

    // Since we already checked indices, there can be no errors.
    assert(status == SMB_SUCCESS);

    current->prev->next = new_node;
    new_node->prev = current->prev;
    new_node->next = current;
    current->prev = new_node;
    list->length++;
  }
}

/**
   @brief Sets an existing element to a new value.

   It is illegal to call ll_set on an out of bounds index.  You must use
   ll_insert for that.

   @param list The list to modify.
   @param index The index of the item to set.
   @param new_data The data to set the index to.
   @param[out] status Status variable.
   @exception INDEX_ERROR If the index is out of bounds
 */
void ll_set(smb_ll *list, int index, DATA new_data, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *current = ll_navigate(list, index, status);
  if (current) {
    current->data = new_data;
  } else {
    *status = SMB_INDEX_ERROR;
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
   @param[out] status Status variable.
   @returns The data at the new location of the iterator.
   @exception SMB_STOP_ITERATION If the list does not have another item.
 */
DATA ll_iter_next(smb_iter *iter, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *node = iter->state.data_ptr;
  if (node == NULL) {
    *status = SMB_STOP_ITERATION;
    return iter->state; // a DATA containing NULL
  }
  iter->state.data_ptr = node->next;
  iter->index++;
  return node->data;
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
  smb_free(iter);
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

void ll_append_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_append(list, new_data);
}

void ll_prepend_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_prepend(list, new_data);
}

DATA ll_get_adapter(const smb_list *l, int index, smb_status *status)
{
  const smb_ll *list = (const smb_ll*) (l->data);
  return ll_get(list, index, status);
}

void ll_remove_adapter(smb_list *l, int index, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_remove(list, index, status);
}

void ll_insert_adapter(smb_list *l, int index, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_insert(list, index, new_data);
}

void ll_delete_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_delete(list);
  l->data = NULL;
  return;
}

void ll_set_adapter(smb_list *l, int index, DATA new_data, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_set(list, index, new_data, status);
}

void ll_push_back_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_push_back(list, new_data);
}

DATA ll_pop_back_adapter(smb_list *l, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_pop_back(list, status);
}

DATA ll_peek_back_adapter(smb_list *l, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_peek_back(list, status);
}

void ll_push_front_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  return  ll_push_front(list, new_data);
}

DATA ll_pop_front_adapter(smb_list *l, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_pop_front(list, status);
}

DATA ll_peek_front_adapter(smb_list *l, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  return ll_peek_front(list, status);
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

   @param generic_list The smb_list to populate with function pointers.
 */
void ll_fill_functions(smb_list *generic_list)
{
  generic_list->append = ll_append_adapter;
  generic_list->prepend = ll_prepend_adapter;
  generic_list->get = ll_get_adapter;
  generic_list->remove = ll_remove_adapter;
  generic_list->insert = ll_insert_adapter;
  generic_list->delete = ll_delete_adapter;
  generic_list->push_back = ll_push_back_adapter;
  generic_list->pop_back = ll_pop_back_adapter;
  generic_list->peek_back = ll_peek_back_adapter;
  generic_list->push_front = ll_push_front_adapter;
  generic_list->pop_front = ll_pop_front_adapter;
  generic_list->peek_front = ll_peek_front_adapter;
  generic_list->length = ll_length_adapter;
  generic_list->set = ll_set_adapter;
}

/**
   @brief Creates a new, empty list, and returns it as an instance of the
   generic smb_list data structure. @see smb_list

   @param[out] status Status variable.
   @returns A generic list interface object.
 */
smb_list ll_create_list(smb_status *status)
{
  smb_ll *list = ll_create(status);
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
  smb_list generic_list;
  generic_list.data = list;

  ll_fill_functions(&generic_list);

  return generic_list;
}
