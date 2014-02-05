/*******************************************************************************

  File:         bitfield.c

  Author:       Stephen Brennan

  Date Created: Wednesday, 29 January 2014

  Description:  A tiny little bit of code that allows for efficent storage and
                access to large amounts of boolean data.

*******************************************************************************/

#include "libstephen.h"
#include <string.h>

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

void bf_delete(unsigned char *data, int num_bools) {
  int size = SMB_BITFIELD_SIZE(num_bools);
  free(data);
  SMB_DECREMENT_MALLOC_COUNTER(size);
}

void bf_init(unsigned char *data, int num_bools) {
  int size = SMB_BITFIELD_SIZE(num_bools);
  memset(data, 0, size);
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
