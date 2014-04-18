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

int ht_test_deletions = 0;

void ht_test_deleter(DATA dValue)
{
  ht_test_deletions++;
}

unsigned int ht_test_constant_hash(DATA dKey)
{
  // return random_die_roll();
  return 4;
}

unsigned int ht_test_linear_hash(DATA dKey)
{
  return (unsigned int) dKey.data_llint;
}

/**
   This doesn't really just test insert.  It also tests create and get.  But I
   can't really isolate *just* insert.
 */
int ht_test_insert()
{
  DATA key, value;
  int i;

  struct smb_ht *table = ht_create(&ht_string_hash);

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
  return 0;
}

int ht_test_remove()
{
  DATA key, value;
  int a = 1;
  int i;
  struct smb_ht *table = ht_create(&ht_string_hash);
  ht_test_deletions = 0;

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
    ht_remove_act(table, key, ht_test_deleter);
    TEST_ASSERT(table->length == TEST_PAIRS - i - 1, a);
    a++;
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASSERT(ht_test_deletions == TEST_PAIRS, a);
  a++;
  return 0;
}

/*
  This test expecs a NOT_FOUND_ERROR
 */
int ht_test_remove_invalid()
{
  DATA key, value;
  int i;
  int a = 1;

  struct smb_ht *table = ht_create(&ht_string_hash);

  key.data_ptr = "invalid key";
  value = ht_get(table, key);

  ht_delete(table);
  return 0;
}

/**
   This test intentionally uses a very bad hash function that returns a
   constant.  This way, I can test whether the insertion and removal for large
   buckets is working.
 */
int ht_test_buckets()
{
  DATA key, value;
  int i;
  int a = 1;
  struct smb_ht *table = ht_create(&ht_test_constant_hash);
  ht_test_deletions = 0;
  
  for (i = 0; i < 20; i++) {
    key.data_llint = i;
    value.data_llint = -i;
    ht_insert(table, key, value);
    TEST_ASSERT(table->length == i+1, a);
    a++;
  }

  //ht_print(table, 0);

  // Remove one from the middle of the bucket list.
  key.data_llint = 10;
  ht_remove_act(table, key, ht_test_deleter);
  TEST_ASSERT(table->length == 19, a);
  a++;

  //ht_print(table, 0);

  // Remove one from the beginning of the bucket list.
  key.data_llint = 0;
  ht_remove_act(table, key, ht_test_deleter);
  TEST_ASSERT(table->length == 18, a);
  a++;

  //ht_print(table, 0);

  // Remove from the end of the list.
  key.data_llint = 19;
  ht_remove_act(table, key, ht_test_deleter);
  TEST_ASSERT(table->length == 17, a);
  a++;

  //ht_print(table, 0);

  // Verify that the other items are still there.
  for (i = 1; i < 10; i++) {
    key.data_llint = i;
    value = ht_get(table, key);
    TEST_ASSERT(value.data_llint == -i, a);
    a++;
  }

  for (i = 11; i < 19; i++) {
    key.data_llint = i;
    value = ht_get(table, key);
    TEST_ASSERT(value.data_llint == -i, a);
    a++;
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASSERT(ht_test_deletions == 20, a);
  a++;
  return 0;
}

/**
   This test adds to the hash table until it is forced to reallocate.  Then it
   checks that every value is still accessible.  
 */
int ht_test_resize()
{
  DATA key, value;
  int i;
  int a = 1;
  // Truncating addition will trim this to the number just before expanding.
  int last_stable = 1 + (int) (HASH_TABLE_INITIAL_SIZE * HASH_TABLE_MAX_LOAD_FACTOR);
  struct smb_ht *table = ht_create(ht_test_linear_hash);
  ht_test_deletions = 0;

  for (i = 0; i < last_stable; i++) {
    key.data_llint = i;
    value.data_llint = -i;
    ht_insert(table, key, value);
    TEST_ASSERT(table->allocated == HASH_TABLE_INITIAL_SIZE, a);
    a++;
    TEST_ASSERT(table->length == i + 1, a);
    a++;
  }

  //ht_print(table, 1);

  key.data_llint = last_stable;
  value.data_llint = -last_stable;
  ht_insert(table, key, value);
  TEST_ASSERT(table->allocated > HASH_TABLE_INITIAL_SIZE, a);
  a++;
  TEST_ASSERT(table->length == last_stable + 1, a);
  a++;

  //ht_print(table, 1);

  for (i = 0; i <= last_stable; i++) {
    key.data_llint = i;
    value = ht_get(table, key);
    TEST_ASSERT(value.data_llint == -i, a);
    a++;
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASSERT(ht_test_deletions == last_stable + 1, a);
  a++;
  return 0;
}

int ht_test_duplicate()
{
  DATA key, value;
  int i;
  int a = 1;
  char *newKey = "not the first value";
  struct smb_ht *table = ht_create(ht_string_hash);
  ht_test_deletions = 0;

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value);
  }

  //ht_print(table, 0);

  for (i = 0; i < TEST_PAIRS; i += 2) {
    TEST_ASSERT(table->length == TEST_PAIRS, a);
    a++;

    key.data_ptr = test_keys[i];
    value.data_ptr = newKey;
    ht_insert(table, key, value);
    value = ht_get(table, key);

    TEST_ASSERT(value.data_ptr == newKey, a);
    a++;
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value = ht_get(table, key);
    if (i % 2 == 1) {
      TEST_ASSERT(value.data_ptr == test_values[i], a);
      a++;
    } else {
      TEST_ASSERT(value.data_ptr == newKey, a);
      a++;
    }
  }

  //ht_print(table, 0);

  ht_delete_act(table, ht_test_deleter);
  TEST_ASSERT(ht_test_deletions == TEST_PAIRS, a);
  a++;
  return 0;
}

void hash_table_test() 
{
  smb_ut_group *group = su_create_test_group("hash table");

  smb_ut_test *insert = su_create_test("insert", ht_test_insert, 0, 1);
  su_add_test(group, insert);

  smb_ut_test *remove = su_create_test("remove", ht_test_remove, 0, 1);
  su_add_test(group, remove);

  smb_ut_test *remove_invalid = su_create_test("remove_invalid", ht_test_remove_invalid, 
                                        NOT_FOUND_ERROR, 1);
  su_add_test(group, remove_invalid);

  smb_ut_test *buckets = su_create_test("buckets", ht_test_buckets, 0, 1);
  su_add_test(group, buckets);

  smb_ut_test *resize = su_create_test("resize", ht_test_resize, 0, 1);
  su_add_test(group, resize);

  smb_ut_test *duplicate = su_create_test("duplicate", ht_test_duplicate, 0, 1);
  su_add_test(group, duplicate);

  su_run_group(group);
  su_delete_group(group);
}
