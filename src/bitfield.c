/***************************************************************************//**

  @file         bitfield.c

  @author       Stephen Brennan

  @date         Created Wednesday, 29 January 2014

  @brief        Implementation of "libstephen/bf.h".

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <string.h>
#include <assert.h>

#include "libstephen/base.h"
#include "libstephen/bf.h"

void bf_init(unsigned char *data, int num_bools) {
  int size = SMB_BITFIELD_SIZE(num_bools);
  memset(data, 0, size);
}

unsigned char *bf_create(int num_bools) {
  unsigned char *data;
  int size = SMB_BITFIELD_SIZE(num_bools);

  data = smb_new(unsigned char, size);

  bf_init(data, num_bools);
  return data;
}

void bf_delete(unsigned char *data, int num_bools) {
  smb_free(data);
}

int bf_check(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  return data[byte_index] & bit_mask;
}

void bf_set(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  data[byte_index] |= bit_mask;
}

void bf_clear(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = ~(0x01 << (index % BIT_PER_CHAR));

  data[byte_index] &= bit_mask;
}

void bf_flip(unsigned char *data, int index) {
  int byte_index = (int)(index / BIT_PER_CHAR);
  unsigned char bit_mask = 0x01 << (index % BIT_PER_CHAR);

  unsigned char value = ~(data[byte_index] & bit_mask) & bit_mask;
  data[byte_index] = (data[byte_index] & ~bit_mask) | value;
}
