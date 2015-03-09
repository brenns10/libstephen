/***************************************************************************//**

  @file         bf.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Bitfield

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

void bf_init(unsigned char *data, int num_bools);
unsigned char *bf_create(int num_bools);
void bf_delete(unsigned char *data, int num_bools);
int bf_check(unsigned char *data, int index);
void bf_set(unsigned char *data, int index);
void bf_clear(unsigned char *data, int index);
void bf_flip(unsigned char *data, int index);

#endif // LIBSTEPHEN_BF_H
