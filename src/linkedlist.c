/***************************************************************************//**

  @file         linkedlist.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Implementation of "libstephen/ll.h".

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

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
   called, the_node is invlidated.  Please note that this function *does not*
   decrement the list's length!

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
  smb_free(the_node);
}

smb_ll_node *ll_node_navigate(smb_ll_node *node, int index)
{
  int i = 0;
  while (i < index) {
    node = node->next;
    i++;
  }
  return node;
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
  return ll_node_navigate(node, index);
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

void ll_init(smb_ll *new_list)
{
  new_list->length = 0;
  new_list->head = NULL;
  new_list->tail = NULL;
}

smb_ll *ll_create()
{
  smb_ll *new_list = smb_new(smb_ll, 1);
  ll_init(new_list);
  return new_list;
}

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

void ll_delete(smb_ll *list)
{
  ll_destroy(list);
  smb_free(list);
}

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

void ll_push_back(smb_ll *list, DATA new_data)
{
  ll_append(list, new_data);
}

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
    return PTR(NULL);
  }
}

DATA ll_peek_back(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *last_node = list->tail;
  if (last_node) {
    return last_node->data;
  } else {
    *status = SMB_INDEX_ERROR;
    return PTR(NULL);
  }
}

void ll_push_front(smb_ll *list, DATA new_data)
{
  ll_prepend(list, new_data);
}

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
    return PTR(NULL);
  }
}

DATA ll_peek_front(smb_ll *list, smb_status *status)
{
  *status = SMB_SUCCESS;
  smb_ll_node *first_node = list->head;
  if (first_node) {
    return first_node->data;
  } else {
    *status = SMB_INDEX_ERROR; // The list is empty
    return PTR(NULL);
  }
}

DATA ll_get(const smb_ll *list, int index, smb_status *status)
{
  *status = SMB_SUCCESS;
  // Navigate to that position in the node.
  smb_ll_node * the_node = ll_navigate(list, index, status);
  if (*status == SMB_INDEX_ERROR) {
    // Return a dummy value.
    return PTR(NULL);
  } else {
    return the_node->data;
  }
}

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

int ll_length(const smb_ll *list)
{
  return list->length;
}

/**
   @brief Recursive mergesort helper function.
   @param head Pointer to the head pointer (will be updated to reflect new list
   after sorting).
   @param length Length of sublist.  Expects the trailing node will not point to
   the next sublist.
   @param cmp Comparator.
   @returns The tail of the sorted list!
 */
static smb_ll_node *ll_sort_rec(smb_ll_node **head, int length, DATA_COMPARE cmp)
{
  smb_ll_node *left = *head;
  smb_ll_node *right, *tail, *next;

  // BASE CASE!
  if (length <= 1) {
    return *head;
  }

  // Find midpoint and bisect list.
  right = ll_node_navigate(*head, length/2);
  right->prev->next = NULL;
  right->prev = NULL;

  // Sort each sublist.
  ll_sort_rec(&left, length/2, cmp);
  ll_sort_rec(&right, length-length/2, cmp);

  // Pick the first element.
  if (cmp(left->data, right->data) <= 0) {
    *head = left;
    tail = left;
    left = left->next;
  } else {
    *head = right;
    tail = right;
    right = right->next;
  }

  // Merge the remaining elements.
  while (--length) {
    if (left == NULL) {
      next = right;
      right = right->next;
    } else if (right == NULL) {
      next = left;
      left = left->next;
    } else if (cmp(left->data, right->data) <= 0) {
      next = left;
      left = left->next;
    } else {
      next = right;
      right = right->next;
    }
    tail->next = next;
    next->prev = tail;
    next->next = NULL;
    tail = next;
  }
  tail->next = NULL;
  return tail;
}

void ll_sort(smb_ll *list, DATA_COMPARE comp)
{
  list->tail = ll_sort_rec(&list->head, list->length, comp);
}

int ll_index_of(const smb_ll *list, DATA d, DATA_COMPARE comp)
{
  int idx = 0;
  smb_ll_node *curr = list->head;

  while (curr) {
    if (comp != NULL) {
      if (comp(curr->data, d) == 0) {
        return idx;
      }
    } else {
      if (curr->data.data_llint == d.data_llint) {
        return idx;
      }
    }
    idx++;
    curr = curr->next;
  }
  return -1;
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
  (void)iter; // unused
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

void ll_filter(smb_ll *list, bool (*test_function)(DATA))
{
  smb_ll_node *curr = list->head, *next;
  while (curr) {
    next = curr->next;
    if (test_function(curr->data)) {
      ll_remove_node(list, curr);
      list->length--;
    }
    curr = next;
  }
}

void ll_map(smb_ll *list, DATA (*map_function)(DATA))
{
  smb_ll_node *curr = list->head;
  while (curr) {
    curr->data = map_function(curr->data);
    curr = curr->next;
  }
}

DATA ll_foldl(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA))
{
  smb_ll_node *curr = list->head;
  while (curr) {
    start_value = reduction(start_value, curr->data);
    curr = curr->next;
  }
  return start_value;
}

DATA ll_foldr(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA))
{
  smb_ll_node *curr = list->tail;
  while (curr) {
    start_value = reduction(curr->data, start_value);
    curr = curr->prev;
  }
  return start_value;
}

/*******************************************************************************

                         Linked List Adapter Functions

  These guys are used as the function pointers for the smb_list.  They really
  don't need any documentation.

*******************************************************************************/

void ll_append_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_append(list, new_data);
}

void ll_prepend_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_prepend(list, new_data);
}

DATA ll_get_adapter(const smb_list *l, int index, smb_status *status)
{
  const smb_ll *list = (const smb_ll*) (l->data);
  return ll_get(list, index, status);
}

void ll_remove_adapter(smb_list *l, int index, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_remove(list, index, status);
}

void ll_insert_adapter(smb_list *l, int index, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_insert(list, index, new_data);
}

void ll_delete_adapter(smb_list *l)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_delete(list);
  l->data = NULL;
}

void ll_set_adapter(smb_list *l, int index, DATA new_data, smb_status *status)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_set(list, index, new_data, status);
}

void ll_push_back_adapter(smb_list *l, DATA new_data)
{
  smb_ll *list = (smb_ll*) (l->data);
  ll_push_back(list, new_data);
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
  ll_push_front(list, new_data);
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

int ll_index_of_adapter(const smb_list *l, DATA d, DATA_COMPARE comp)
{
  const smb_ll *list = (const smb_ll*) (l->data);
  return ll_index_of(list, d, comp);
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
  generic_list->index_of = ll_index_of_adapter;
}

smb_list ll_create_list()
{
  smb_ll *list = ll_create();
  return ll_cast_to_list(list);
}

smb_list ll_cast_to_list(smb_ll *list)
{
  smb_list generic_list;
  generic_list.data = list;

  ll_fill_functions(&generic_list);

  return generic_list;
}
