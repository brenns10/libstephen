/***************************************************************************//**

  @file         libstephen/ll.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        A linked list data structure.

  This linked list data structure provides most basic features necessary in a
  linked list, along with some more advanced ones like iterators, stack
  functionality, and a generic list data structure that can use a different
  implementation.

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_LL_H
#define LIBSTEPHEN_LL_H

#include <stdbool.h> /* bool */

#include "base.h"  /* DATA     */
#include "list.h"  /* smb_list */

/**
   @brief Node structure for linked list.

   This must be exposed in order for other data types to be public.  This should
   not be used by users of the library.
 */
typedef struct smb_ll_node
{
  /**
     @brief The previous node in the linked list.
   */
  struct smb_ll_node *prev;

  /**
     @brief The next node in the linked list.
   */
  struct smb_ll_node *next;

  /**
     @brief The data stored in this node.
   */
  DATA data;

} smb_ll_node;

/**
   @brief The actual linked list data type.  "Bare" functions return a pointer
   to this structure.
 */
typedef struct smb_ll
{
  /**
     @brief A pointer to the head of the list.
   */
  struct smb_ll_node *head;

  /**
     @brief A pointer to the tail of the list.  Useful for speeding up
     navigation to a particular spot.
   */
  struct smb_ll_node *tail;

  /**
     @brief The number of items stored in the list.
   */
  int length;

} smb_ll;

/**
   @brief Initializes a new list in memory which has already been allocated.
   @param new_list A pointer to the memory to initialize.
 */
void ll_init(smb_ll *new_list);
/**
   @brief Allocates and initializes a new, empty linked list.
   @returns A pointer to the new list.
 */
smb_ll *ll_create();
/**
   @brief Frees all the resources held by the linked list without freeing the
   actual pointer to the list.

   If you create a list on the stack and use ll_init to initialize it, calling
   ll_delete will attempt to free your stack memory (bad).  Use this to free all
   the resources of the list without freeing the pointer.
   @param list The list to destroy.
 */
void ll_destroy(smb_ll *list);
/**
   @brief Frees the resources held by the linked list, and the memory allocated
   to the smb_ll object.

   This is equivalent to calling ll_destroy on the pointer, and then calling
   smb_free() on the pointer.
   @param list A pointer to the list to delete.
 */
void ll_delete(smb_ll *list);

/**
   @brief Creates a new, empty list, and returns it as an instance of the
   generic smb_list data structure. @see smb_list
   @returns A generic list interface object.
 */
smb_list ll_create_list();
/**
   @brief Cast a smb_ll pointer to an instance of the smb_list interface.  @see
   smb_list
   @param list The linked list to cast to a generic list.
   @return A generic list interface object.
 */
smb_list ll_cast_to_list(smb_ll *list);

/**
   @brief Append the given data to the end of the list.
   @param list A pointer to the list to append to.
   @param new_data The data to append.
 */
void ll_append(smb_ll *list, DATA new_data);
/**
   @brief Prepend the given data to the beginning of the list.  All other data
   is shifted forward one index.
   @param list A pointer to the list.
   @param new_data The data to prepend.
 */
void ll_prepend(smb_ll *list, DATA new_data);
/**
   @brief Push the data to the back of the list.  An alias for ll_append().

   Push, of course, refers to its usage in the context of stacks.  In other
   words, the function appends an item at the end fo the list.
   @param list A pointer to the list to push to.
   @param new_data The data to push.
 */
void ll_push_back(smb_ll *list, DATA new_data);
/**
   @brief Pop data from the back of the list.

   Pop, of course, refers to its usage in the context of stacks.  In other
   words, the function removes the item at the back/end of the list and returns
   it.
   @param list A pointer to the list to pop from.
   @param[out] status Status variable.
   @returns The item from the back of the list.
   @exception SMB_INDEX_ERROR If the list is empty.
 */
DATA ll_pop_back(smb_ll *list, smb_status *status);
/**
   @brief Peek at the back of the list.

   Peek, of course, refers to its usage in the context of stacks.  So this
   returns the item at the end/back of the list without removing it.
   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @returns The item from the back of the list.
   @exception SMB_INDEX_ERROR If the list is empty.
 */
DATA ll_peek_back(smb_ll *list, smb_status *status);
/**
   @brief Push the data to the front of the list.  An alias for ll_prepend.
   @see ll_push_back If the term 'push' is unfamiliar.
   @param list A pointer to the list.
   @param new_data The data to push.
 */
void ll_push_front(smb_ll *list, DATA new_data);
/**
   @brief Pop the data from the front of the list.
   @see ll_pop_back If the term 'pop' is unfamiliar.
   @param list A pointer to the list to pop from
   @param[out] status Status variable.
   @returns The data from the front of the list.
   @exception SMB_INDEX_ERROR If the list is empty.
 */
DATA ll_pop_front(smb_ll *list, smb_status *status);
/**
   @brief Peek at the front of the list.
   @see ll_peek_back If the term 'peek' is unfamiliar.
   @param list A pointer to the list to peek from.
   @param[out] status Status variable.
   @return The data from the front of the list.
   @exception SMB_INDEX_ERROR If the list is empty.
 */
DATA ll_peek_front(smb_ll *list, smb_status *status);
/**
   @brief Gets the data from the given index.

   However, there is no guarantee that the index was valid.  An empty DATA
   object is returned in that case, and an SMB_INDEX_ERROR is raised.
   @param list A pointer to the list to get from.
   @param index The index to get from the list.
   @param[out] status Status variable.
   @return The data at the specified index, if it exists.
   @exception SMB_INDEX_ERROR If the given index is out of range.
 */
DATA ll_get(const smb_ll *list, int index, smb_status *status);
/**
   @brief Removes the node at the given index, if the index exists.

   If the item is not at the end of the list, the index of every item after this
   one will be shifted down one.
   @param list A pointer to the list to remove from.
   @param index The index to remove from the list.
   @param[out] status Status variable.
   @exception SMB_INDEX_ERROR If the given index is out of range.
 */
void ll_remove(smb_ll *list, int index, smb_status *status);
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
void ll_insert(smb_ll *list, int index, DATA new_data);
/**
   @brief Sets an existing element to a new value.

   It is illegal to call ll_set() on an out of bounds index.  You must use
   ll_insert() for that.
   @param list The list to modify.
   @param index The index of the item to set.
   @param new_data The data to set the index to.
   @param[out] status Status variable.
   @exception INDEX_ERROR If the index is out of bounds
 */
void ll_set(smb_ll *list, int index, DATA new_data, smb_status *status);
/**
   @brief Returns the length of the given list.
   @param list A pointer to the list
   @returns The length of the list.
 */
int  ll_length(const smb_ll *list);
/**
   @brief Returns whether or not the list contains the item.
   @param list Pointer to the list.
   @param d The item to search for.
   @param comp The comparator to use.  NULL for bit comparison.
 */
int  ll_index_of(const smb_ll *list, DATA d, DATA_COMPARE comp);
/**
   @brief Stable sort of linked list.
   @param list List to sort.
   @param cmp Comparator for sorting.
   @return Nothing, but the list is sorted in place.
 */
void ll_sort(smb_ll *list, DATA_COMPARE cmp);

/**
   @brief Get an iterator for the linked list.
   @param list A pointer to the list.
   @returns an iterator
 */
smb_iter ll_get_iter(const smb_ll *list);

/**
   @brief Remove every element for which test_function returns true.
   @param list List to remove from.
   @param test_function Returns true if an element should be removed.
 */
void ll_filter(smb_ll *list, bool (*test_function)(DATA));

/**
   @brief Apply a function to every item in the list.

   Calls the function (with one argument) with each element of the list.  Then,
   puts the return value back into the list at the same location.  This is all
   IN PLACE, and thus pretty bad if you need to keep the references for the
   future.
   @param list The list to map over.
   @param map_function Function to apply.
   @returns nothing - the list is modified!
 */
void ll_map(smb_ll *list, DATA (*map_function)(DATA));

/**
   @brief Fold a function starting on the left.

   Applies `reduction(start_value, list[0])`, then `reduction(result, list[1])`,
   etc.  Returns the final result.
   @param list List to reduce the function along.
   @param start_value Initial value to give to the reduction.
   @param reduction Function to reduce.
   @returns the final result
 */
DATA ll_foldl(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA));

/**
   @brief Fold a function starting on the right.

   Applies `reduction(start_value, list[n-1])`, then `reduction(result,
   list[n-2])`, etc.  Returns the final result.
   @param list List to reduce the function along.
   @param start_value Initial value to give to the reduction.
   @param reduction Function to reduce.
   @returns the final result
 */
DATA ll_foldr(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA));

#endif // LIBSTEPHEN_LL_H
