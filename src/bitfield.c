/***************************************************************************//**

  @file         bitfield.c

  @author       Stephen Brennan

  @date         Created Wednesday, 29 January 2014

  @brief        A tiny little bit of code that allows for efficient storage and
                access to large amounts of boolean data.

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

#include "libstephen.h"
#include <string.h>

/**
   @brief Initialize the memory where a bitfield is contained to all 0's.  

   This is public so people can use the function to allocate their own bitfields
   on function stacks instead of via the heap.

   @param data A pointer to the bitfield.
   @param num_bools The size of the bitfield, in number of bools (aka bits, not
     bytes).
 */
void bf_init(unsigned char *data, int num_bools) {
  int size = SMB_BITFIELD_SIZE(num_bools);
  memset(data, 0, size);
}

/**
   @brief Allocate a bitfield capable of holding the given number of bools.

   This will be allocated in dynamic memory, and a pointer will be returned.
   The bf_init function is available for creating bitfields in arbitrary
   locations.

   @param num_bools The number of bools to fit in the bit field.
   @returns A pointer to the bitfield.
   @exception ALLOCATION_ERROR
 */
unsigned char *bf_create(int num_bools) {
  unsigned char *data;
  int size = SMB_BITFIELD_SIZE(num_bools);
  
  CLEAR_ALL_ERRORS;
  data = (unsigned char*) malloc(size * sizeof(unsigned char));
  
  if (!data) {
    RAISE(ALLOCATION_ERROR);
    return NULL;
  }

  SMB_INCREMENT_MALLOC_COUNTER(size);
  bf_init(data, num_bools);
  return data;
}

/**
   @brief Delete the bitfield pointed to.  Only do this if you created the
   bitfield via bf_create().

   @param data A pointer to the bitfield.
   @param num_bools The number of bools contained in the bitfield.
 */
void bf_delete(unsigned char *data, int num_bools) {
  int size = SMB_BITFIELD_SIZE(num_bools);
  free(data);
  SMB_DECREMENT_MALLOC_COUNTER(size);
}

/**
   @brief Check whether the given bit is set.

   @param data A pointer to the bitfield.
   @param index The index of the bit to Check
   @retval 0 if the bit is not set.
   @retval Nonzero if the bit is set.
 */
int bf_check(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  return data[byte_index] & bit_mask;
}

/**
   @brief Set a bit.

   @param data A pointer to the bitfield
   @param index The index of the bit to set.
 */
void bf_set(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  data[byte_index] |= bit_mask;
}

/**
   @brief Clear a bit.

   @param data A pointer to the bitfield.
   @param index The index of the bit to clear.
 */
void bf_clear(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = ~(0x01 << (index % BIT_PER_CHAR));

  data[byte_index] &= bit_mask;
}

/**
   @brief Clear a bit.

   @param data A pointer to the bitfield.
   @param index The index of the bit to flip.
 */
void bf_flip(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  unsigned char value = ~(data[byte_index] & bit_mask) & bit_mask;
  data[byte_index] = (data[byte_index] & ~bit_mask) | value;
}
