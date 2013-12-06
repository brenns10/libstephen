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

/**
   This doesn't really just test insert.  It also tests create and get.  But I
   can't really isolate *just* insert.
 */
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

  // ht_print(table, 0);

  ht_delete(table);
}

int ht_test_remove()
{
  DATA key, value;
  int a = 1;
  int i;

  HASH_TABLE *table = ht_create(&ht_string_hash);

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value);
  }

  TEST_ASSERT(table->length == TEST_PAIRS, a);
  a++;

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value = ht_get(table, key);
    TEST_ASSERT(test_values[i] == value.data_ptr, a);
    a++;
    ht_remove(table, key);
    TEST_ASSERT(table->length == TEST_PAIRS - i - 1, a);
    a++;
  }

  ht_delete(table);
}

/*
  This test expecs a NOT_FOUND_ERROR
 */
int ht_test_remove_invalid()
{
  DATA key, value;
  int i;
  int a = 1;

  HASH_TABLE *table = ht_create(&ht_string_hash);

  key.data_ptr = "invalid key";
  value = ht_get(table, key);

  ht_delete(table);
}

void hash_table_test() 
{
  TEST_GROUP *group = su_create_test_group("hash table");

  TEST *insert = su_create_test("insert", ht_test_insert, 0, 1);
  su_add_test(group, insert);

  TEST *remove = su_create_test("remove", ht_test_remove, 0, 1);
  su_add_test(group, remove);

  TEST *remove_invalid = su_create_test("remove_invalid", ht_test_remove_invalid, 
                                        NOT_FOUND_ERROR, 1);
  su_add_test(group, remove_invalid);
  

  su_run_group(group);
  su_delete_group(group);
}
