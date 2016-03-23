/***************************************************************************//**

  @file         hashtable.c

  @author       Stephen Brennan

  @date         Created Thursday, 7 November 2013

  @brief        Implementation of "libstephen/ht.h".

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
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
   @brief Find the proper index for insertion into the table.
   @param obj Hash table object.
   @param key Key we're inserting.
 */
unsigned int ht_find_insert(const smb_ht *obj, DATA key)
{
  unsigned int index = obj->hash(key) % obj->allocated;
  unsigned int j = 1;

  // Continue searching until we either find a non-full slot, or we find the key
  // we're trying to insert.
  // until (cell.mark != full || cell.key == key)
  // while (cell.mark == full && cell.key != key)
  while (obj->table[index].mark == HT_FULL &&
         obj->equal(key, obj->table[index].key) != 0) {
    // This is quadratic probing, but I'm avoiding squaring numbers:
    // j:     1, 3, 5, 7,  9, 11, ..
    // index: 0, 1, 4, 9, 16, 25, 36
    index = (index + j) % obj->allocated;
    j += 2;
  }

  return index;
}

/**
   @brief Find the proper index for retrieval from the table.
   @param obj Hash table object.
   @param key Key we're looking up.
 */
unsigned int ht_find_retrieve(const smb_ht *obj, DATA key)
{
  unsigned int index = obj->hash(key) % obj->allocated;
  unsigned int j = 1;

  // Continue searching until we either find an empty slot, or we find the key
  // we're trying to insert.
  // until (cell.mark == empty || cell.key == key)
  // while (cell.mark != empty && cell.key != key)
  while (obj->table[index].mark != HT_EMPTY &&
         obj->equal(key, obj->table[index].key) != 0) {
    // This is quadratic probing, but I'm avoiding squaring numbers:
    // j:     1, 3, 5, 7,  9, 11, ..
    // index: 0, 1, 4, 9, 16, 25, 36
    index = (index + j) % obj->allocated;
    j += 2;
  }

  return index;
}

/**
   @brief Expand the hash table, adding increment to the capacity of the table.

   @param table The table to expand.
 */
void ht_resize(smb_ht *table)
{
  smb_ht_bckt *old_table;
  unsigned int index, old_allocated;

  // Step one: allocate new space for the table
  old_table = table->table;
  old_allocated = table->allocated;
  table->length = 0;
  table->allocated = ht_next_size(old_allocated);
  table->table = smb_new(smb_ht_bckt, table->allocated);

  // Zero out the new block too.
  memset((void*)table->table, 0, table->allocated * sizeof(smb_ht_bckt));

  // Step two, add the old items to the new table.
  for (index = 0; index < old_allocated; index++) {
    if (old_table[index].mark == HT_FULL) {
      ht_insert(table, old_table[index].key, old_table[index].value);
    }
  }

  // Step three: free old data.
  smb_free(old_table);
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
  table->table = smb_new(smb_ht_bckt, HASH_TABLE_INITIAL_SIZE);

  // Zero out the entries in the table so we don't get segmentation faults.
  memset((void*)table->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(smb_ht_bckt));
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
  unsigned int i;

  // Do the action on all the items in the table, if provided.
  if (deleter) {
    for (i = 0; i < table->allocated; i++) {
      if (table->table[i].mark == HT_FULL) {
        deleter(table->table[i].value);
      }
    }
  }

  // Delete the table.
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
  unsigned int index;
  if (ht_load_factor(table) > HASH_TABLE_MAX_LOAD_FACTOR) {
    ht_resize(table);
  }

  // First, probe for the key as if we're trying to return it.  If we find it,
  // we update the existing key.
  index = ht_find_retrieve(table, key);
  if (table->table[index].mark == HT_FULL) {
    table->table[index].value = value;
    return;
  }

  // If we don't find the key, then we find the first open slot or gravestone.
  index = ht_find_insert(table, key);
  table->table[index].key = key;
  table->table[index].value = value;
  table->table[index].mark = HT_FULL;
  table->length++;
}

void ht_remove_act(smb_ht *table, DATA key, DATA_ACTION deleter,
                   smb_status *status)
{
  *status = SMB_SUCCESS;
  unsigned int index = ht_find_retrieve(table, key);

  // If the returned slot isn't full, that means we couldn't find it.
  if (table->table[index].mark != HT_FULL) {
    *status = SMB_NOT_FOUND_ERROR;
    return;
  }

  // Perform the action if there is one.
  if (deleter) {
    deleter(table->table[index].value);
  }

  // Mark the slot with a "grave stone", indicating it is deleted.
  table->table[index].mark = HT_GRAVE;
  table->length--;
}

void ht_remove(smb_ht *table, DATA key, smb_status *status)
{
  ht_remove_act(table, key, NULL, status);
}

DATA ht_get(smb_ht const *table, DATA key, smb_status *status)
{
  *status = SMB_SUCCESS;
  unsigned int index = ht_find_retrieve(table, key);

  // If the slot is not marked full, we didn't find the key.
  if (table->table[index].mark != HT_FULL) {
    *status = SMB_NOT_FOUND_ERROR;
    return PTR(NULL);
  }

  // Otherwise, return the key.
  return table->table[index].value;
}

bool ht_contains(smb_ht const *table, DATA key)
{
  smb_status status = SMB_SUCCESS;
  ht_get(table, key, &status);
  return status == SMB_SUCCESS;
}

DATA ht_iter_next(smb_iter *iter, smb_status *status)
{
  *status = SMB_SUCCESS;
  long long int i = iter->state.data_llint + 1;
  const smb_ht *ht = iter->ds;

  // Move up until we either run off the table, or find a bucket that contains
  // something.
  while (i < ht->allocated && ht->table[i].mark != HT_FULL) {
    i++;
  }

  // save current index back to data structure
  iter->state.data_llint = i;

  // If we hit the end of the table, stop iterating.
  if (i >= ht->allocated) {
    *status = SMB_STOP_ITERATION;
    return LLINT(0);
  } else {
    // Otherwise, return the key we found.
    iter->index++; // count the number of items we've found so far
    return ht->table[i].key;
  }
}

bool ht_iter_has_next(smb_iter *iter)
{
  const smb_ht *ht = iter->ds;
  return iter->index < (int)ht->allocated;
}

void ht_iter_destroy(smb_iter *iter)
{
  (void)iter; //unused
}

void ht_iter_delete(smb_iter *iter)
{
  ht_iter_destroy(iter);
  free(iter);
}

smb_iter ht_get_iter(const smb_ht *ht)
{
  smb_iter iter = {
    // Data:
    .ds = ht,          // A reference to the data structure.
    .state = LLINT(0), // This tracks the actual index into the table.
    .index = 0,        // This tracks how many keys have been returned.

    // Functions
    .next = &ht_iter_next,
    .has_next = &ht_iter_has_next,
    .destroy = &ht_iter_destroy,
    .delete = &ht_iter_delete
  };
  return iter;
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
  unsigned int i;
  char *MARKS[] = {"EMPTY", " FULL", "GRAVE"};

  for (i = 0; i < table->allocated; i++) {
    if (full_mode || table->table[i].mark == HT_FULL) {
      printf("[%04d|%s]: key=0x%llx, value=0x%llx\n", i,
             MARKS[table->table[i].mark], table->table[i].key.data_llint,
             table->table[i].value.data_llint);
    }
  }
}
