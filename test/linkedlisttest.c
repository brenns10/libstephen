/*******************************************************************************

  File:         linkedlisttest.c

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  Test of the linked list

*******************************************************************************/

#include <stdio.h>
#include "tests.h"
#include "../libstephen.h"

void print_list_int(LINKED_LIST *list)
{
  LL_ITERATOR iter = ll_get_iter(list);
  DATA d;
  while (ll_iter_valid(&iter)) {
    d = ll_iter_curr(&iter);
    printf("Item %d: %d\n", iter.index, d.data_llint);
    ll_iter_next(&iter);
  }
}

void test_push_pop()
{
  printf("################################################################################\n");
  printf("##### BEGIN test_push_pop()\n");
  printf("################################################################################\n\n");

  DATA theData;
  LINKED_LIST *list = ll_create_empty();

  printf("##### Initialized the list\n");

  int number = 10;
  while (number < 20) {
    theData.data_llint = number++;
    ll_push_front(list, theData);
  }

  printf("##### Pushed to the front\n");

  print_list_int(list);

  while (number < 30) {
    theData.data_llint = number++;
    ll_push_back(list, theData);
  }

  printf("##### Pushed to the back\n");
  
  print_list_int(list);
  
  while (number < 35) {
    printf("Pop front: %d\n", ll_pop_front(list).data_llint);
    number++;
  }

  while (number < 40) {
    printf("Pop back: %d\n", ll_pop_back(list).data_llint);
    number++;
  }

  print_list_int(list);

  printf("Malloc count: %d\n", SMB_GET_MALLOC_COUNTER);

  ll_delete(list);
  printf("Malloc count: %d\n", SMB_GET_MALLOC_COUNTER);

  printf("\n################################################################################\n");
  printf("##### END test_push_pop()\n");
  printf("################################################################################\n");
}

void test_insert_remove()
{
  printf("################################################################################\n");
  printf("##### BEGIN test_insert_remove()\n");
  printf("################################################################################\n\n");

  DATA theData;
  LINKED_LIST *list = ll_create_empty();
  
  printf("##### Initialized the list\n");

  for (int i = 0; i < 20; i++) {
    theData.data_llint = i;
    ll_insert(list, i, theData);
    // print_list_int(list);
  }

  printf("##### Inserted 20 items\n");
  print_list_int(list);

  theData.data_llint = 100;
  ll_insert(list, 10, theData);

  printf("##### Inserted in middle\n");
  print_list_int(list);

  theData.data_llint++;
  ll_insert(list, 0, theData);

  printf("##### Inserted at beginning\n");
  print_list_int(list);

  theData.data_llint++;
  ll_insert(list, list->length, theData);

  printf("##### Inserted at end\n");
  print_list_int(list);
  printf("Malloc count: %d\n", SMB_GET_MALLOC_COUNTER);
  ll_delete(list);
  printf("Malloc count: %d\n", SMB_GET_MALLOC_COUNTER);
  printf("\n################################################################################\n");
  printf("##### END test_insert_remove()\n");
  printf("################################################################################\n");
}

void linked_list_test()
{
  test_push_pop();
  test_insert_remove();
}
