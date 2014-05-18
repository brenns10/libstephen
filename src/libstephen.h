/**
  @file    libstephen.h
  
  @author  Stephen Brennan

  @date    Created Friday, 27 September 2013

  @brief   Public interface of libstephen

  Libstephen is a C library designed to provide basic data structures and
  operations to supplement the standard C library.  It is mainly a hobby
  programming project, but on completion it should be usable for basing future
  projects on it.
*/

#ifndef SMB___LIBSTEPHEN_H_
#define SMB___LIBSTEPHEN_H_

#include <stdlib.h>       /* size_t */
#include <stdint.h>       /* uint64_t */


/*******************************************************************************

                                 Miscellaneous

*******************************************************************************/

#ifdef SMB_CONF
#include "libstephen_conf.h"
#endif

#ifdef SMB_DEBUG
#define SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_ENABLE_DIAGNOSTIC_PRINTING
#endif

/**
   @brief Increments the memory allocation counter.  Don't use this.

   This function is used for memory management.  It is enabled and disabled by
   the preprocessor.  So you should stick to the macros for this.

   @see SMB_INCREMENT_MALLOC_COUNTER

   @param number_of_mallocs The number of bytes allocated.
 */
void smb___helper_inc_malloc_counter(size_t number_of_mallocs);

/**
   @brief Decrements memory allocation counter.  Don't use.  

   @see smb___helper_inc_malloc_counter
   @see SMB_DECREMENT_MALLOC_COUNTER

   @param The number of bytes freed.
 */
void smb___helper_dec_malloc_counter(size_t number_of_frees);

/**
   @brief Gets the memory allocation counter.  Don't use.

   @see smb___helper_inc_malloc_counter
   @see SMB_GET_MALLOC_COUNTER

   @returns The number of bytes allocated.
 */
size_t smb___helper_get_malloc_counter();

/**
   @brief Increment the memory allocation counter.

   Call this function with the number of bytes every time you allocate memory.
   This will only do anything if SMB_MEMORY_DIAGNOSTICS is defined.

   @param The number of bytes allocated.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_INCREMENT_MALLOC_COUNTER(x) smb___helper_inc_malloc_counter(x)
#else
#define SMB_INCREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   @brief Decrement the memory allocation couter.

   Call this function with the number of bytes every time you free memory.
   This will only do anything if SMB_MEMORY_DIAGNOSTICS is defined.

   @param The number of bytes freed.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_DECREMENT_MALLOC_COUNTER(x) smb___helper_dec_malloc_counter(x)
#else
#define SMB_DECREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   @brief Get the memory allocation counter.

   Call this function to get the number of bytes currently allocated by my code.
   Only does anything if SMB_MEMORY_DIAGNOSTICS is defined.  If it's not
   defined, evaluates to 0.
   
   @returns The number of bytes allocated right now.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_GET_MALLOC_COUNTER smb___helper_get_malloc_counter()
#else
#define SMB_GET_MALLOC_COUNTER 0
#endif

/**
   @brief Run this code only if SMB_ENABLE_DIAGNOSTIC_CODE is defined.
 */
#ifdef SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_DIAG_ONLY(x) x
#else
#define SMB_DIAG_ONLY(x)
#endif // SMB_ENABLE_DIAGNOSTIC_CODE

/**
   @brief Print this (printf) only if SMB_ENABLE_DIAGNOSTIC_PRINTING is defined.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_PRINTING
#define SMB_DP(x...) printf(x)
#else
#define SMB_DP(x...)
#endif // SMB_ENABLE_DIAGNOSTIC_PRINTING

/**
   @brief Base data type for the data structures.

   Capable of containing long integers, double precision floats, or pointers.
   Takes up 8 bytes.
 */
typedef union DATA {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;

} DATA;

/**
   @brief A function pointer that takes a DATA and performs an action on it 

   The function could count it, call free on it, print it, etc.  Useful for
   stuff like deleting data structures full of items (if they're pointers to
   dynamically allocated data, they'll need to be freed), applying an action to
   every item in a list (e.g. printing), and many more applications.
 */
typedef void (*DATA_ACTION)(DATA toDelete);

/*******************************************************************************

                                 Error Handling

*******************************************************************************/

/**
   @brief The name of the error variable.

   It's supposed to be a bit mangled and useless to avoid naming conflicts.
 */
#define ERROR_VAR smb___error_var_

/**
   @brief A variable declared in some far-off C file, which contains your
   errors.
 */
extern unsigned int ERROR_VAR;

/**
   @brief Reserved for communicating errors with malloc().
 */
#define ALLOCATION_ERROR 0x0001

/**
   @brief Set when an provided index is out of range.  Includes incidents when
   you try to pop or peek at an empty list.
 */
#define INDEX_ERROR 0x0002

/**
   @brief Set when an item is not found, mainly in the hash table.
 */
#define NOT_FOUND_ERROR 0x0004

/**
   @brief Set the flag corresponding to the given error code.
 */
#define RAISE(x) (ERROR_VAR |= x)

/**
   @brief Test the flag corresponding to the given error code.  

   @retval 1 if flag set.
   @retval 0 if flag cleared.
 */
#define CHECK(x) (ERROR_VAR & x)

/**
   @brief Clear the flag corresponding to the given error code.
 */
#define CLEAR(x) (ERROR_VAR &= (~x))

/**
   @brief Clear the flags for all errors.
 */
#define CLEAR_ALL_ERRORS ERROR_VAR = 0

/*******************************************************************************

                             Unit Testing (smb_ut)

*******************************************************************************/

/**
   @brief The size of a description text field (in characters) for an smbunit test.
 */
#define SMB_UNIT_DESCRIPTION_SIZE 20

/**
   @brief The max number of unit tests in a single test group.
 */
#define SMB_UNIT_TESTS_PER_GROUP 20

/**
   @brief Asserts that an expression is true.  If false, returns a given value.

   Note that this is a macro, so using some things (++ and -- operators
   especially) can have unintended results.  Any code in expr will be executed
   every time, but code within retval will only be executed when the assertion
   fails.  As a general rule, do not put any complicated code inside the
   TEST_ASSERT() statement at all.

   # PARAMETERS #

   - expr: The expression to check.  If the expression evaluates to true (that
     is, not 0), the assertion passes.  If the expression evaluates to false
     (that is, zero), the assertion fails.

   - retval: An integer return code.  The return code will be given in the test
     output on failure, so follow a convention that makes it easy to identify
     exactly at what point the code fails.
 */
#define TEST_ASSERT(expr, retval) if(!(expr)) return retval

/**
   @brief Defines a single unit test.  

   Members should be modified with care, preferably not at all, except by using
   the smbunit functions.

 */
typedef struct smb_ut_test
{
  /**
     @brief A 20 character null-terminated string that identifies this
     particular test.
   */
  char description[SMB_UNIT_DESCRIPTION_SIZE];

  /**
     @brief Function pointer to the test to run.  The function should return 0
     if the test is successful.
   */
  int (*run)();

  /**
     @brief The error value of any expected errors from this test.
   */
  int expected_errors;

  /**
     @brief Should we check for memory leaks?
   */
  int check_mem_leaks;

} smb_ut_test;

/**
   @brief A structure holding a group of unit tests that are all related.  

   Members shouldn't me modified by client code.  All should be managed by the
   functions in smbunit.
 */
typedef struct smb_ut_group
{
  /**
     @brief A short description (length defined by SMB_UNIT_DESCRIPTION_SIZE)
     for the test.
   */
  char description[SMB_UNIT_DESCRIPTION_SIZE];

  /**
     @brief The number of tests in the group.
  */
  int num_tests;

  /**
     @brief Pointers to the tests contained.  Max amount of tests is SMB_UNIT_TESTS_PER_GROUP.
   */
  smb_ut_test *tests[SMB_UNIT_TESTS_PER_GROUP];

} smb_ut_group;

smb_ut_test *su_create_test(char *description, int (*run)(), 
                                   int expected_errors, int check_mem_leaks);
smb_ut_group *su_create_test_group(char *description);
void su_add_test(smb_ut_group *group, smb_ut_test *test);
int su_run_test(smb_ut_test *test);
int su_run_group(smb_ut_group *group);
void su_delete_test(smb_ut_test *test);
void su_delete_group(smb_ut_group *group);

/*******************************************************************************

                          Generic List Data Structure

*******************************************************************************/

/**
   @brief A generic list data structure.

   Can represent an array list or a linked list.  Uses function pointers to hide
   the implementation.  Has heavyweight memory requirements (many pointers).
   Function calls must be made like this:

       list->functionName(list, <params...>)
 */
typedef struct smb_list 
{
  /**
     @brief A pointer to implementation-specific data.

     This data is used by the rest of the functions in the struct to perform all
     required actions.
   */
  void *data;

  /**
     @see ll_append @see al_append
   */
  void (*append)(struct smb_list *l, DATA newData);

  /**
     @see ll_prepend @see al_prepend
   */
  void (*prepend)(struct smb_list *l, DATA newData);

  /**
     @see ll_get @see al_get
   */
  DATA (*get)(const struct smb_list *l, int index);

  /**
     @see ll_set @see al_set
   */
  void (*set)(struct smb_list *l, int index, DATA newData);
  
  /**
     @see ll_remove @see al_remove
   */
  void (*remove)(struct smb_list *l, int index);

  /**
     @see ll_insert @see al_insert
   */
  void (*insert)(struct smb_list *l, int index, DATA newData);

  /**
     @see ll_delete @see al_delete
   */
  void (*delete)(struct smb_list *l);

  /**
     @see ll_length @see al_length
   */
  int (*length)(const struct smb_list *l);

  /**
     @see ll_push_back @see al_push_back
   */
  void (*push_back)(struct smb_list *l, DATA newData);

  /**
     @see ll_pop_back @see al_pop_back
   */
  DATA (*pop_back)(struct smb_list *l);

  /**
     @see ll_peek_back @see al_peek_back
   */
  DATA (*peek_back)(struct smb_list *l);
  
  /**
     @see ll_push_front @see al_push_front
   */
  void (*push_front)(struct smb_list *l, DATA newData);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*pop_front)(struct smb_list *l);

  /**
     @see ll_pop_front @see al_pop_front
   */
  DATA (*peek_front)(struct smb_list *l);

} smb_list;

/*******************************************************************************

                              Linked List (smb_ll)

*******************************************************************************/

/**
   @brief Node structure for linked list.  

   This must be exposed in order for other data types to be public.  This should
   not be used by users of the library.
 */
typedef struct smb_ll_node
{
  /**
     @brief The previous node in the linked list.
   */
  struct smb_ll_node *prev;

  /**
     @brief The next node in the linked list.
   */
  struct smb_ll_node *next;

  /**
     @brief The data stored in this node.
   */
  DATA data;

} smb_ll_node;

/**
   @brief The actual linked list data type.  "Bare" functions return a pointer
   to this structure.
 */
typedef struct smb_ll
{
  /**
     @brief A pointer to the head of the list.
   */
  struct smb_ll_node *head;

  /**
     @brief A pointer to the tail of the list.  Useful for speeding up
     navigation to a particular spot.
   */
  struct smb_ll_node *tail;

  /**
     @brief The number of items stored in the list.
   */
  int length;

} smb_ll;

/**
   @brief A linked list iterator.  Do not modify the structure yourself.
 */
typedef struct smb_ll_iter
{
  /**
     @brief A pointer to the list being used.
   */
  struct smb_ll *list;

  /**
     @brief A pointer to the current node in the list.
   */
  struct smb_ll_node *current;

  /**
     @brief The index associated with this item.
   */
  int index;

} smb_ll_iter;

void ll_init(smb_ll *newList);
smb_ll *ll_create();
void ll_destroy(smb_ll *list);
void ll_delete(smb_ll *list);

smb_list ll_create_list();
smb_list ll_cast_to_list(smb_ll *list);

void ll_append(smb_ll *list, DATA newData);
void ll_prepend(smb_ll *list, DATA newData);
void ll_push_back(smb_ll *list, DATA newData);
DATA ll_pop_back(smb_ll *list);
DATA ll_peek_back(smb_ll *list);
void ll_push_front(smb_ll *list, DATA newData);
DATA ll_pop_front(smb_ll *list);
DATA ll_peek_front(smb_ll *list);
DATA ll_get(const smb_ll *list, int index);
void ll_remove(smb_ll *list, int index);
void ll_insert(smb_ll *list, int index, DATA newData);
void ll_set(smb_ll *list, int index, DATA newData);
int ll_length(const smb_ll *list);

smb_ll_iter ll_get_iter(smb_ll *list);
DATA ll_iter_next(smb_ll_iter *iterator);
DATA ll_iter_prev(smb_ll_iter *iterator);
DATA ll_iter_curr(smb_ll_iter *iterator);
int ll_iter_has_next(smb_ll_iter *iterator);
int ll_iter_has_prev(smb_ll_iter *iterator);
int ll_iter_valid(smb_ll_iter *iterator);

/*******************************************************************************

                                   Array List

*******************************************************************************/

/**
   @brief The actual array list data type.

   "Bare" functions return a pointer to this structure.  You should not use any
   of the members, as they are implementation specific and subject to change.
 */
typedef struct smb_al
{
  /**
     @brief The area of memory containing the data.
   */
  DATA *data;

  /**
     @brief The number of items in the list.
   */
  int length;

  /**
     @brief The space allocated for the list.
   */
  int allocated;

} smb_al;

void al_init(smb_al *list);
smb_al *al_create();
void al_destroy(smb_al *list);
void al_delete(smb_al *list);

void al_append(smb_al *list, DATA newData);
void al_prepend(smb_al *list, DATA newData);
DATA al_get(const smb_al *list, int index);
void al_remove(smb_al *list, int index);
void al_insert(smb_al *list, int index, DATA newData);
void al_set(smb_al *list, int index, DATA newData);
void al_push_back(smb_al *list, DATA newData);
DATA al_pop_back(smb_al *list);
DATA al_peek_back(smb_al *list);
void al_push_front(smb_al *list, DATA newData);
DATA al_pop_front(smb_al *list);
DATA al_peek_front(smb_al *list);
int al_length(const smb_al *list);
int al_index_of(const smb_al *list, DATA d);

/*******************************************************************************

                              Hash Table (smb_ht)

*******************************************************************************/

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
     @brief Pointer to the data of the table.
   */
  struct smb_ht_bckt **table;

} smb_ht;

void ht_init(smb_ht *pTable, HASH_FUNCTION hash_func);
smb_ht *ht_create(HASH_FUNCTION hash_func);
void ht_destroy_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_destroy(smb_ht *pTable);
void ht_delete_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_delete(smb_ht *pTable);

void ht_insert(smb_ht *pTable, DATA dKey, DATA dValue);
void ht_remove_act(smb_ht *pTable, DATA dKey, DATA_ACTION deleter);
void ht_remove(smb_ht *pTable, DATA dKey);
DATA ht_get(smb_ht const *pTable, DATA dKey);
unsigned int ht_string_hash(DATA data);
void ht_print(smb_ht const *pTable, int full_mode);

/*******************************************************************************

                               Arg Data (smb_ad)

*******************************************************************************/

/**
   @brief The number of regular flags.  52 = 26 + 26.
 */
#define MAX_FLAGS 52

/**
   @brief Data structure to store information on arguments passed to the program.
 */
typedef struct smb_ad
{
  /**
     @brief Holds boolean value for whether each character flag is set.
   */
  uint64_t flags;

  /**
     @brief Holds the parameters for each regular (character) flag.
   */
  char *flag_strings[MAX_FLAGS];

  /**
     @brief Holds the long flags.
   */
  struct smb_ll *long_flags;

  /**
     @brief Holds the parameters of the long flags.
   */
  struct smb_ll *long_flag_strings;

  /**
     @brief Holds the bare strings (strings that aren't flags or flag params).
   */
  struct smb_ll *bare_strings;

} smb_ad;

void arg_data_init(smb_ad *data);
smb_ad *arg_data_create();
void arg_data_destroy(smb_ad *data);
void arg_data_delete(smb_ad *data);

void process_args(smb_ad *data, int argc, char **argv);
int check_flag(smb_ad *data, char flag);
int check_long_flag(smb_ad *data, char *flag);
int check_bare_string(smb_ad *data, char *string);
char *get_flag_parameter(smb_ad *data, char flag);
char *get_long_flag_parameter(smb_ad *data, char *string);

/*******************************************************************************

                  Bitfield Macros and Data Structures (smb_bf)

*******************************************************************************/

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

#endif // SMB___LIBSTEPHEN_H_
