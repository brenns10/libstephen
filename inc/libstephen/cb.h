/***************************************************************************//**

  @file         cb.h

  @author       Stephen Brennan

  @date         Saturday, 23 May 2015

  @brief        Character buffer declarations.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_CB_H
#define LIBSTEPHEN_CB_H

#include <stdarg.h>
#include <wchar.h>

/**
   @brief A character buffer utility that is easier to handle than a char*.

   This character buffer provides an interface for string processing that allows
   you to be efficient, while not needing to handle nearly as much of the
   allocations that are necessary.  It automatically expands as you add to it.
 */
typedef struct {
  /**
     @brief Buffer pointer.
   */
  char *buf;
  /**
     @brief Allocated size of the buffer.
   */
  int capacity;
  /**
     @brief Length of the string in the buffer.
   */
  int length;
} cbuf;

/**
   @brief Initialize a brand-new character buffer.

   A buffer of the given capacity is initialized, and given an empty string
   value.
   @param obj The cbuf to initialize.
   @param capacity Initial capacity of the buffer.
 */
void cb_init(cbuf *obj, int capacity);
/**
   @brief Allocate and initialize a brand-new character buffer.

   A buffer of the given capacity is initialized, and given an empty string
   value.  The cbuf struct is also allocated and the pointer returned.
   @param capacity Initial capacity of the buffer.
 */
cbuf *cb_create(int capacity);
/**
   @brief Deallocate the contents of the string buffer.
   @param obj The character buffer to deallocate.
 */
void cb_destroy(cbuf *obj);
/**
   @brief Deallocate the contents of, and delete the string buffer.
   @param obj The character buffer to delete.
 */
void cb_delete(cbuf *obj);

/**
   @brief Append a string onto the end of the character buffer.
   @param obj The buffer to append onto.
   @param str The string to append.
 */
void cb_append(cbuf *obj, char *str);
/**
   @brief Format and print a string onto the end of a character buffer.
   @param obj The object to print onto.
   @param format The format string to print.
   @param ... The arguments to the format string.
 */
void cb_printf(cbuf *obj, char *format, ...);
/**
   @brief Format and print a string onto the cbuf using a va_list.
   @param obj The cbuf to print into.
   @param format The format string to print.
   @param va_list The vararg list.
 */
void cb_vprintf(cbuf *obj, char *format, va_list va);


/**
   @brief A wide character buffer.  Similar to cbuf.

   Again, this buffer provides an interface for simpler string manipulation, by
   managing the buffer reallocations for you.  Of course, the difference being
   that this one is for wide characters.
 */
typedef struct {
  /**
     @brief Buffer pointer.
   */
  wchar_t *buf;
  /**
     @brief Actual capacity of the buffer.
   */
  int capacity;
  /**
     @brief Length of the string in the buffer.
   */
  int length;
} wcbuf;

/**
   @brief Initialize a string buffer to a given capacity.
   @param obj The string buffer to initialize.
   @param capacity The initial capacity to give it.
 */
void wcb_init(wcbuf *obj, int capacity);
/**
   @brief Initialize a string buffer and allocate the struct as well.
   @param capacity The capacity of the string buffer to create.
   @return A pointer to the buffer.
 */
wcbuf *wcb_create(int capacity);
/**
   @brief Destroy the wide buffer contained in the wcbuf struct.
   @param obj The wide buffer to destroy.
 */
void wcb_destroy(wcbuf *obj);
/**
   @brief Destroy and delete the wide buffer and struct.
   @param obj The wide buffer struct to destroy.
 */
void wcb_delete(wcbuf *obj);

/**
   @brief Append a wide character string into the wide buffer.
   @param obj The wide buffer to append onto.
   @param str The wide string to append on.
 */
void wcb_append(wcbuf *obj, wchar_t *str);
/**
   @brief Printf a wide character string onto the wide buffer.
   @param obj The wide buffer to printf onto.
   @param format Format string to print.
   @param ... The arguments to to put into the format string.
 */
void wcb_printf(wcbuf *obj, wchar_t *format, ...);
/**
   @brief Format and print a string onto the wcbuf using a va_list.
   @param obj The wcbuf to print into.
   @param format The format string to print.
   @param va_list The vararg list.
 */
void wcb_vprintf(wcbuf *obj, wchar_t *format, va_list va);

#endif //LIBSTEPHEN_CB_H
