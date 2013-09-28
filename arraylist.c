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

void al_expand(ARRAY_LIST *list)
{
  int newAllocation = list->allocated + SMB_AL_BLOCK_SIZE;

  DATA *newBlock = (DATA*) realloc(list->data, newAllocation * sizeof(DATA));
  if (!newBlock) {
    RAISE(ALLOCATION_ERROR);
    return;
  }

  SMB_INCREMENT_MALLOC_COUNTER(SMB_AL_BLOCK_SIZE);
  list->data = newBlock;
  list->allocated = newAllocation;
}

void al_shift_up(ARRAY_LIST *list, int from_index)
{
  // PRECONDITION: from_index is within bounds.
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
}

////////////////////////////////////////////////////////////////////////////////
// Public Functions

ARRAY_LIST *al_create(DATA newData)
{
  CLEAR_ALL_ERRORS;

  ARRAY_LIST *list = al_create_empty();

  if (list) {
    al_append(list, newData);
    // No need to check for ALLOCATION_ERROR, as no memory is allocated when
    // appending to size 0 array list.p
  }

  return list; // NULL if failed
}

ARRAY_LIST *al_create_empty()
{
  CLEAR_ALL_ERRORS;

  ARRAY_LIST *list = (ARRAY_LIST*) malloc(sizeof(ARRAY_LIST));
  if (!list) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(1);
  
  list->data = (DATA*) malloc(SMB_AL_BLOCK_SIZE * sizeof(DATA));
  if (!list->data) {
    free(list);
    SMB_DECREMENT_MALLOC_COUNTER(1);
    RAISE(ALLOCATION_ERROR);
    return NULL
  }
  list->length = 0;
  list->allocated = SMB_AL_BLOCK_SIZE;
  SMB_INCREMENT_MALLOC_COUNTER(SMB_AL_BLOCK_SIZE);

  return list;
}

void al_append(ARRAY_LIST *list, DATA newData)
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

void al_prepend(ARRAY_LIST *list, DATA newData)
{
  CLEAR_ALL_ERRORS;

  al_shift_up(list, 0);
  if (CHECK(ALLOCATION_ERROR))
    return;

  list->data[0] = newData;
  list->length++;
}

DATA al_get(ARRAY_LIST *list, int index)
{
  CLEAR_ALL_ERRORS;

  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    DATA mockData;
    return mockData;
  }

  return list->data[index];
}

void al_set(ARRAY_LIST *list, int index, DATA newData)
{
  CLEAR_ALL_ERRORS;
  
  if (index < 0 || index >= list->length) {
    RAISE(INDEX_ERROR);
    return;
  }

  list->data[index] = newData;
}



////////////////////////////////////////////////////////////////////////////////
// Interface Adapters

void al_append_adapter(LIST *l, DATA newData)
{
  ARRAY_LIST *list = (ARRAY_LIST*) (l->data);
  return al_append(list, newData);
}

void al_prepend_adapter(LIST *l, DATA newData)
{
  ARRAY_LIST *list = (ARRAY_LIST*) (l->data);
  return al_prepend(list, newData);
}

DATA al_get_adapter(LIST *l, int index)
{
  ARRAY_LIST *list = (ARRAY_LIST*) (l->data);
  return al_get(list, index);
}

void al_set_adapter(LIST *l, int index, DATA newData)
{
  ARRAY_LIST *list = (ARRAY_LIST*) (l->data);
  return al_set(list, index, newData);
}

void al_fill_functions(LIST *l)
{
  l->append = al_append_adapter;
  l->prepend = al_prepend_adapter;
  l->get = al_get_adapter;
  l->set = al_set_adapter;
}

////////////////////////////////////////////////////////////////////////////////
// Interface Public Functions
