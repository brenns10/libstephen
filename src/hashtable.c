/***************************************************************************//**

  @file         hashtable.c

  @author       Stephen Brennan

  @date         Created Thursday, 7 November 2013

  @brief        A simple hash table and string hash function.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
   @exception ALLOCATION_ERROR If memory allocation failed.
 */
smb_ht_bckt *ht_bucket_create(DATA key, DATA value, smb_ht_bckt *next,
                              smb_status *status)
{
  smb_ht_bckt *pBucket;
  pBucket = (smb_ht_bckt*) malloc(sizeof(smb_ht_bckt));
  if (!pBucket) {
    *status = SMB_ALLOCATION_ERROR;
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ht_bckt));
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
  free(pToDelete);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht_bckt));
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
   @param[out] status Status variable.
   @exception ALLOCATION_ERROR If the new table couldn't be allocated, no change
   is made.
 */
void ht_resize(smb_ht *table, smb_status *status)
{
  smb_ht_bckt **pOldBuffer;
  smb_ht_bckt *curr, *temp;
  int index, oldLength, oldAllocated;

  // Step one: allocate new space for the table
  pOldBuffer = table->table;
  oldLength = table->length;
  oldAllocated = table->allocated;
  table->length = 0;
  table->allocated = ht_next_size(oldAllocated);
  table->table = (smb_ht_bckt**) malloc(table->allocated * sizeof(smb_ht_bckt*));
  if (!table->table) {
    // We want to preserve the data already contained in the table.
    *status = SMB_ALLOCATION_ERROR;
    table->table = pOldBuffer;
    table->length = oldLength;
    table->allocated = oldAllocated;
    return;
  }
  // Zero out the new block too.
  memset((void*)table->table, 0, table->allocated * sizeof(smb_ht_bckt*));
  SMB_INCREMENT_MALLOC_COUNTER(table->allocated * sizeof(smb_ht_bckt*));

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
  free(pOldBuffer);
  SMB_DECREMENT_MALLOC_COUNTER(oldAllocated * sizeof(smb_ht_bckt*));
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

////////////////////////////////////////////////////////////////////////////////
// Public Interface Functions
////////////////////////////////////////////////////////////////////////////////

/**
   @brief Initialize a hash table in memory already allocated.

   @param table A pointer to the table to initialize.
   @param hash_func A hash function for the table.
   @param equal A comparison function for DATA.
   @param[out] status Status variable.
   @exception ALLOCATION_ERROR
 */
void ht_init(smb_ht *table, HASH_FUNCTION hash_func, DATA_COMPARE equal,
             smb_status *status)
{
  *status = SMB_SUCCESS;
  // Initialize values
  table->length = 0;
  table->allocated = HASH_TABLE_INITIAL_SIZE;
  table->hash = hash_func;
  table->equal = equal;

  // Create the bucket list
  table->table = (smb_ht_bckt**) malloc(HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));
  if (!table->table) {
    SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht));
    free(table);
    *status = SMB_ALLOCATION_ERROR;
    return;
  }
  SMB_INCREMENT_MALLOC_COUNTER(HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));

  // Zero out the entries in the table so we don't get segmentation faults.
  memset((void*)table->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));
}

/**
   @brief Allocate and initialize a hash table.

   @param hash_func A function that takes one DATA and returns a hash value
   generated from it.  It should be a good hash function.
   @param equal A comparison function for DATA.
   @param[out] status Status variable.
   @returns A pointer to the new hash table.
   @exception ALLOCATION_ERROR
 */
smb_ht *ht_create(HASH_FUNCTION hash_func, DATA_COMPARE equal,
                  smb_status *status)
{
  *status = SMB_SUCCESS;

  // Allocate and create the table.
  smb_ht *table;
  table = (smb_ht*) malloc(sizeof(smb_ht));
  if (!table) {
    *status = SMB_ALLOCATION_ERROR;
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ht));

  ht_init(table, hash_func, equal, status);
  if (*status == SMB_ALLOCATION_ERROR) {
    free(table);
    SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht));
    return NULL;
  }

  return table;
}

/**
   @brief Free resources used by the hash table, but does not free the pointer
   itself.  Perform an action on the data as it is deleted.

   Useful for stack valued hash tables.  A deleter must be specified in this
   function call.

   @param table The table to destroy.
   @param deleter The deletion action on the data.
 */
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
}

/**
   @brief Free any resources used by the hash table, but doesn't free the
   pointer.  Doesn't perform any actions on the data as it is deleted.

   If pointers are contained within the hash table, they are not freed.  Use
   ht_destroy_act to specify a deletion action on the hash table.

   @param table The table to destroy.
 */
void ht_destroy(smb_ht *table)
{
  ht_destroy_act(table, NULL);
}

/**
   @brief Free the hash table and its resources.  Perform an action on each data before
   freeing the table.  Useful for freeing pointers stored in the table.

   @param table The table to free.
   @param deleter The action to perform on each value in the hash table before
   deletion.
 */
void ht_delete_act(smb_ht *table, DATA_ACTION deleter)
{
  if (!table) return;

  ht_destroy_act(table, deleter);

  SMB_DECREMENT_MALLOC_COUNTER(table->allocated * sizeof(smb_ht_bckt*));
  free(table->table);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht));
  free(table);
}

/**
   @brief Free the hash table and its resources.  No pointers contained in the
   table will be freed.

   @param table The table to free.
 */
void ht_delete(smb_ht *table)
{
  ht_delete_act(table, NULL);
}

/**
   @brief Insert data into the hash table.

   Expands the hash table if the load factor is below a threshold.  If the key
   already exists in the table, then the function will overwrite it with the new
   data provided.

   @param table A pointer to the hash table.
   @param key The key to insert.
   @param value The value to insert at the key.
   @param[out] status Status variable.
   @exception ALLOCATION_ERROR
 */
void ht_insert(smb_ht *table, DATA key, DATA value, smb_status *status)
{
  *status = SMB_SUCCESS;

  if (ht_load_factor(table) > HASH_TABLE_MAX_LOAD_FACTOR) {
    ht_resize(table, status);
    if (*status == SMB_ALLOCATION_ERROR) return;
  }

  smb_ht_bckt *pBucket = ht_bucket_create(key, value, NULL, status);
  if (*status == SMB_ALLOCATION_ERROR) return;

  ht_insert_bucket(table, pBucket);
}

/**
   @brief Remove the key, value pair stored in the hash table.

   @param table A pointer to the hash table.
   @param key The key to delete.
   @param deleter The action to perform on the value before removing it.
   @param[out] status Status variable.
   @exception SMB_NOT_FOUND_ERROR If an item with the given key is not found.
 */
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

/**
   @brief Remove the key, value pair stored in the hash table.

   This function does not call a deleter on the stored data.

   @param table A pointer to the hash table.
   @param key The key to delete.
   @param[out] status Status variable.
   @exception SMB_NOT_FOUND_ERROR If an item with the given key is not found.
 */
void ht_remove(smb_ht *table, DATA key, smb_status *status)
{
  ht_remove_act(table, key, NULL, status);
}

/**
   @brief Return the value associated with the key provided.

   @param table A pointer to the hash table.
   @param key The key whose value to retrieve.
   @param[out] status Status variable.
   @returns The value associated the key.
   @exception NOT_FOUND_ERROR
 */
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

/**
   @brief Return the hash of the data, interpreting it as a string.

   @param data The string to hash, assuming that the value contained is a char*.
   @returns The hash value of the string.
 */
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

/**
   @brief Print the entire hash table.

   This function is useful for diagnostics.  It can show every row in the table
   (with full_mode) so you can see how well entries are distributed in the
   table.  Or, it can be compact and show just the rows with data.

   @param table The table to print.
   @param full_mode Whether to print every row in the hash table.
 */
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
      printf("0x%Lx|0x%Lx, ", curr->key.data_llint, curr->value.data_llint);
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
