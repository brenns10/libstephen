/***************************************************************************//**

  @file         hta.c

  @author       Stephen Brennan

  @date         Created Tuesday, 20 September 2016

  @brief        Hash Table for Any data

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

  This is based on my original hashtable.c implementation, but aims to address
  the observation that using DATA is inflexible compared to simply storing
  blocks of memory.

*******************************************************************************/

#include <string.h>
#include <stdio.h>

#include "libstephen/ht.h"
#include "libstephen/hta.h"

/*******************************************************************************

                               Private Functions

*******************************************************************************/

unsigned int item_size(const smb_hta *obj)
{
  return HTA_KEY_OFFSET + obj->key_size + obj->value_size;
}

unsigned int convert_idx(const smb_hta *obj, unsigned int orig)
{
  return orig * item_size(obj);
}

/**
   @brief Find the proper index for insertion into the table.
   @param obj Hash table object.
   @param key Key we're inserting.
 */
unsigned int hta_find_insert(const smb_hta *obj, void *key)
{
  unsigned int index = obj->hash(key) % obj->allocated;
  unsigned int bufidx = convert_idx(obj, index);
  unsigned int j = 1;

  // Continue searching until we either find a non-full slot, or we find the key
  // we're trying to insert.
  // until (cell.mark != full || cell.key == key)
  // while (cell.mark == full && cell.key != key)
  while (HTA_MARK(obj, bufidx) == HT_FULL &&
         obj->equal(key, obj->table + bufidx + HTA_KEY_OFFSET) != 0) {
    // This is quadratic probing, but I'm avoiding squaring numbers:
    // j:     1, 3, 5, 7,  9, 11, ..
    // index: 0, 1, 4, 9, 16, 25, 36
    index = (index + j) % obj->allocated;
    j += 2;
    bufidx = convert_idx(obj, index);
  }

  return index;
}

/**
   @brief Find the proper index for retrieval from the table.
   @param obj Hash table object.
   @param key Key we're looking up.
 */
unsigned int hta_find_retrieve(const smb_hta *obj, void *key)
{
  unsigned int index = obj->hash(key) % obj->allocated;
  unsigned int bufidx = convert_idx(obj, index);
  unsigned int j = 1;

  // Continue searching until we either find an empty slot, or we find the key
  // we're trying to insert.
  // until (cell.mark == empty || cell.key == key)
  // while (cell.mark != empty && cell.key != key)
  while (HTA_MARK(obj, bufidx) != HT_EMPTY &&
         obj->equal(key, obj->table + bufidx + HTA_KEY_OFFSET) != 0) {
    // This is quadratic probing, but I'm avoiding squaring numbers:
    // j:     1, 3, 5, 7,  9, 11, ..
    // index: 0, 1, 4, 9, 16, 25, 36
    index = (index + j) % obj->allocated;
    j += 2;
    bufidx = convert_idx(obj, index);
  }

  return index;
}

/**
   @brief Expand the hash table, adding increment to the capacity of the table.

   @param table The table to expand.
 */
void hta_resize(smb_hta *table)
{
  void *old_table;
  unsigned int index, old_allocated, bufidx;

  // Step one: allocate new space for the table
  old_table = table->table;
  old_allocated = table->allocated;
  table->length = 0;
  table->allocated = ht_next_size(old_allocated);
  table->table = calloc(table->allocated, item_size(table));

  // Step two, add the old items to the new table.
  for (index = 0; index < old_allocated; index++) {
    bufidx = convert_idx(table, index);
    if (((int8_t*)old_table)[bufidx] == HT_FULL) {
      hta_insert(table, old_table + bufidx + HTA_KEY_OFFSET,
                 old_table + bufidx + HTA_KEY_OFFSET + table->value_size);
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
double hta_load_factor(smb_hta *table)
{
  return ((double) table->length) / ((double) table->allocated);
}

/*******************************************************************************

                           Public Interface Functions

*******************************************************************************/

void hta_init(smb_hta *table, HTA_HASH hash_func, HTA_COMP equal,
              unsigned int key_size, unsigned int value_size)
{
  // Initialize values
  table->length = 0;
  table->allocated = HASH_TABLE_INITIAL_SIZE;
  table->key_size = key_size;
  table->value_size = value_size;
  table->hash = hash_func;
  table->equal = equal;

  // Allocate table
  table->table = calloc(HASH_TABLE_INITIAL_SIZE, item_size(table));
}

smb_hta *hta_create(HTA_HASH hash_func, HTA_COMP equal,
                    unsigned int key_size, unsigned int value_size)
{
  // Allocate and create the table.
  smb_hta *table;
  table = smb_new(smb_hta, 1);
  hta_init(table, hash_func, equal, key_size, value_size);
  return table;
}

void hta_destroy(smb_hta *table)
{
  smb_free(table->table);
}

void hta_delete(smb_hta *table)
{
  if (!table) {
    return;
  }

  hta_destroy(table);
  smb_free(table);
}

void hta_insert(smb_hta *table, void *key, void *value)
{
  unsigned int index, bufidx;
  if (hta_load_factor(table) > HASH_TABLE_MAX_LOAD_FACTOR) {
    hta_resize(table);
  }

  // First, probe for the key as if we're trying to return it.  If we find it,
  // we update the existing key.
  index = hta_find_retrieve(table, key);
  bufidx = convert_idx(table, index);
  if (HTA_MARK(table, bufidx) == HT_FULL) {
    memcpy(table->table + bufidx + HTA_KEY_OFFSET + table->key_size, value,
           table->value_size);
    return;
  }

  // If we don't find the key, then we find the first open slot or gravestone.
  index = hta_find_insert(table, key);
  bufidx = convert_idx(table, index);
  HTA_MARK(table, bufidx) = HT_FULL;
  memcpy(table->table + bufidx + HTA_KEY_OFFSET, key, table->key_size);
  memcpy(table->table + bufidx + HTA_KEY_OFFSET + table->key_size, value,
         table->value_size);
  table->length++;
}

void hta_remove(smb_hta *table, void *key, smb_status *status)
{
  *status = SMB_SUCCESS;
  unsigned int index = hta_find_retrieve(table, key);
  unsigned int bufidx = convert_idx(table, index);

  // If the returned slot isn't full, that means we couldn't find it.
  if (HTA_MARK(table, bufidx) != HT_FULL) {
    *status = SMB_NOT_FOUND_ERROR;
    return;
  }

  // Mark the slot with a "grave stone", indicating it is deleted.
  HTA_MARK(table, bufidx) = HT_GRAVE;
  table->length--;
}

void *hta_get(smb_hta const *table, void *key, smb_status *status)
{
  *status = SMB_SUCCESS;
  unsigned int index = hta_find_retrieve(table, key);
  unsigned int bufidx = convert_idx(table, index);

  // If the slot is not marked full, we didn't find the key.
  if (HTA_MARK(table, bufidx) != HT_FULL) {
    *status = SMB_NOT_FOUND_ERROR;
    return NULL;
  }

  // Otherwise, return the value.
  return table->table + bufidx + HTA_KEY_OFFSET + table->key_size;
}

bool hta_contains(smb_hta const *table, void *key)
{
  smb_status status = SMB_SUCCESS;
  hta_get(table, key, &status);
  return status == SMB_SUCCESS;
}

unsigned int hta_string_hash(void *data)
{
  char *theString = *(char**)data;
  unsigned int hash = 0;

  while (theString && *theString != '\0' ) {
    hash = (hash << 5) - hash + *theString;
    theString++;
  }

  // printf("Hash of \"%s\": %u\n", (char*)data.data_ptr, hash);
  return hash;
}

int hta_string_comp(void *left, void *right)
{
  char **l = (char**)left, **r = (char**)right;
  return strcmp(*l,*r);
}

int hta_int_comp(void *left, void *right)
{
  int *l = (int*)left, *r = (int*)right;
  return *l - *r;
}

void hta_print(FILE* f, smb_hta const *table, HTA_PRINT key, HTA_PRINT value,
               int full_mode)
{
  unsigned int i, bufidx;
  char *MARKS[] = {"EMPTY", " FULL", "GRAVE"};

  for (i = 0; i < table->allocated; i++) {
    bufidx = convert_idx(table, i);
    int8_t mark = HTA_MARK(table, bufidx);
    if (full_mode || mark == HT_FULL) {
      printf("[%04d|%05d|%s]:\n", i, bufidx, MARKS[mark]);
      if (mark == HT_FULL) {
        printf("  key: ");
        key(f, table->table + bufidx + HTA_KEY_OFFSET);
        printf("\n  value: ");
        value(f, table->table + bufidx + HTA_KEY_OFFSET + table->value_size);
        printf("\n");
      }
    }
  }
}
