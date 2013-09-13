/*******************************************************************************

  File:         linkedlisttest.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  Test of the linked list

*******************************************************************************/

#include <stdio.h>
#include "tests.h"
#include "../linkedlist/linkedlist.h"
#include "../common_include.h"

void print_list_int(LINKED_LIST *list)
{
  for(int i = 0; i < list->length; i++) {
    printf("Item %d: %d\n", i, ll_get(list, i).data_llint);
  }
}

void linked_list_test()
{
  printf("################################################################################\n");
  printf("############################ BEGIN LINKED LIST TEST ############################\n");

  // Data initialization
  DATA theData;
  theData.data_llint = 0;
  LINKED_LIST *test = ll_create(theData);
  int size = 1;

  printf("##### Initialized\n");
  
  // Adding data to the list
  for ( ; size < 10; size++) {
    theData.data_llint = size;
    ll_append(test, theData);
  }
  printf("##### Added elements\n");
  print_list_int(test, size);

  printf("##### Removing an element\n");
  // Removing data from the list
  ll_remove(test, 5);
  size--;  
  print_list_int(test, size);

  // Memory test
  printf("##### Number of mallocs: %d\n", SMB_GET_MALLOC_COUNTER);
  ll_delete(test);
  printf("##### Number of mallocs: %d\n", SMB_GET_MALLOC_COUNTER);

  printf("############################# END LINKED LIST TEST #############################\n");
  printf("################################################################################\n");
}
