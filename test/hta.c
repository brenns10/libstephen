/***************************************************************************//**

  @file         hta.c

  @author       Stephen Brennan

  @date         Created Tuesday,  5 December 2013

  @brief        A test of the HTA.

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>

#include "tests.h"
#include "libstephen/ut.h"
#include "libstephen/ht.h"
#include "libstephen/hta.h"

#define TEST_PAIRS 5

char *hta_test_keys[] = {
  "first key",
  "second key",
  "third key",
  "fourth key",
  "fifth key"
};

char *hta_test_values[] = {
  "first value",
  "second value",
  "third value",
  "fourth value",
  "fifth value"
};

unsigned int hta_test_constant_hash(void *key)
{
  (void) key; // unused
  return 4;
}

unsigned int hta_test_linear_hash(void *key)
{
  return * (unsigned int*) key;
}

/**
   This doesn't really just test insert.  It also tests create and get.  But I
   can't really isolate *just* insert.
 */
int hta_test_insert()
{
  smb_status status = SMB_SUCCESS;
  int i;
  char **rv;
  smb_hta *table = hta_create(&hta_string_hash, &hta_string_comp,
                              sizeof(char*), sizeof(char*));

  for (i = 0; i < TEST_PAIRS; i++) {
    hta_insert(table, &hta_test_keys[i], &hta_test_values[i]);
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    TEST_ASSERT(hta_contains(table, &hta_test_keys[i]));
    rv = (char**)hta_get(table, &hta_test_keys[i], &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_PTR_EQ(hta_test_values[i], *rv);
  }

  hta_delete(table);
  return 0;
}

int hta_test_remove()
{
  smb_status status= SMB_SUCCESS;
  char **rv;
  unsigned int i;
  smb_hta *table = hta_create(&hta_string_hash, &hta_string_comp,
                              sizeof(char*), sizeof(char*));

  for (i = 0; i < TEST_PAIRS; i++) {
    hta_insert(table, &hta_test_keys[i], &hta_test_values[i]);
  }

  TA_INT_EQ(table->length, TEST_PAIRS);

  for (i = 0; i < TEST_PAIRS; i++) {
    rv = (char**)hta_get(table, &hta_test_keys[i], &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_PTR_EQ(hta_test_values[i], *rv);
    hta_remove(table, &hta_test_keys[i], &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_INT_EQ(table->length, TEST_PAIRS - i - 1);
    TEST_ASSERT(!hta_contains(table, &hta_test_keys[i]));
  }

  hta_delete(table);
  return 0;
}

/*
  This test expects a NOT_FOUND_ERROR
 */
int hta_test_remove_invalid()
{
  smb_status status = SMB_SUCCESS;
  smb_hta *table = hta_create(&hta_string_hash, &hta_string_comp, sizeof(char*), sizeof(char*));
  char *key = "invalid_key";

  hta_get(table, &key, &status);
  TA_INT_EQ(status, SMB_NOT_FOUND_ERROR);

  hta_delete(table);
  return 0;
}

/**
   This test intentionally uses a very bad hash function that returns a
   constant.  This way, I can test whether the insertion and removal for large
   buckets is working.
 */
int hta_test_buckets()
{
  smb_status status = SMB_SUCCESS;
  int key, value, *rv;
  unsigned int i;
  smb_hta *table = hta_create(&hta_test_constant_hash, &hta_int_comp,
                              sizeof(int), sizeof(int));

  for (i = 0; i < 20; i++) {
    key = i;
    value = -i;
    hta_insert(table, &key, &value);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_INT_EQ(table->length, i+1);
  }

  // Remove one from the middle of the bucket list.
  key = 10;
  hta_remove(table, &key, &status);
  TA_INT_EQ(status, SMB_SUCCESS);
  TA_INT_EQ(table->length, 19);

  // Remove one from the beginning of the bucket list.
  key= 0;
  hta_remove(table, &key, &status);
  TA_INT_EQ(status, SMB_SUCCESS);
  TA_INT_EQ(table->length, 18);

  //hta_print(table, 0);

  // Remove from the end of the list.
  key = 19;
  hta_remove(table, &key, &status);
  TA_INT_EQ(status, SMB_SUCCESS);
  TA_INT_EQ(table->length, 17);

  //hta_print(table, 0);

  // Verify that the other items are still there.
  for (i = 1; i < 10; i++) {
    key = i;
    rv = hta_get(table, &key, &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_INT_EQ(*rv, (int)-i);
  }

  for (i = 11; i < 19; i++) {
    key = i;
    rv = hta_get(table, &key, &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_INT_EQ(*rv, (int)-i);
  }

  hta_delete(table);
  return 0;
}

/**
   This test adds to the hash table until it is forced to reallocate.  Then it
   checks that every value is still accessible.
 */
int hta_test_resize()
{
  smb_status status = SMB_SUCCESS;
  int key, value, *rv;
  unsigned int i;
  // Truncating addition will trim this to the number just before expanding.
  unsigned int last_stable = 1 + (int) (HASH_TABLE_INITIAL_SIZE * HASH_TABLE_MAX_LOAD_FACTOR);
  smb_hta *table = hta_create(hta_test_linear_hash, &hta_int_comp, sizeof(int), sizeof(int));

  for (i = 0; i < last_stable; i++) {
    key = i;
    value = -i;
    hta_insert(table, &key, &value);
    TA_INT_EQ(table->allocated, HASH_TABLE_INITIAL_SIZE);
    TA_INT_EQ(table->length, i + 1);
  }

  key = last_stable;
  value = -last_stable;
  hta_insert(table, &key, &value);
  TA_INT_GT(table->allocated, HASH_TABLE_INITIAL_SIZE);
  TA_INT_EQ(table->length, last_stable + 1);

  for (i = 0; i <= last_stable; i++) {
    key = i;
    rv = hta_get(table, &key, &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_INT_EQ(*rv, (int)-i);
  }

  hta_delete(table);
  return 0;
}

int hta_test_duplicate()
{
  smb_status status = SMB_SUCCESS;
  int i;
  char **rv;
  char *newKey = "not the first value";
  smb_hta *table = hta_create(hta_string_hash, &hta_string_comp, sizeof(char*), sizeof(char*));

  for (i = 0; i < TEST_PAIRS; i++) {
    hta_insert(table, &hta_test_keys[i], &hta_test_values[i]);
  }

  for (i = 0; i < TEST_PAIRS; i += 2) {
    TA_INT_EQ(table->length, TEST_PAIRS);

    hta_insert(table, &hta_test_keys[i], &newKey);
    rv = (char**)hta_get(table, &hta_test_keys[i], &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    TA_PTR_EQ(*rv, newKey);
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    rv = (char**)hta_get(table, &hta_test_keys[i], &status);
    TA_INT_EQ(status, SMB_SUCCESS);
    if (i % 2 == 1) {
      TA_PTR_EQ(*rv, hta_test_values[i]);
    } else {
      TA_PTR_EQ(*rv, newKey);
    }
  }

  hta_delete(table);
  return 0;
}

void hta_test()
{
  smb_ut_group *group = su_create_test_group("test/hta.c");

  smb_ut_test *insert = su_create_test("insert", hta_test_insert);
  su_add_test(group, insert);

  smb_ut_test *remove = su_create_test("remove", hta_test_remove);
  su_add_test(group, remove);

  smb_ut_test *remove_invalid = su_create_test("remove_invalid", hta_test_remove_invalid);
  su_add_test(group, remove_invalid);

  smb_ut_test *buckets = su_create_test("buckets", hta_test_buckets);
  su_add_test(group, buckets);

  smb_ut_test *resize = su_create_test("resize", hta_test_resize);
  su_add_test(group, resize);

  smb_ut_test *duplicate = su_create_test("duplicate", hta_test_duplicate);
  su_add_test(group, duplicate);

  su_run_group(group);
  su_delete_group(group);
}
