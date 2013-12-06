/*******************************************************************************

  File:         hashtable.c

  Author:       Stephen Brennan

  Date Created: Thursday,  7 November 2013

  Description:  A simple hash table, with an included hash function for strings.

*******************************************************************************/

#include <string.h>
#include "libstephen.h"

////////////////////////////////////////////////////////////////////////////////
// Data structure definitions
////////////////////////////////////////////////////////////////////////////////

#define HT_BUCKET struct ht_bucket
HT_BUCKET
{
  DATA key;
  DATA value;
  HT_BUCKET *next;
};

#define HASH_TABLE struct hash_table
HASH_TABLE
{
  int length;
  int allocated;
  int (*hash)(DATA dData);
  HT_BUCKET **table;
};

#define HASH_TABLE_INITIAL_SIZE 256
#define HASH_TABLE_MAX_LOAD_FACTOR 0.7

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
  Create a hash table bucket.
 */
HT_BUCKET *ht_bucket_create(DATA dKey, DATA dValue, HT_BUCKET *pNext)
{
  HT_BUCKET *pBucket;
  pBucket = (HT_BUCKET*) malloc(sizeof(HT_BUCKET));
  if (!pBucket) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(HT_BUCKET));
  pBucket->key = dKey;
  pBucket->value = dValue;
  pBucket->next = pNext;
  return pBucket;
}

/*
  Delete a hash table bucket.
 */
void ht_bucket_delete(HT_BUCKET *pToDelete)
{
  free(pToDelete);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(HT_BUCKET));
}

/*
  Find a pointer to the first hash entry with key==dKey.  If it doesn't exist,
  return a pointer to the tail of the list.  If the bucket is null, return NULL.
 */
HT_BUCKET *ht_find_in_bucket(HT_BUCKET *pBucket, DATA dKey)
{
  if (pBucket == NULL) return NULL;

  while (pBucket->next && !data_equal(pBucket->key, dKey)) pBucket = pBucket->next;

  return pBucket;
}

/*
  Insert data into the hash table.  If the key already exists in the table,
  overwrites with a new value.
 */
void ht_insert_bucket(HASH_TABLE *pTable, HT_BUCKET *pBucket)
{
  HT_BUCKET *curr;
  int index = pTable->hash(pBucket->key) % pTable->allocated;
  
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
    }
  } else {
    // No linked list exists yet
    pTable->table[index] = pBucket;
  }
  
  pTable->length++;
}

/*
  Resize the hash table, adding increment to the capacity of the table.
 */
void ht_resize(HASH_TABLE *pTable, size_t increment)
{
  HT_BUCKET **pOldBuffer;
  HT_BUCKET *curr, *temp;
  int index, oldLength, oldAllocated;

  // Step one: allocate new space for the table
  pOldBuffer = pTable->table;
  oldLength = pTable->length;
  oldAllocated = pTable->allocated;
  pTable->length = 0;
  pTable->allocated += increment;
  pTable->table = (HT_BUCKET**) malloc(pTable->allocated * sizeof(HT_BUCKET*));
  if (!pTable->table) {
    // We want to preserve the data already contained in the table.
    RAISE(ALLOCATION_ERROR);
    pTable->table = pOldBuffer;
    pTable->length = oldLength;
    pTable->allocated = oldAllocated;
    return;
  }
  
  // Step two, add the old items to the new table (no freeing, please)
  for (index = 0; index < oldAllocated; index++) {
    if (pOldBuffer[index]) {
      // a bucket exists here
      curr = pOldBuffer[index];
      while (curr) {
        temp = curr->next; // Hang on to the next pointer...it could be changed
                           // once added to the "new" hash table
        ht_insert_bucket(pTable, curr);
        curr = temp;
      }
    }
  }

  // Step three: free old data.
  free(pOldBuffer);
  SMB_DECREMENT_MALLOC_COUNTER(oldAllocated * sizeof(HT_BUCKET*));
}

/*
  Return the load factor of a hash table.
 */
double ht_load_factor(HASH_TABLE *pTable)
{
  return ((double) pTable->length) / ((double) pTable->allocated);
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface Functions
////////////////////////////////////////////////////////////////////////////////

HASH_TABLE *ht_create(int (*hash_function)(DATA dData))
{
  CLEAR_ALL_ERRORS;

  // Allocate and create the table.
  HASH_TABLE *pTable;
  pTable = (HASH_TABLE*) malloc(sizeof(HASH_TABLE));
  if (!pTable) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }

  SMB_INCREMENT_MALLOC_COUNTER(sizeof(HASH_TABLE));

  // Initialize values
  pTable->length = 0;
  pTable->allocated = HASH_TABLE_INITIAL_SIZE;
  pTable->hash = hash_function;
  pTable->table = (HT_BUCKET**) malloc(HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));
  if (!pTable->table) {
    SMB_DECREMENT_MALLOC_COUNTER(sizeof(HASH_TABLE));
    free(pTable);
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }

  SMB_INCREMENT_MALLOC_COUNTER(HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));

  // Zero out the entries in the table so we don't get segmentation faults.
  memset((void*)pTable->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));

  return pTable;
}

/*
  Insert data into the hash table.  If the key already exists in the table,
  overwrites with a new value.
 */
void ht_insert(HASH_TABLE *pTable, DATA dKey, DATA dValue)
{
  CLEAR_ALL_ERRORS;

  if (ht_load_factor(pTable) > HASH_TABLE_MAX_LOAD_FACTOR) {
    ht_resize(pTable, HASH_TABLE_INITIAL_SIZE);
  }

  HT_BUCKET *pBucket = ht_bucket_create(dKey, dValue, NULL);
  
  ht_insert_bucket(pTable, pBucket);
}

void ht_remove(HASH_TABLE *pTable, DATA dKey)
{
  CLEAR_ALL_ERRORS;

  HT_BUCKET *curr, *prev;
  int index = pTable->hash(dKey) % pTable->allocated;

  // Stop if the key doesn't exist in the table.
  if (!pTable->table[index])
    return;

  curr = pTable->table[index];
  prev = NULL;
  
  while (curr && !data_equal(curr->key, dKey)) {
    prev = curr;
    curr = curr->next;
  }

  if (curr) {
    // Cut curr out of the list
    prev->next = curr->next;
    ht_bucket_delete(curr);
    pTable->length--;
  }
}

DATA ht_get(HASH_TABLE const *pTable, DATA dKey)
{
  CLEAR_ALL_ERRORS;

  HT_BUCKET *pBucket;
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

int ht_string_hash(DATA data)
{
  char *theString = (char *)data.data_ptr;
  int hash = 0;

  while (theString && *theString != '\0' ) {
    hash = hash << 5 - hash + *theString;
    theString++;
  }
  
  return hash;
}
