/***************************************************************************//**

  @file         libstephen/bf.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        A tiny little bit of code that allows for efficient storage and
                access to large amounts of boolean data.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_BF_H
#define LIBSTEPHEN_BF_H

#include "base.h"

/**
   @brief Number of bits in a char type.  Shouldn't really change...
 */
#define BIT_PER_CHAR 8

/**
   @brief Get the number amount of space eneded for a bitfield of the specified
   amount of booleans.

   If you want to allocate a buffer on the stack, you need this macro.

   @param num_bools The number of booleans.
   @returns The number of bytes
 */
#define SMB_BITFIELD_SIZE(num_bools) ((int)((num_bools) / BIT_PER_CHAR) + \
                                      ((num_bools) % BIT_PER_CHAR == 0 ? 0 : 1))

/**
   @brief Initialize the memory where a bitfield is contained to all 0's.

   This is public so people can use the function to allocate their own bitfields
   on function stacks instead of via the heap.  No errors are defined for this
   function.
   @param data A pointer to the bitfield.
   @param num_bools The size of the bitfield, in number of bools (aka bits, not
     bytes).
 */
void bf_init(unsigned char *data, int num_bools);
/**
   @brief Allocate a bitfield capable of holding the given number of bools.

   This will be allocated in dynamic memory, and a pointer will be returned.
   The bf_init function is available for creating bitfields in arbitrary
   locations.
   @param num_bools The number of bools to fit in the bit field.
   @returns A pointer to the bitfield.
 */
unsigned char *bf_create(int num_bools);
/**
   @brief Delete the bitfield pointed to.  Only do this if you created the
   bitfield via bf_create().
   @param data A pointer to the bitfield.
   @param num_bools The number of bools contained in the bitfield.
 */
void bf_delete(unsigned char *data, int num_bools);
/**
   @brief Check whether the given bit is set.
   @param data A pointer to the bitfield.
   @param index The index of the bit to Check
   @retval 0 if the bit is not set.
   @retval Nonzero if the bit is set.
 */
int bf_check(unsigned char *data, int index);
/**
   @brief Set a bit.
   @param data A pointer to the bitfield
   @param index The index of the bit to set.
 */
void bf_set(unsigned char *data, int index);
/**
   @brief Clear a bit.
   @param data A pointer to the bitfield.
   @param index The index of the bit to clear.
 */
void bf_clear(unsigned char *data, int index);
/**
   @brief Clear a bit.
   @param data A pointer to the bitfield.
   @param index The index of the bit to flip.
 */
void bf_flip(unsigned char *data, int index);

#endif // LIBSTEPHEN_BF_H
