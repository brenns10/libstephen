/*******************************************************************************

  File:         hashtabletest.c

  Author:       Stephen Brennan

  Date Created: Thursday,  5 December 2013

  Description:  A test of the hash table.

*******************************************************************************/

#include <stdio.h>
#include "tests.h"
#include "../libstephen.h"

#define TEST_PAIRS 5

char *test_keys[] = {
  "first key",
  "second key",
  "third key",
  "fourth key",
  "fifth key"
};

char *test_values[] = {
  "first value",
  "second value",
  "third value",
  "fourth value",
  "fifth value"
};

int ht_test_insert()
{
  DATA key, value;
  int i;

  HASH_TABLE *table = ht_create(&ht_string_hash);

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value);
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    TEST_ASSERT(test_values[i] == ht_get(table, key).data_ptr, i+1);
  }

  ht_delete(table);
}

void hash_table_test() 
{
  TEST_GROUP *group = su_create_test_group("hash table");

  TEST *insert = su_create_test("insert", ht_test_insert, 0, 1);
  su_add_test(group, insert);

  su_run_group(group);
  su_delete_group(group);
}
