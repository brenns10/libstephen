/***************************************************************************//**

  @file         base.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Base Declarations.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_BASE_H
#define LIBSTEPHEN_BASE_H

#include <stdlib.h>  /* size_t  */
#include <stdio.h>   /* fprintf */
#include <stdbool.h> /* bool */


/*******************************************************************************

                                 Base Data Type

*******************************************************************************/

/**
   @brief Base data type for the data structures.

   Capable of containing long integers, double precision floats, or pointers.
   Takes up 8 bytes.
 */
typedef union {
  long long int data_llint; // 64bit integer
  double data_dbl;          // 64bit IEEE double precision float
  void * data_ptr;          // pointer (max 64bit, hopefully)

} LsDATA;

#define LSO(x) ((LsDATA){.data_ptr=(x)})
#define LSI(x) ((LsDATA){.data_llint=(x)})
#define LSD(x) ((LsDATA){.data_dbl=(x)})

/**
   @brief A function pointer that takes two LsDATA and compares them.

   This comparator function is used for two purposes: (1) to check for equality,
   and (2) to order data.  If a particular type of data has no ordering, then it
   is sufficient for the purposes of equality testing to return 0 if equal, and
   1 if not equal.  However, this will fail for any function that uses the
   DATA_COMPARE to order LsDATA.  Therefore, any function that takes a
   DATA_COMPARE should specify in its documentation whether it will use it for
   equality testing, or for ordering.

   The DATA_COMPARE function shall return 0 iff the two LsDATA are equal.  It
   shall return a value less than 0 iff the first is less than the second.  It
   shall return a value greater than zero iff the first is greater than the
   second.
 */
typedef int (*LsProc_Compare)(LsDATA,LsDATA);

int data_compare_string(LsDATA d1, LsDATA d2);
int data_compare_int(LsDATA d1, LsDATA d2);
int data_compare_float(LsDATA d1, LsDATA d2);
int data_compare_pointer(LsDATA d1, LsDATA d2);

/**
   @brief A hash function declaration.

   @param toHash The data that will be passed to the hash function.
   @returns The hash value
 */
typedef unsigned int (*LsProc_Hash)(LsDATA toHash);

/**
   @brief To-string function declaration.

   This function MUST return a free-able string representation of the object
   passed to it.  The caller takes responsibility for freeing the string when it
   is done using it.
   @param toStr Data to convert to a string.
   @returns A freeable string.
 */
typedef char (*LsProc_Str)(LsDATA toStr);

/**
   @brief A function pointer that takes a LsDATA and prints it.

   This function should not print a trailing newline!  There are higher level
   functions that will deal with that.
 */
typedef void (*LsProc_Print)(FILE*, LsDATA);

void data_printer_string(FILE *f, LsDATA d);
void data_printer_int(FILE *f, LsDATA d);
void data_printer_float(FILE *f, LsDATA d);
void data_printer_pointer(FILE *f, LsDATA d);

typedef void (*LsProc_Free)(LsDATA);

typedef smb_iter (*LsProc_Iter)(LsDATA);


/*******************************************************************************

                                  Type System

*******************************************************************************/

typedef void *LsVTable[];

typedef struct {

  char *tp_name;             // name of the type
  LsProc_Free tp_destroy;    // for cleaning up contained items
  LsProc_Compare tp_cmp;     // for comparisons
  LsProc_Hash tp_hash;       // hashing, for hash table and set
  LsProc_Str tp_str;         // to string
  LsProc_Print tp_print;     // print to file
  LsProc_Iter tp_iter;       // return an iterator
  LsVTable tp_seq;           // get, set, delete, append, prepend, etc.
  LsVTable tp_set;           // contains, union, intersection
  LsVTable tp_map;           // get, set, delete

} LsType;

extern LsType LsString;
extern LsType LsInt;
extern LsType LsDouble;
extern LsType LsObject;


/*******************************************************************************

                               Memory Management

*******************************************************************************/

void *smb___new(size_t amt);
void *smb___renew(void *ptr, size_t newsize);
void smb___free(void *ptr);

/**
   @brief A nicer allocation function.

   This macro/function wraps malloc().  It's inspired by the g_new function in
   glib.  It allows you to allocate memory and adjust the allocation counter in
   one call.  Instead of specifying the size of the memory, you specify type and
   number of instances.

   @param type The type of the memory to allocate.
   @param n The number of instances to allocate.
   @returns A pointer to the allocated memory, casted to the correct type.
 */
#define smb_new(type, n) ((type*) smb___new((n) * sizeof(type)))

/**
   @brief A nicer reallocation function.

   This macro/function wraps realloc().  It allows you reallocate memory and
   adjust the allocation counter accordingly.  Exits on failure.

   @param type The type of the memory to reallocate.
   @param ptr The memory to reallocate.
   @param newamt The amount of items to allocate.
   @param oldamt The previous amount of items that were allocated.
   @returns A pointer to the reallocated memory.
 */
#define smb_renew(type, ptr, newamt) \
  ((type*) smb___renew(ptr, (newamt) * sizeof(type)))

/**
   @brief A nicer free function.

   This macro/function wraps free().  It exits on failure and adjusts the malloc
   counter.

   @param type The type of memory you're freeing.
   @param ptr The memory you're freeing.
   @param amt The number of items you're freeing.
 */
#define smb_free(ptr) smb___free(ptr)

/*******************************************************************************

                                 Error Handling

*******************************************************************************/

/**
   @brief An enumeration of all possible status values for libstephen functions.
 */
typedef int smb_status;

#define SMB_SUCCESS 0
#define SMB_INDEX_ERROR 1
#define SMB_NOT_FOUND_ERROR 2
#define SMB_STOP_ITERATION 3
#define SMB_EXTERNAL_EXCEPTION_START 100

char *smb_status_string(smb_status status);

#define PRINT_ERROR_LOC fprintf(stderr, "An error occurred at line %d in file "\
                                "%s (function %s).\n", __LINE__, __FILE__,\
                                __func__)

#endif // LIBSTEPHEN_BASE_H
