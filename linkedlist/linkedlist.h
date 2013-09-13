/*******************************************************************************

  File:         linkedlist.h

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  A linked list data structure

*******************************************************************************/

#include "../generic.h"

typedef struct ll_node 
{
  struct ll_node *prev;
  struct ll_node *next;
  DATA data;

} NODE;

struct ll_obj
{
  struct ll_node *head;
  struct ll_node *tail;
  int length;
};

typedef struct ll_obj LINKED_LIST;

/**
   Creates a new list with the given data.

   DATA newData - the data to store.

   return - a pointer to the list.
 */
LINKED_LIST *ll_create(DATA newData);

/**
   Append the given data to the end of the list.
 */
void ll_append(LINKED_LIST *list, DATA newData);

/**
   Gets the data from the given index.  However, there is no guarantee that the
   index was valid.  An empty DATA object is returned in that case.
 */
DATA ll_get(LINKED_LIST *list, int index);

/**
   Removes the node at the given index.
 */
int ll_remove(LINKED_LIST *list, int index);

/**
   Removes the linked list.
 */
void ll_delete(LINKED_LIST *list);
