
/***************************************************************************//**

  @file         libstephen/hta.h

  @author       Stephen Brennan

  @date         Created Tuesday,  20 September 2016

  @brief        Hash Table for Any data

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_HTA_H
#define LIBSTEPHEN_HTA_H

#include "base.h"

#define HTA_KEY_OFFSET 1

#define HTA_MARK(t, i) ((int8_t*)t->table)[i]

/**
   @brief A hash function declaration.

   @param to_hash The data that will be passed to the hash function.
   @returns The hash value
 */
typedef unsigned int (*HTA_HASH)(void *to_hash);

/**
   @brief A comparator function declaration.

   @param left Left side of comparison.
   @param right Right side of comparison.
   @returns < 0 if left < right, 0 if left == right, > 0 if left > right
 */
typedef int (*HTA_COMP)(void *left, void *right);

/**
   @brief A print function declaration.

   @param f File to print to.
   @param obj Object to print.
*/
typedef void (*HTA_PRINT)(FILE *f, void *obj);

/**
   @brief A hash table data structure.
 */
typedef struct smb_hta
{
  /**
     @brief The number of items in the hash table.
   */
  unsigned int length;

  /**
     @brief The number of slots allocated in the hash table.
   */
  unsigned int allocated;

  /**
     @brief Size of keys
   */
  unsigned int key_size;

  /**
     @brief Size of values
   */
  unsigned int value_size;

  /**
     @brief The hash function for this hash table.
   */
  HTA_HASH hash;

  /**
     @brief Function to use to compare equality.
   */
  HTA_COMP equal;

  /**
     @brief Pointer to the data of the table.
   */
  void *table;

} smb_hta;

/**
   @brief Initialize a hash table in memory already allocated.
   @param table A pointer to the table to initialize.
   @param hash_func A hash function for the table.
   @param equal A comparison function for DATA.
   @param key_size Size of keys.
   @param value_size Size of values.
 */
void hta_init(smb_hta *table, HTA_HASH hash_func, HTA_COMP equal,
              unsigned int key_size, unsigned int value_size);
/**
   @brief Allocate and initialize a hash table.
   @param hash_func A function that takes one DATA and returns a hash value
   generated from it.  It should be a good hash function.
   @param equal A comparison function for DATA.
   @param key_size Size of keys.
   @param value_size Size of values.
   @returns A pointer to the new hash table.
 */
smb_hta *hta_create(HTA_HASH hash_func, HTA_COMP equal,
                    unsigned int key_size, unsigned int value_size);
/**
   @brief Free any resources used by the hash table, but doesn't free the
   pointer.  Doesn't perform any actions on the data as it is deleted.

   If pointers are contained within the hash table, they are not freed. Use to
   specify a deletion action on the hash table.
   @param table The table to destroy.
 */
void hta_destroy(smb_hta *table);
/**
   @brief Free the hash table and its resources.  No pointers contained in the
   table will be freed.
   @param table The table to free.
 */
void hta_delete(smb_hta *table);

/**
   @brief Insert data into the hash table.

   Expands the hash table if the load factor is below a threshold.  If the key
   already exists in the table, then the function will overwrite it with the new
   data provided.
   @param table A pointer to the hash table.
   @param key The key to insert.
   @param value The value to insert at the key.
 */
void hta_insert(smb_hta *table, void *key, void *value);
/**
   @brief Remove the key, value pair stored in the hash table.

   This function does not call a deleter on the stored data.
   @param table A pointer to the hash table.
   @param key The key to delete.
   @param[out] status Status variable.
   @exception SMB_NOT_FOUND_ERROR If an item with the given key is not found.
 */
void hta_remove(smb_hta *table, void *key, smb_status *status);
/**
   @brief Return the value associated with the key provided.
   @param table A pointer to the hash table.
   @param key The key whose value to retrieve.
   @param[out] status Status variable.
   @returns The value associated the key.
   @exception NOT_FOUND_ERROR
 */
void *hta_get(smb_hta const *table, void *key, smb_status *status);
/**
   @brief Return true when a key is contained in the table.
   @param table A pointer to the hash table.
   @param key The key to search for.
   @returns Whether the key is present.
 */
bool hta_contains(smb_hta const *table, void *key);
/**
   @brief Return the hash of the data, interpreting it as a string.
   @param data The string to hash, assuming that the value contained is a char*.
   @returns The hash value of the string.
 */
unsigned int hta_string_hash(void *data);
int hta_string_comp(void *left, void *right);
int hta_int_comp(void *left, void *right);
/**
   @brief Print the entire hash table.

   This function is useful for diagnostics.  It can show every row in the table
   (with full_mode) so you can see how well entries are distributed in the
   table.  Or, it can be compact and show just the rows with data.
   @param table The table to print.
   @param full_mode Whether to print every row in the hash table.
 */
void hta_print(FILE *f, smb_hta const *table, HTA_PRINT key, HTA_PRINT value,
               int full_mode);

#endif // LIBSTEPHEN_HTA_H
