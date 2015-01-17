/***************************************************************************//**

  @file         ht.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Hash Table

  @copyright    Copyright (c) 2014, Stephen Brennan.
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

#ifndef LIBSTEPHEN_HT_H
#define LIBSTEPHEN_HT_H

#include "base.h"  /* DATA, DATA_ACTION */

/**
   @brief An initial amount of rows in the hash table.  Something just off of a
   power of 2.
 */
#define HASH_TABLE_INITIAL_SIZE 257

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
     @brief The next item in the linked list.
   */
  struct smb_ht_bckt *next;

} smb_ht_bckt;

/**
   @brief A hash table data structure.
 */
typedef struct smb_ht
{
  /**
     @brief The number of items in the hash table.
   */
  int length;

  /**
     @brief The number of slots allocated in the hash table.
   */
  int allocated;

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
  struct smb_ht_bckt **table;

} smb_ht;

void ht_init(smb_ht *pTable, HASH_FUNCTION hash_func, DATA_COMPARE equal);
smb_ht *ht_create(HASH_FUNCTION hash_func, DATA_COMPARE equal);
void ht_destroy_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_destroy(smb_ht *pTable);
void ht_delete_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_delete(smb_ht *pTable);

void ht_insert(smb_ht *pTable, DATA dKey, DATA dValue);
void ht_remove_act(smb_ht *pTable, DATA dKey, DATA_ACTION deleter,
                   smb_status *status);
void ht_remove(smb_ht *pTable, DATA dKey, smb_status *status);
DATA ht_get(smb_ht const *pTable, DATA dKey, smb_status *status);
unsigned int ht_string_hash(DATA data);
void ht_print(smb_ht const *pTable, int full_mode);

#endif // LIBSTEPHEN_HT_H
