/***************************************************************************//**

  @file         libstephen/al.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        An array based implementation of the list interface.

  arraylist.c provides an array implementation of the list interface.  It has
  its own set of functions, prefixed with al_*, that do the same operations as
  the linked list ones.  It also supports the use of the generic list interface,
  so both array lists and linked lists can be used interchangeably, in C!

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_AL_H
#define LIBSTEPHEN_AL_H

#include "base.h"  /* DATA     */
#include "list.h"  /* smb_list */

/**
   @brief The actual array list data type.

   "Bare" functions return a pointer to this structure.  You should not use any
   of the members, as they are implementation specific and subject to change.
 */
typedef struct smb_al
{
  /**
     @brief The area of memory containing the data.
   */
  DATA *data;

  /**
     @brief The number of items in the list.
   */
  int length;

  /**
     @brief The space allocated for the list.
   */
  int allocated;

} smb_al;

/**
   @brief Initialize an empty array list in memory already allocated.

   This function is useful if you would like to declare your array list on the
   stack and initialize it, rather than allocating space on the heap.
 */
void al_init(smb_al *list);
/**
   @brief Allocate and initialize an empty array list.
   @returns A pointer to the new array list.
 */
smb_al *al_create();
/**
   @brief Free the resources used by the array list, but don't actually free the
   pointer given.

   This is useful if you have a stack-allocated array list.
   @param list A pointer to the list to 'destroy.'
 */
void al_destroy(smb_al *list);
/**
   @brief Free the resources and the pointer to the array list.
   @param list A pointer to the list to delete.
 */
void al_delete(smb_al *list);

/**
   @brief Create a generic list as an array list.
   @returns A generic list pointing to a new array list.
 */
smb_list al_create_list();
/**
   @brief Cast an array list to a generic list.
   @param list The list to cast.
   @returns A generic list pointing to the same array list.
 */
smb_list al_cast_to_list(smb_al *list);

/**
   @brief Append an item to the end of a list.
   @param list A pointer to the list to append to.
   @param newData The data to append
 */
void al_append(smb_al *list, DATA newData);
/**
   @brief Prepend an item to the beginning of the list.
   @param list A pointer to the list to prepend to.
   @param newData The data to prepend.
 */
void al_prepend(smb_al *list, DATA newData);
/**
   @brief Return the data at a specified index.
   @param list A pointer to the list to get from.
   @param index The index to get from the list.
   @param[out] status Status variable.
   @returns The data at the specified index.
   @exception SMB_INDEX_ERROR If the specified index was out of range.
 */
DATA al_get(const smb_al *list, int index, smb_status *status);
/**
   @brief Removes the node at the given index, if the index exists.
   @param list A pointer to the list to remove from.
   @param index The index to remove from the list.
   @param[out] status Status variable.
   @exception SMB_INDEX_ERROR If the specified index was out of range.
 */
void al_remove(smb_al *list, int index, smb_status *status);
/**
   @brief Inserts an item at the specified location in the list.

   If the location is not the end of the list, every item at the given index is
   shifted up one index.  If the provided location is less than 0, the location
   will be treated as 0.  If the provided location is greater than the length of
   the list, the item will be added to the end of the list.
   @param list A pointer to the list to insert into.
   @param index The index to insert at.
   @param newData The data to insert.
 */
void al_insert(smb_al *list, int index, DATA newData);
/**
   @brief Sets the item at the given index.

   This can only be used to set *existing* indices.  If the list is of size 10
   and you try to set element 10 in order to expand it, you will fail.  You need
   to use al_insert or al_append for that.
   @param list A pointer to the list to modify.
   @param index The index to set.
   @param newData The new data.
   @param[out] status Status variable.
   @exception SMB_INDEX_ERROR If the provided index was out of range.
 */
void al_set(smb_al *list, int index, DATA newData, smb_status *status);
/**
   @brief Push the data to the back of the list.  An alias for al_append.  See
   ll_push_back() if you don't know the what 'push' means.
   @param list A pointer to the list to push to.
   @param newData The data to push to the back.
 */
void al_push_back(smb_al *list, DATA newData);
/**
   @brief Pop the data from the back of the list.  See ll_pop_back() if you
   don't know what 'pop' means.
   @param list A pointer to the list to pop from.
   @param[out] status Status variable.
   @returns The data from the back of the list.
   @exception SMB_INDEX_ERROR if the list is empty.
 */
DATA al_pop_back(smb_al *list, smb_status *status);
/**
   @brief Peeks at the data from the back of the list. See ll_peek_back() if you
   don't know what 'peek' means.
   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @returns The data at the back of the list.
   @exception SMB_INDEX_ERROR if the list is empty.
 */
DATA al_peek_back(smb_al *list, smb_status *status);
/**
   @brief Push the data to the front of the list.  Alias for al_prepend.  See
   ll_push_back() if you don't know what 'push' means.
   @param list A pointer to the list to push to.
   @param data The data to push to the front.
 */
void al_push_front(smb_al *list, DATA data);
/**
   @brief Pop the data from the front of the list.  See ll_pop_back() if you
   don't know what 'pop' means.
   @param list A pointer to the list to pop from.
   @param[out] status Status variable.
   @returns The data from the front of the list.
   @exception SMB_INDEX_ERROR If the list is empty.
 */
DATA al_pop_front(smb_al *list, smb_status *status);
/**
   @brief Peek at the data from the front of the list.  See ll_peek_back() if
   you don't know what 'peek' means.
   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @returns The data from the front of the list.
   @exception SMB_INDEX_ERROR if the list is empty.
 */
DATA al_peek_front(smb_al *list, smb_status *status);
/**
   @brief Returns the length of the list.
   @param list A pointer to the list.
   @returns The length of the list.
 */
int al_length(const smb_al *list);
/**
   @brief Returns the index of the item in the array.
   @param list A pointer to the list
   @param d The item to search for
   @param comp The comparator to use.  NULL for bit comparison.
   @returns Index of the item, or -1 if it's not in the list.
 */
int al_index_of(const smb_al *list, DATA d, DATA_COMPARE comp);

/**
   @brief Return an iterator on the array list.
   @param list A pointer to the list.
   @return The smb_iter to the list.
 */
smb_iter al_get_iter(const smb_al *list);

#endif // LIBSTEPHEN_AL_H
