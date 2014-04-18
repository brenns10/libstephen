/*******************************************************************************

  File:         hashtable.c

  Author:       Stephen Brennan

  Date Created: Thursday,  7 November 2013

  Description:  A simple hash table, with an included hash function for strings.

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "libstephen.h"

////////////////////////////////////////////////////////////////////////////////
// Private Helper Functions
////////////////////////////////////////////////////////////////////////////////

/*
  Returns whether or not two DATA structs are equal.
 */
int data_equal(DATA d1, DATA d2)
{
  return (d1.data_llint == d2.data_llint)
    && (d1.data_dbl == d2.data_dbl)
    && (d1.data_ptr == d2.data_ptr);
}

/*
  Returns the next hashtable size.
 */
int ht_next_size(int current)
{
  return 2 * current + 1; // Try not to use something with a common factor of 1.
                          // I'm also striving for simplicity here.  It would
                          // probably be best to use only prime numbers here,
                          // but this is considerably simpler.
}

/*
  Create a hash table bucket.
 */
smb_ht_bckt *ht_bucket_create(DATA dKey, DATA dValue, smb_ht_bckt *pNext)
{
  smb_ht_bckt *pBucket;
  pBucket = (smb_ht_bckt*) malloc(sizeof(smb_ht_bckt));
  if (!pBucket) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ht_bckt));
  pBucket->key = dKey;
  pBucket->value = dValue;
  pBucket->next = pNext;
  return pBucket;
}

/*
  Delete a hash table bucket.
 */
void ht_bucket_delete(smb_ht_bckt *pToDelete)
{
  free(pToDelete);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht_bckt));
}

/*
  Find a pointer to the first hash entry with key==dKey.  If it doesn't exist,
  return a pointer to the tail of the list.  If the bucket is null, return NULL.
 */
smb_ht_bckt *ht_find_in_bucket(smb_ht_bckt *pBucket, DATA dKey)
{
  if (pBucket == NULL) return NULL;

  while (pBucket->next && !data_equal(pBucket->key, dKey)) pBucket = pBucket->next;

  return pBucket;
}

/*
  Insert data into the hash table.  If the key already exists in the table,
  overwrites with a new value.
 */
void ht_insert_bucket(smb_ht *pTable, smb_ht_bckt *pBucket)
{
  smb_ht_bckt *curr;
  unsigned int index = pTable->hash(pBucket->key) % pTable->allocated;
  
  if (pTable->table[index]) {
    // A linked list already exists here.
    curr = ht_find_in_bucket(pTable->table[index], pBucket->key);
    if (data_equal(curr->key, pBucket->key)) {
      // Key already exists in table
      curr->value = pBucket->value;
      ht_bucket_delete(pBucket);
    } else {
      // assert curr->next == NULL
      curr->next = pBucket;
      pTable->length++;
    }
  } else {
    // No linked list exists yet
    pTable->table[index] = pBucket;
    pTable->length++;
  }
 
}

/*
  Resize the hash table, adding increment to the capacity of the table.
 */
void ht_resize(smb_ht *pTable)
{
  smb_ht_bckt **pOldBuffer;
  smb_ht_bckt *curr, *temp;
  int index, oldLength, oldAllocated;

  // Step one: allocate new space for the table
  pOldBuffer = pTable->table;
  oldLength = pTable->length;
  oldAllocated = pTable->allocated;
  pTable->length = 0;
  pTable->allocated = ht_next_size(oldAllocated);
  pTable->table = (smb_ht_bckt**) malloc(pTable->allocated * sizeof(smb_ht_bckt*));
  if (!pTable->table) {
    // We want to preserve the data already contained in the table.
    RAISE(ALLOCATION_ERROR);
    pTable->table = pOldBuffer;
    pTable->length = oldLength;
    pTable->allocated = oldAllocated;
    return;
  }
  // Zero out the new block too.
  memset((void*)pTable->table, 0, pTable->allocated * sizeof(smb_ht_bckt*));
  SMB_INCREMENT_MALLOC_COUNTER(pTable->allocated * sizeof(smb_ht_bckt*));
  
  // Step two, add the old items to the new table (no freeing, please)
  for (index = 0; index < oldAllocated; index++) {
    if (pOldBuffer[index]) {
      // a bucket exists here
      curr = pOldBuffer[index];
      while (curr) {
        temp = curr->next; // Hang on to the next pointer...it could be changed
                           // once added to the "new" hash table
        curr->next = NULL;
        ht_insert_bucket(pTable, curr);
        curr = temp;
      }
    }
  }

  // Step three: free old data.
  free(pOldBuffer);
  SMB_DECREMENT_MALLOC_COUNTER(oldAllocated * sizeof(smb_ht_bckt*));
}

/*
  Return the load factor of a hash table.
 */
double ht_load_factor(smb_ht *pTable)
{
  return ((double) pTable->length) / ((double) pTable->allocated);
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface Functions
////////////////////////////////////////////////////////////////////////////////

void ht_init(smb_ht *pTable, HASH_FUNCTION hash_func)
{
  CLEAR_ALL_ERRORS;

  // Initialize values
  pTable->length = 0;
  pTable->allocated = HASH_TABLE_INITIAL_SIZE;
  pTable->hash = hash_func;

  // Create the bucket list
  pTable->table = (smb_ht_bckt**) malloc(HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));
  if (!pTable->table) {
    SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht));
    free(pTable);
    RAISE(ALLOCATION_ERROR);
    return;
  }
  SMB_INCREMENT_MALLOC_COUNTER(HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));

  // Zero out the entries in the table so we don't get segmentation faults.
  memset((void*)pTable->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt*));  
}

smb_ht *ht_create(HASH_FUNCTION hash_func)
{
  CLEAR_ALL_ERRORS;

  // Allocate and create the table.
  smb_ht *pTable;
  pTable = (smb_ht*) malloc(sizeof(smb_ht));
  if (!pTable) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ht));

  ht_init(pTable, hash_func);
  if (CHECK(ALLOCATION_ERROR)) {
    return NULL;
  }

  return pTable;
}

void ht_destroy_act(smb_ht *pTable, DATA_ACTION deleter)
{
  int i;
  smb_ht_bckt *curr, *temp;

  if (!pTable) return;

  // Free all smb_ht_bckt's in the table
  for (i = 0; i < pTable->allocated; i++) {
    curr = pTable->table[i];
    while (curr) {
      temp = curr->next;
      if (deleter)
        deleter(curr->value);
      ht_bucket_delete(curr);
      curr = temp;
    }
    pTable->table[i] = NULL;
  }
}

void ht_destroy(smb_ht *pTable)
{
  ht_destroy_act(pTable, NULL);
}

void ht_delete_act(smb_ht *pTable, DATA_ACTION deleter)
{
  if (!pTable) return;

  ht_destroy_act(pTable, deleter);

  SMB_DECREMENT_MALLOC_COUNTER(pTable->allocated * sizeof(smb_ht_bckt*));
  free(pTable->table);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ht));
  free(pTable);
}

void ht_delete(smb_ht *pTable)
{
  ht_delete_act(pTable, NULL);
}

/*
  Insert data into the hash table.  If the key already exists in the table,
  overwrites with a new value.
 */
void ht_insert(smb_ht *pTable, DATA dKey, DATA dValue)
{
  CLEAR_ALL_ERRORS;

  if (ht_load_factor(pTable) > HASH_TABLE_MAX_LOAD_FACTOR) {
    ht_resize(pTable);
    if (CHECK(ALLOCATION_ERROR)) return;
  }

  smb_ht_bckt *pBucket = ht_bucket_create(dKey, dValue, NULL);
  if (!pBucket) return;
  
  ht_insert_bucket(pTable, pBucket);
}

void ht_remove_act(smb_ht *pTable, DATA dKey, DATA_ACTION deleter)
{
  CLEAR_ALL_ERRORS;

  smb_ht_bckt *curr, *prev;
  int index = pTable->hash(dKey) % pTable->allocated;

  // Stop if the key doesn't exist in the table.
  if (!pTable->table[index]) {
    RAISE(NOT_FOUND_ERROR);
    return;
  }

  curr = pTable->table[index];
  prev = NULL;
  
  while (curr && !data_equal(curr->key, dKey)) {
    prev = curr;
    curr = curr->next;
  }

  if (curr && prev) {
    // Found a match, and there is something before it in the linked list.
    prev->next = curr->next;
    if (deleter)
      deleter(curr->value); // Perform user requested action
    ht_bucket_delete(curr);
    pTable->length--;
  } else if (curr && !prev) {
    // Found a match, and it is the first in the linked list
    pTable->table[index] = curr->next;
    if (deleter)
      deleter(curr->value); // Perform user requested action
    ht_bucket_delete(curr);
    pTable->length--;
  } else {
    // Didn't find a match in this bucket list
    RAISE(NOT_FOUND_ERROR);
  }
}

void ht_remove(smb_ht *pTable, DATA dKey)
{
  ht_remove_act(pTable, dKey, NULL);
}

DATA ht_get(smb_ht const *pTable, DATA dKey)
{
  CLEAR_ALL_ERRORS;

  smb_ht_bckt *pBucket;
  DATA d;
  int index = pTable->hash(dKey) % pTable->allocated;

  if (pTable->table[index]) {
    pBucket = ht_find_in_bucket(pTable->table[index], dKey);
    if (data_equal(pBucket->key, dKey)){
      return pBucket->value;
    }
  }
  // ELSE: Not found
  RAISE(NOT_FOUND_ERROR);
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

/*
  Print the hash table.  If full_mode is nonzero, it will print every cell of
  the hash table, regardless of whether or not it contains anything.
 */
void ht_print(smb_ht const *pTable, int full_mode)
{
  smb_ht_bckt *curr = NULL;
  int i;
  int printed = 0;

  for (i = 0; i < pTable->allocated; i++) {
    curr = pTable->table[i];
    if (pTable->table[i] || full_mode)
      printf("[%d]: ", i);
    while (curr) {
      printf("0x%Lx|0x%Lx, ", curr->key.data_llint, curr->value.data_llint);
      curr = curr->next;
      printed++;
    }
    if (pTable->table[i] || full_mode)
      printf("\n");
  }

  if (printed != pTable->length)
    printf("Error: %d items printed, but %d items are recorded by hash table.\n", 
           printed, pTable->length);
}
