/***************************************************************************//**

  @file         hashtable.c

  @author       Stephen Brennan

  @date         Created Thursday, 7 November 2013

  @brief        Implementation of "libstephen/ht.h".

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <string.h>
#include <stdio.h>

#include "libstephen/ht.h"

/*******************************************************************************

                               Private Functions

*******************************************************************************/

/**
   @brief Returns the next hashtable size.

   @param current The current size of the hash table.
   @returns The next size in the sequence for hash tables.
 */
int ht_next_size(int current)
{
  return 2 * current + 1; // Try not to use something with a common factor of 1.
                          // I'm also striving for simplicity here.  It would
                          // probably be best to use only prime numbers here,
                          // but this is considerably simpler.
}

/**
   @brief Allocate and initialize a hash table bucket.

   @param key The key stored
   @param value The value stored
   @param next The next bucket pointer
   @param[out] status Status variable.
   @returns A pointer to a new hash table bucket.
 */
smb_ht_bckt *ht_bucket_create(DATA key, DATA value, smb_ht_bckt *next)
{
  smb_ht_bckt *pBucket;
  pBucket = smb_new(smb_ht_bckt, 1);
  pBucket->key = key;
  pBucket->value = value;
  pBucket->next = next;
  return pBucket;
}

/**
   @brief Free a hash table bucket.

   @param pToDelete The bucket to delete.
 */
void ht_bucket_delete(smb_ht_bckt *pToDelete)
{
  smb_free(pToDelete);
}

/**
  @brief Finds a value within a singly linked hash table bucket list.

  This function actually performs a lot of tasks rolled into one.  First, if you
  pass NULL as the pBucket, it will return NULL.  More interestingly, it will
  return the pointer to the bucket that equals key if it exists, or the last
  bucket in the list if there is none equalling key.  This way, you can just
  check the return value, and if key == pBucket->key, you have the pre-xisting
  spot for key.  Otherwise, you can just create the next item in the list.

  @param pBucket The bucket to search within.
  @param key The key to search for.
  @returns The bucket containing key.
 */
smb_ht_bckt *ht_find_in_bucket(smb_ht_bckt *pBucket, DATA key,
                               DATA_COMPARE equal)
{
  if (pBucket == NULL) return NULL;

  while (pBucket->next && equal(pBucket->key, key) != 0)
    pBucket = pBucket->next;

  return pBucket;
}

/**
   @brief Insert a bucket with data into the hash table.  If the key already
   exists in the table, overwrites with a new value.

   This function is at a lower lever than ht_insert.  In order to properly
   implement a hash table, the table must expand as you add to it, which is the
   part that ht_insert takes care of.

   @param table The table to insert into.
   @param pBucket The bucket containing the data to insert.  Note that this
   bucket may not be the final bucket containing the data.
 */
void ht_insert_bucket(smb_ht *table, smb_ht_bckt *pBucket)
{
  smb_ht_bckt *curr;
  unsigned int index = table->hash(pBucket->key) % table->allocated;

  if (table->table[index]) {
    // A linked list already exists here.
    curr = ht_find_in_bucket(table->table[index], pBucket->key, table->equal);
    if (table->equal(curr->key, pBucket->key) == 0) {
      // Key already exists in table
      curr->value = pBucket->value;
      ht_bucket_delete(pBucket);
    } else {
      // assert curr->next == NULL
      curr->next = pBucket;
      table->length++;
    }
  } else {
    // No linked list exists yet
    table->table[index] = pBucket;
    table->length++;
  }

}

/**
   @brief Expand the hash table, adding increment to the capacity of the table.

   @param table The table to expand.
 */
void ht_resize(smb_ht *table)
{
  smb_ht_bckt **pOldBuffer;
  smb_ht_bckt *curr, *temp;
  int index, oldAllocated;

  // Step one: allocate new space for the table
  pOldBuffer = table->table;
  oldAllocated = table->allocated;
  table->length = 0;
  table->allocated = ht_next_size(oldAllocated);
  table->table = smb_new(smb_ht_bckt*, table->allocated);

  // Zero out the new block too.
  memset((void*)table->table, 0, table->allocated * sizeof(smb_ht_bckt*));

  // Step two, add the old items to the new table (no freeing, please)
  for (index = 0; index < oldAllocated; index++) {
    if (pOldBuffer[index]) {
      // a bucket exists here
      curr = pOldBuffer[index];
      while (curr) {
        temp = curr->next; // Hang on to the next pointer...it could be changed
                           // once added to the "new" hash table
        curr->next = NULL;
        ht_insert_bucket(table, curr);
        curr = temp;
      }
    }
  }

  // Step three: free old data.
  smb_free(pOldBuffer);
}

/**
   @brief Return the load factor of a hash table.

   @param table The table to find the load factor of.
   @returns The load factor of the hash table.
 */
double ht_load_factor(smb_ht *table)
{
  return ((double) table->length) / ((double) table->allocated);
}

/*******************************************************************************

                           Public Interface Functions

*******************************************************************************/

void ht_init(smb_ht *table, HASH_FUNCTION hash_func, DATA_COMPARE equal)
{
  // Initialize values
  table->length = 0;
  table->allocated = HASH_TABLE_INITIAL_SIZE;
  table->hash = hash_func;
  table->equal = equal;

  // Create the bucket list
  table->table = smb_new(smb_ht_bckt*, HASH_TABLE_INITIAL_SIZE);

  // Zero out the entries in the table so we don't get segmentation faults.
  memset((void*)table->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));
}

smb_ht *ht_create(HASH_FUNCTION hash_func, DATA_COMPARE equal)
{
  // Allocate and create the table.
  smb_ht *table;
  table = smb_new(smb_ht, 1);
  ht_init(table, hash_func, equal);
  return table;
}

void ht_destroy_act(smb_ht *table, DATA_ACTION deleter)
{
  int i;
  smb_ht_bckt *curr, *temp;

  if (!table) return;

  // Free all smb_ht_bckt's in the table
  for (i = 0; i < table->allocated; i++) {
    curr = table->table[i];
    while (curr) {
      temp = curr->next;
      if (deleter)
        deleter(curr->value);
      ht_bucket_delete(curr);
      curr = temp;
    }
    table->table[i] = NULL;
  }

  smb_free(table->table);
}

void ht_destroy(smb_ht *table)
{
  ht_destroy_act(table, NULL);
}

void ht_delete_act(smb_ht *table, DATA_ACTION deleter)
{
  if (!table) {
    return;
  }

  ht_destroy_act(table, deleter);
  smb_free(table);
}

void ht_delete(smb_ht *table)
{
  ht_delete_act(table, NULL);
}

void ht_insert(smb_ht *table, DATA key, DATA value)
{
  if (ht_load_factor(table) > HASH_TABLE_MAX_LOAD_FACTOR) {
    ht_resize(table);
  }

  smb_ht_bckt *pBucket = ht_bucket_create(key, value, NULL);
  ht_insert_bucket(table, pBucket);
}

void ht_remove_act(smb_ht *table, DATA key, DATA_ACTION deleter,
                   smb_status *status)
{
  *status = SMB_SUCCESS;

  smb_ht_bckt *curr, *prev;
  int index = table->hash(key) % table->allocated;

  // Stop if the key doesn't exist in the table.
  if (!table->table[index]) {
    *status = SMB_NOT_FOUND_ERROR;
    return;
  }

  curr = table->table[index];
  prev = NULL;

  while (curr && table->equal(curr->key, key) != 0) {
    prev = curr;
    curr = curr->next;
  }

  if (curr && prev) {
    // Found a match, and there is something before it in the linked list.
    prev->next = curr->next;
    if (deleter)
      deleter(curr->value); // Perform user requested action
    ht_bucket_delete(curr);
    table->length--;
  } else if (curr && !prev) {
    // Found a match, and it is the first in the linked list
    table->table[index] = curr->next;
    if (deleter)
      deleter(curr->value); // Perform user requested action
    ht_bucket_delete(curr);
    table->length--;
  } else {
    // Didn't find a match in this bucket list
    *status = SMB_NOT_FOUND_ERROR;
  }
}

void ht_remove(smb_ht *table, DATA key, smb_status *status)
{
  ht_remove_act(table, key, NULL, status);
}

DATA ht_get(smb_ht const *table, DATA key, smb_status *status)
{
  *status = SMB_SUCCESS;

  smb_ht_bckt *pBucket;
  DATA d;
  int index = table->hash(key) % table->allocated;

  if (table->table[index]) {
    pBucket = ht_find_in_bucket(table->table[index], key, table->equal);
    if (table->equal(pBucket->key, key) == 0){
      return pBucket->value;
    }
  }
  // ELSE: Not found
  *status = SMB_NOT_FOUND_ERROR;
  return d;
}

unsigned int ht_string_hash(DATA data)
{
  char *theString = (char *)data.data_ptr;
  unsigned int hash = 0;

  while (theString && *theString != '\0' ) {
    hash = (hash << 5) - hash + *theString;
    theString++;
  }

  // printf("Hash of \"%s\": %u\n", (char*)data.data_ptr, hash);
  return hash;
}

void ht_print(smb_ht const *table, int full_mode)
{
  smb_ht_bckt *curr = NULL;
  int i;
  int printed = 0;

  for (i = 0; i < table->allocated; i++) {
    curr = table->table[i];
    if (table->table[i] || full_mode)
      printf("[%d]: ", i);
    while (curr) {
      printf("0x%llx|0x%llx, ", curr->key.data_llint, curr->value.data_llint);
      curr = curr->next;
      printed++;
    }
    if (table->table[i] || full_mode)
      printf("\n");
  }

  if (printed != table->length)
    printf("Error: %d items printed, but %d items are recorded by hash table.\n",
           printed, table->length);
}
