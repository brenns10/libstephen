/***************************************************************************//**

  @file         libstephen/cb.h

  @author       Stephen Brennan

  @date         Monday, 8 August 2016

  @brief        Ring buffer data structure, for nice dequeues.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_RB_H
#define LIBSTEPHEN_RB_H

/**
   A ring buffer data structure. This buffer can be inserted into and removed
   from at either end in constant time, except for memory allocations which may
   have to occur to expand the buffer. However these always double the buffer
   size, which means the number of allocations is logarithmic with respect to
   the number of insertions.
 */
typedef struct {

  void *data;
  int dsize;

  int nalloc;
  int start;
  int count;

} smb_rb;

/**
   @brief Initialize a ring buffer.
   @param rb Pointer to a ring buffer struct.
   @param dsize Size of data type to store in ring buffer.
   @param init Initial amount of space to allocate.
 */
void rb_init(smb_rb *rb, int dsize, int init);
/**
   @brief Free all resources held by the ring buffer.
   @param rb Pointer to the ring buffer struct.
 */
void rb_destroy(smb_rb *rb);
/**
   @brief Add an item to the front of the ring buffer.  May trigger expansion.
   @param src Area of memory to read from.
 */
void rb_push_front(smb_rb *rb, void *src);
/**
   @brief Remove an item from the front of the ring buffer.
   @param dst Area of memory to write resulting data to.

   Note that behavior is unbefined if you decide to pop from an empty buffer.
 */
void rb_pop_front(smb_rb *rb, void *dst);
/**
   @brief Add an item to the end of the ring buffer.  May trigger expansion.
   @param src Area of memory to read from.
*/
void rb_push_back(smb_rb *rb, void *src);
/**
   @brief Remove an item from the end of the ring buffer.
   @param dst Area of memory to write resulting data to.

   Note that behavior is undefined if you decide to pop from an empty buffer.
*/
void rb_pop_back(smb_rb *rb, void *dst);

/**
   @brief Expand a ring buffer (by doubling its size).
   @param rb Pointer to ring buffer.

   Note that this is mostly an internal function, and is exposed in the header
   for testing purposes. No guarantee is made that its interface will stay the
   same, or that it will continue to exist.
 */
void rb_grow(smb_rb *rb);

#endif //LIBSTEPHEN_RB_H
