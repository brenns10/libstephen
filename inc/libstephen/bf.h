/***************************************************************************//**

  @file         bf.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Bitfield

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

void bf_init(unsigned char *data, int num_bools, smb_status *status);
unsigned char *bf_create(int num_bools, smb_status *status);
void bf_delete(unsigned char *data, int num_bools);
int bf_check(unsigned char *data, int index);
void bf_set(unsigned char *data, int index);
void bf_clear(unsigned char *data, int index);
void bf_flip(unsigned char *data, int index);

#endif // LIBSTEPHEN_BF_H
