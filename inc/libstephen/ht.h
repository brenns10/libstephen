/***************************************************************************//**

  @file         libstephen/ht.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        A simple hash table and string hash function.

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_HT_H
#define LIBSTEPHEN_HT_H

#include "base.h"  /* DATA, DATA_ACTION */
#include "list.h"  /* DATA, DATA_ACTION */

/**
   @brief An initial amount of rows in the hash table.  Something just off of a
   power of 2.
 */
#define HASH_TABLE_INITIAL_SIZE 31

/**
   @brief The maximum load factor that can be allowed in the hash table.
 */
#define HASH_TABLE_MAX_LOAD_FACTOR 0.7

/**
   @brief A hash function declaration.

   @param toHash The data that will be passed to the hash function.
   @returns The hash value
 */
typedef unsigned int (*HASH_FUNCTION)(DATA toHash);

/**
   @brief This mark goes in each table cell.

   In order for probing to work properly, you need to be able to tell whether or
   not a cell is empty because something was deleted out of it, or whether it
   has always been empty.  So, the "GRAVE" mark is a grave stone.  When you
   delete a key from a cell, you mark it with a grave stone.  And when you're
   searching for a key, you continue when you find a grave marker.
 */
typedef enum smb_ht_mark {
  HT_EMPTY=0, HT_FULL, HT_GRAVE
} smb_ht_mark;

/**
   @brief The bucket of a hash table.  Contains key,value pairs, and is a singly
   linked list.
 */
typedef struct smb_ht_bckt
{
  /**
     @brief The key of this entry.
   */
  DATA key;

  /**
     @brief The value of this entry.
   */
  DATA value;

  /**
     @brief Marker for whether or not the table is full or empty.
   */
  enum smb_ht_mark mark;

} smb_ht_bckt;

/**
   @brief A hash table data structure.
 */
typedef struct smb_ht
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
     @brief The hash function for this hash table.
   */
  HASH_FUNCTION hash;

  /**
     @brief Function to use to compare equality.
   */
  DATA_COMPARE equal;

  /**
     @brief Pointer to the data of the table.
   */
  struct smb_ht_bckt *table;

} smb_ht;

/**
   @brief Initialize a hash table in memory already allocated.
   @param table A pointer to the table to initialize.
   @param hash_func A hash function for the table.
   @param equal A comparison function for DATA.
 */
void ht_init(smb_ht *table, HASH_FUNCTION hash_func, DATA_COMPARE equal);
/**
   @brief Allocate and initialize a hash table.
   @param hash_func A function that takes one DATA and returns a hash value
   generated from it.  It should be a good hash function.
   @param equal A comparison function for DATA.
   @returns A pointer to the new hash table.
 */
smb_ht *ht_create(HASH_FUNCTION hash_func, DATA_COMPARE equal);
/**
   @brief Free resources used by the hash table, but does not free the pointer
   itself.  Perform an action on the data as it is deleted.

   Useful for stack valued hash tables.  A deleter must be specified in this
   function call.
   @param table The table to destroy.
   @param deleter The deletion action on the data.
 */
void ht_destroy_act(smb_ht *table, DATA_ACTION deleter);
/**
   @brief Free any resources used by the hash table, but doesn't free the
   pointer.  Doesn't perform any actions on the data as it is deleted.

   If pointers are contained within the hash table, they are not freed.  Use
   ht_destroy_act() to specify a deletion action on the hash table.
   @param table The table to destroy.
 */
void ht_destroy(smb_ht *table);
/**
   @brief Free the hash table and its resources.  Perform an action on each data
   before freeing the table.  Useful for freeing pointers stored in the table.
   @param table The table to free.
   @param deleter The action to perform on each value in the hash table before
   deletion.
 */
void ht_delete_act(smb_ht *table, DATA_ACTION deleter);
/**
   @brief Free the hash table and its resources.  No pointers contained in the
   table will be freed.
   @param table The table to free.
 */
void ht_delete(smb_ht *table);

/**
   @brief Insert data into the hash table.

   Expands the hash table if the load factor is below a threshold.  If the key
   already exists in the table, then the function will overwrite it with the new
   data provided.
   @param table A pointer to the hash table.
   @param key The key to insert.
   @param value The value to insert at the key.
 */
void ht_insert(smb_ht *table, DATA key, DATA value);
/**
   @brief Remove the key, value pair stored in the hash table.
   @param table A pointer to the hash table.
   @param key The key to delete.
   @param deleter The action to perform on the value before removing it.
   @param[out] status Status variable.
   @exception SMB_NOT_FOUND_ERROR If an item with the given key is not found.
 */
void ht_remove_act(smb_ht *table, DATA key, DATA_ACTION deleter,
                   smb_status *status);
/**
   @brief Remove the key, value pair stored in the hash table.

   This function does not call a deleter on the stored data.
   @param table A pointer to the hash table.
   @param key The key to delete.
   @param[out] status Status variable.
   @exception SMB_NOT_FOUND_ERROR If an item with the given key is not found.
 */
void ht_remove(smb_ht *table, DATA key, smb_status *status);
/**
   @brief Return the value associated with the key provided.
   @param table A pointer to the hash table.
   @param key The key whose value to retrieve.
   @param[out] status Status variable.
   @returns The value associated the key.
   @exception NOT_FOUND_ERROR
 */
DATA ht_get(smb_ht const *table, DATA key, smb_status *status);
/**
   @brief Return true when a key is contained in the table.
   @param table A pointer to the hash table.
   @param key The key to search for.
   @returns Whether the key is present.
 */
bool ht_contains(smb_ht const *table, DATA key);
/**
   @brief Return an iterator over the keys of the table.
   @param ht A pointer to the hash table.
   @returns An iterator struct.
 */
smb_iter ht_get_iter(const smb_ht *ht);
/**
   @brief Return the hash of the data, interpreting it as a string.
   @param data The string to hash, assuming that the value contained is a char*.
   @returns The hash value of the string.
 */
unsigned int ht_string_hash(DATA data);
/**
   @brief Print the entire hash table.

   This function is useful for diagnostics.  It can show every row in the table
   (with full_mode) so you can see how well entries are distributed in the
   table.  Or, it can be compact and show just the rows with data.
   @param table The table to print.
   @param full_mode Whether to print every row in the hash table.
 */
void ht_print(smb_ht const *table, int full_mode);

#endif // LIBSTEPHEN_HT_H
