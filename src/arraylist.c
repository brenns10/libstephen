/*******************************************************************************

  File:         arraylist.c
  
  Author:       Stephen Brennan
  
  Date Created: Friday, 27 September 2013
  
  Description:  An array based implementation of the list interface.

*******************************************************************************/

#include <stdlib.h> // for memcpy
#include "libstephen.h"

// This is the default size that an array list is allocated with, and is added
// to the capacity each time it expands.
#define SMB_AL_BLOCK_SIZE 20

////////////////////////////////////////////////////////////////////////////////
// Private Helper Functions

/**
   al_expand:

   Expands the struct smb_al by adding another default chunk size.  Uses realloc().

   # Parameters #

   - struct smb_al *list: the list to expand.

   # Raises #

   - ALLOCATION_ERROR: if realloc fails.  Unexpanded block of data remains
     valid, and no changes are made to the list.
 */
void al_expand(struct smb_al *list)
{
  int newAllocation = list->allocated + SMB_AL_BLOCK_SIZE;

  DATA *newBlock = (DATA*) realloc(list->data, newAllocation * sizeof(DATA));
  if (!newBlock) {
    RAISE(ALLOCATION_ERROR);
    return;
  }

  SMB_INCREMENT_MALLOC_COUNTER(SMB_AL_BLOCK_SIZE * sizeof(DATA));
  list->data = newBlock;
  list->allocated = newAllocation;
}

/**
   al_shift_up:

   Shifts the elements in the array up one element starting from from_index.
   Increments the list->length.  Precondition is that from_index is within
   range.

   # Parameters #

   - struct smb_al *list: the list to operate on.

   - int from_index: the index to start shifting up from.

   # Raises #
   
   - ALLOCATION_ERROR: if an expansion was required and realloc() failed, no
     shift is performed.  All data remains valid, but no changes are made to the
     array.
 */
void al_shift_up(struct smb_al *list, int from_index)
{
  // Check if there's space and allocate more if necessary
  if (list->length >= list->allocated) {
    al_expand(list);
    if (CHECK(ALLOCATION_ERROR)) 
      return;
  }

  // Starting at the last element in the array, shift up by one
  for (int i = list->length - 1; i >= from_index; i--) {
    list->data[i + 1] = list->data[i];
  }

  list->length++;
}

/**
   al_shift_down:

   Shifts the elements of the array down one element, eventually overwriting the
   element at index to_index.  Decrements the list->length.  Precondition is
   that to_index is within range.

   # Parameters #

   - struct smb_al *list: the list to operate on.

   - int to_index: the index to shift down to.  The element at this index is
     eventually overwritten.
 */
void al_shift_down(struct smb_al *list, int to_index)
{
  for (int i = to_index + 1; i < list->length; i++) {
    list->data[i - 1] = list->data[i];
  }

  list->length--;
}

////////////////////////////////////////////////////////////////////////////////
// Public Functions

/*
  Initialize fields of a newly allocated array list.
 */
void al_init(struct smb_al *list)
{
  CLEAR_ALL_ERRORS;

  list->data = (DATA*) malloc(SMB_AL_BLOCK_SIZE * sizeof(DATA));
  if (!list->data) {
    free(list);
    SMB_DECREMENT_MALLOC_COUNTER(sizeof(struct smb_al));
    RAISE(ALLOCATION_ERROR);
    return;
  }
  list->length = 0;
  list->allocated = SMB_AL_BLOCK_SIZE;
  SMB_INCREMENT_MALLOC_COUNTER(SMB_AL_BLOCK_SIZE * sizeof(DATA));
}

/*
  Allocate and initialize an empty array list on the heap.
 */
struct smb_al *al_create()
{
  CLEAR_ALL_ERRORS;

  struct smb_al *list = (struct smb_al*) malloc(sizeof(struct smb_al));
  if (!list) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(struct smb_al));

  al_init(list);
  if (CHECK(ALLOCATION_ERROR)) {
    return NULL;
  }

  return list;
}

void al_append(struct smb_al *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  if (list->length < list->allocated) {
    list->data[list->length++] = newData;
  } else {
    al_expand(list);
    if (CHECK(ALLOCATION_ERROR)) {
      return; // Prevent segfault
    }
    list->data[list->length++] = newData;
  }
}

void al_prepend(struct smb_al *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  al_shift_up(list, 0);
  if (CHECK(ALLOCATION_ERROR))
    return;

  list->data[0] = newData;
}

DATA al_get(struct smb_al *list, int index)
{
  CLEAR_ALL_ERRORS;

  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    DATA mockData;
    return mockData;
  }

  return list->data[index];
}

void al_set(struct smb_al *list, int index, DATA newData)
{
  CLEAR_ALL_ERRORS;
  
  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    return;
  }

  list->data[index] = newData;
}

void al_remove(struct smb_al *list, int index)
{
  CLEAR_ALL_ERRORS;

  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    return;
  }

  al_shift_down(list, index);
}

void al_insert(struct smb_al *list, int index, DATA newData)
{
  CLEAR_ALL_ERRORS;
  
  if (index < 0) {
    index = 0;
  } else if (index > list->length) {
    index = list->length;
  }

  al_shift_up(list, index);

  if (CHECK(ALLOCATION_ERROR)) {
    return;
  }

  list->data[index] = newData;
}

void al_destroy(struct smb_al *list)
{
  CLEAR_ALL_ERRORS;

  free(list->data);
  SMB_DECREMENT_MALLOC_COUNTER(list->allocated * sizeof(DATA));
}

void al_delete(struct smb_al *list)
{
  CLEAR_ALL_ERRORS;

  al_destroy(list);
  free(list);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(struct smb_al));
}

int al_length(struct smb_al *list)
{
  CLEAR_ALL_ERRORS;

  return list->length;
}

void al_push_back(struct smb_al *list, DATA newData)
{
  return al_append(list, newData);
}

DATA al_pop_back(struct smb_al *list)
{
  DATA toReturn = al_get(list, list->length - 1);
  al_remove(list, list->length - 1);
  return toReturn;
}

DATA al_peek_back(struct smb_al *list)
{
  return al_get(list, list->length - 1);
}

void al_push_front(struct smb_al *list, DATA newData)
{
  return al_prepend(list, newData);
}

DATA al_pop_front(struct smb_al *list)
{
  DATA toReturn = al_get(list, 0);
  al_remove(list, 0);
  return toReturn;
}

DATA al_peek_front(struct smb_al *list)
{
  return al_get(list, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Interface Adapters

void al_append_adapter(struct smb_list *l, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_append(list, newData);
}

void al_prepend_adapter(struct smb_list *l, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_prepend(list, newData);
}

DATA al_get_adapter(struct smb_list *l, int index)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_get(list, index);
}

void al_set_adapter(struct smb_list *l, int index, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_set(list, index, newData);
}

void al_remove_adapter(struct smb_list *l, int index)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_remove(list, index);
}

void al_insert_adapter(struct smb_list *l, int index, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_insert(list, index, newData);
}

void al_delete_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  al_delete(list);
  l->data = NULL;
  return;
}

int al_length_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_length(list);
}

void al_push_back_adapter(struct smb_list *l, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_push_back(list, newData);
}

DATA al_pop_back_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_pop_back(list);
}

DATA al_peek_back_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_peek_back(list);
}

void al_push_front_adapter(struct smb_list *l, DATA newData)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_push_front(list, newData);
}

DATA al_pop_front_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_pop_front(list);
}

DATA al_peek_front_adapter(struct smb_list *l)
{
  struct smb_al *list = (struct smb_al*) (l->data);
  return al_peek_front(list);
}

void al_fill_functions(struct smb_list *l)
{
  l->append = al_append_adapter;
  l->prepend = al_prepend_adapter;
  l->get = al_get_adapter;
  l->set = al_set_adapter;
  l->remove = al_remove_adapter;
  l->insert = al_insert_adapter;
  l->delete = al_delete_adapter;
  l->length = al_length_adapter;
  l->push_back = al_push_back_adapter;
  l->pop_back = al_pop_back_adapter;
  l->peek_back = al_peek_back_adapter;
  l->push_front = al_push_front_adapter;
  l->pop_front = al_pop_front_adapter;
  l->peek_front = al_peek_front_adapter;
}

////////////////////////////////////////////////////////////////////////////////
// Interface Public Functions

struct smb_list al_cast_to_list(struct smb_al *list)
{
  struct smb_list genericList;
  genericList.data = list;

  al_fill_functions(&genericList);

  return genericList;
}

struct smb_list al_create_list()
{
  struct smb_al *list = al_create();

  return al_cast_to_list(list);
}
