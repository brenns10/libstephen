/***************************************************************************//**

  @file         arraylist.c

  @author       Stephen Brennan

  @date         Created Friday, 27 September 2013

  @brief        Implementation of "libstephen/al.h".

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdlib.h>      /* memcpy    */

#include "libstephen/al.h"

/**
   @brief The default size that an array list is allocated with, and is added to
   the capacity each time it expands.
*/
#define SMB_AL_BLOCK_SIZE 20

/*******************************************************************************

                               Private Functions

*******************************************************************************/

/**
   @brief Expands the smb_al by adding anothur chunk of the default size.

   Note that this is a *private* function, not defined in libstephen.h for a
   reason.

   @param list The list to expand.
   @param[out] status Status variable.
 */
void al_expand(smb_al *list)
{
  list->allocated = list->allocated + SMB_AL_BLOCK_SIZE;
  list->data = smb_renew(DATA, list->data, list->allocated);
}

/**
   @brief Shifts the elements in the array up one element starting from
   from_index.

   Additionally, increments the list->length.  Precondition is that from_index
   is within range.  Since this is a private function, that is reasonable.

   Note that this is a *private* function, not defined in libstephen.h for a
   reason.

   @param list The list to operate on.
   @param from_index The index to start shifting up from.
   @param[out] status Status variable.
 */
void al_shift_up(smb_al *list, int from_index)
{
  // Check if there's space and allocate more if necessary
  if (list->length >= list->allocated) {
    al_expand(list);
  }

  // Starting at the last element in the array, shift up by one
  for (int i = list->length - 1; i >= from_index; i--) {
    list->data[i + 1] = list->data[i];
  }

  list->length++;
}

/**
   @brief Shifts the elements of the array down one element.

   The function shifts each element down in the list, until the element at the
   given index is eventually overwritten.  Decrements the list->length.
   Precondition is that to_index is within range.

   Note that this is a *private* function, not defined in libstephen.h for a
   reason.

   @param list The list to operate on.
   @param to_index The index to shift down to.  The element at this index is
   eventually overwritten.
 */
void al_shift_down(smb_al *list, int to_index)
{
  for (int i = to_index + 1; i < list->length; i++) {
    list->data[i - 1] = list->data[i];
  }

  list->length--;
}

/*******************************************************************************

                                Public Functions

*******************************************************************************/

void al_init(smb_al *list)
{
  list->data = smb_new(DATA, SMB_AL_BLOCK_SIZE);
  list->length = 0;
  list->allocated = SMB_AL_BLOCK_SIZE;
}

smb_al *al_create()
{
  smb_al *list = smb_new(smb_al, 1);
  al_init(list);
  return list;
}

void al_destroy(smb_al *list)
{
  smb_free(list->data);
}

void al_delete(smb_al *list)
{
  al_destroy(list);
  smb_free(list);
}

void al_append(smb_al *list, DATA newData)
{
  if (list->length < list->allocated) {
    list->data[list->length++] = newData;
  } else {
    al_expand(list);
    list->data[list->length++] = newData;
  }
}

void al_prepend(smb_al *list, DATA newData)
{
  al_shift_up(list, 0);
  list->data[0] = newData;
}

DATA al_get(const smb_al *list, int index, smb_status *status)
{
  *status = SMB_SUCCESS;
  if (index < 0 || index >= list->length) {
    *status = SMB_INDEX_ERROR;
    DATA mockData = PTR(NULL);
    return mockData;
  }

  return list->data[index];
}

void al_remove(smb_al *list, int index, smb_status *status)
{
  *status = SMB_SUCCESS;
  if (index < 0 || index >= list->length) {
    *status = SMB_INDEX_ERROR;
    return;
  }

  al_shift_down(list, index);
}

void al_insert(smb_al *list, int index, DATA newData)
{
  if (index < 0) {
    index = 0;
  } else if (index > list->length) {
    index = list->length;
  }

  al_shift_up(list, index);
  list->data[index] = newData;
}

void al_set(smb_al *list, int index, DATA newData, smb_status *status)
{
  *status = SMB_SUCCESS;
  if (index < 0 || index >= list->length) {
    *status = SMB_INDEX_ERROR;
    return;
  }

  list->data[index] = newData;
}

void al_push_back(smb_al *list, DATA newData)
{
  al_append(list, newData);
}

DATA al_pop_back(smb_al *list, smb_status *status)
{
  // On failure, returns dummy and sets INDEX_ERROR:
  DATA toReturn = al_get(list, list->length - 1, status);
  // On failure, sets INDEX_ERROR and does nothing:
  al_remove(list, list->length - 1, status);
  return toReturn;
}

DATA al_peek_back(smb_al *list, smb_status *status)
{
  return al_get(list, list->length - 1, status);
}

void al_push_front(smb_al *list, DATA newData)
{
  al_prepend(list, newData);
}

DATA al_pop_front(smb_al *list, smb_status *status)
{
  // On failure, returns dummy and sets INDEX_ERROR.
  DATA toReturn = al_get(list, 0, status);
  // On failure, sets INDEX_ERROR and does nothing.
  al_remove(list, 0, status);
  return toReturn;
}

DATA al_peek_front(smb_al *list, smb_status *status)
{
  return al_get(list, 0, status);
}

int al_length(const smb_al *list)
{
  return list->length;
}

int al_index_of(const smb_al *list, DATA d, DATA_COMPARE comp)
{
  int i;
  for (i = 0; i < list->length; i++) {
    if (comp == NULL) {
      if (list->data[i].data_llint == d.data_llint) {
        return i;
      }
    } else {
      if (comp(list->data[i], d) == 0) {
        return i;
      }
    }
  }
  return -1;
}

/**
   @brief Return the next item in the array list.
   @param iter The iterator being used.
   @param[out] status Status variable.
   @return The next item in the array list.
   @exception SMB_STOP_ITERATION If the list has no more elements.
 */
DATA al_iter_next(smb_iter *iter, smb_status *status)
{
  DATA ret_val = al_get((const smb_al *)iter->ds, iter->index++, status);
  if (*status == SMB_INDEX_ERROR) {
    *status = SMB_STOP_ITERATION;
  }
  return ret_val;
}

/**
   @brief Return whether or not the iterator has another item.
   @param iter The iterator being used.
   @return Whether or not the iterator has another item.
 */
bool al_iter_has_next(smb_iter *iter)
{
  return iter->index < al_length((const smb_al *)iter->ds);
}

/**
   @brief Free whatever resources are held by the iterator.
   @param iter The iterator to clean up.
 */
void al_iter_destroy(smb_iter *iter)
{
  (void)iter; // unused
}

/**
   @brief Free the iterator and its resources.
   @param iter The iterator to free.
 */
void al_iter_delete(smb_iter *iter)
{
  iter->destroy(iter);
  smb_free(iter);
}

smb_iter al_get_iter(const smb_al *list)
{
  smb_iter iter = {
    // Data values
    .ds = list,
    .state = (DATA) { .data_ptr = NULL },
    .index = 0,

    // Functions
    .next = &al_iter_next,
    .has_next = &al_iter_has_next,
    .destroy = &al_iter_destroy,
    .delete = &al_iter_delete
  };
  return iter;
}

/*******************************************************************************

                             List Adapter Functions

  These guys are used as the function pointers in smb_list.  They really don't
  need any documentation.

*******************************************************************************/

void al_append_adapter(smb_list *l, DATA newData)
{
  smb_al *list = (smb_al*) (l->data);
  al_append(list, newData);
}

void al_prepend_adapter(smb_list *l, DATA newData)
{
  smb_al *list = (smb_al*) (l->data);
  al_prepend(list, newData);
}

DATA al_get_adapter(const smb_list *l, int index, smb_status *status)
{
  const smb_al *list = (const smb_al*) (l->data);
  return al_get(list, index, status);
}

void al_set_adapter(smb_list *l, int index, DATA newData, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  al_set(list, index, newData, status);
}

void al_remove_adapter(smb_list *l, int index, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  al_remove(list, index, status);
}

void al_insert_adapter(smb_list *l, int index, DATA newData)
{
  smb_al *list = (smb_al*) (l->data);
  al_insert(list, index, newData);
}

void al_delete_adapter(smb_list *l)
{
  smb_al *list = (smb_al*) (l->data);
  al_delete(list);
  l->data = NULL;
  return;
}

int al_length_adapter(const smb_list *l)
{
  const smb_al *list = (const smb_al*) (l->data);
  return al_length(list);
}

void al_push_back_adapter(smb_list *l, DATA newData)
{
  smb_al *list = (smb_al*) (l->data);
  al_push_back(list, newData);
}

DATA al_pop_back_adapter(smb_list *l, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  return al_pop_back(list, status);
}

DATA al_peek_back_adapter(smb_list *l, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  return al_peek_back(list, status);
}

void al_push_front_adapter(smb_list *l, DATA newData)
{
  smb_al *list = (smb_al*) (l->data);
  al_push_front(list, newData);
}

DATA al_pop_front_adapter(smb_list *l, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  return al_pop_front(list, status);
}

DATA al_peek_front_adapter(smb_list *l, smb_status *status)
{
  smb_al *list = (smb_al*) (l->data);
  return al_peek_front(list, status);
}

int al_index_of_adapter(const smb_list *l, DATA d, DATA_COMPARE comp)
{
  const smb_al *list = (smb_al*) (l->data);
  return al_index_of(list, d, comp);
}

/**
   @brief Populate generic smb_list with function pointers necessary to use
   smb_al with it.

   Note that this is a *private* function, not defined in libstephen.h for a
   reason.  You shouldn't need the function, as there is library functionality
   that provides it for you.

   @param l The list to fill up.
 */
void al_fill_functions(smb_list *l)
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
  l->index_of = al_index_of_adapter;
}

smb_list al_cast_to_list(smb_al *list)
{
  smb_list genericList;
  genericList.data = list;

  al_fill_functions(&genericList);

  return genericList;
}

smb_list al_create_list()
{
  smb_al *list = al_create();
  return al_cast_to_list(list);
}
