/*******************************************************************************

  File:         libstephen.h
  
  Author:       Stephen Brennan

  Date Created: Friday, 27 September 2013

  Description: The public interface of libstephen.  Libstephen is a C library
  designed to provide basic data structures and operations to supplement the
  standard C library.  It is mainly a hobby programming project, but on
  completion it should be usable for basing future projects on it.

*******************************************************************************/

#ifndef SMB___LIBSTEPHEN_H_
#define SMB___LIBSTEPHEN_H_

// Required for size_t
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
// USEFUL MACROS
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
    Configuration Section
    Comment/Uncomment the #defines to enable or disable functionality
*******************************************************************************/

#define SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_ENABLE_DIAGNOSTIC_PRINTING

/*******************************************************************************
    SMB_ENABLE_MEMORY_DIAGNOSTICS
    All this functionality is enabled when SMB_MEMORY_DIAGNOSTICS is defined.
*******************************************************************************/

// These functions should NOT be called on their own.  The macros should be
// used, as they are macro controlled.
void smb___helper_inc_malloc_counter(size_t number_of_mallocs);
void smb___helper_dec_malloc_counter(size_t number_of_frees);
size_t smb___helper_get_malloc_counter();

/**
   Call this function with the number of bytes every time you allocate memory.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_INCREMENT_MALLOC_COUNTER(x) smb___helper_inc_malloc_counter(x)
#else
#define SMB_INCREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function with the number of bytes every time you free memory.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_DECREMENT_MALLOC_COUNTER(x) smb___helper_dec_malloc_counter(x)
#else
#define SMB_DECREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function to get the number of bytes currently allocated by my code.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_GET_MALLOC_COUNTER smb___helper_get_malloc_counter()
#else
#define SMB_GET_MALLOC_COUNTER
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_CODE
    Allows writing diagnostic code.  Enabled by SMB_DIAGNOSTIC_CODE
*******************************************************************************/

/**
   For running diagnostic code.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_DIAG_ONLY(x) x
#else
#define SMB_DIAG_ONLY(x)
#endif // SMB_ENABLE_DIAGNOSTIC_CODE

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_PRINTING
*******************************************************************************/

/**
   Pass the given parameters to printf if printing is enabled.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_PRINTING
#define SMB_DIAG_PRINT(x...) printf(x)
#else
#define SMB_DIAG_PRINT(x...)
#endif // SMB_ENABLE_DIAGNOSTIC_PRINTING


////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLING
////////////////////////////////////////////////////////////////////////////////

// Not for external use
#define ERROR_VAR smb___error_var_
extern unsigned int ERROR_VAR;

// Error codes
#define ALLOCATION_ERROR 0x0001
#define INDEX_ERROR 0x0002

// Error checking
#define RAISE(x) (ERROR_VAR |= x)
#define CHECK(x) (ERROR_VAR & x)
#define CLEAR(x) (ERROR_VAR &= (~x))

// Clear all errors
#define CLEAR_ALL_ERRORS ERROR_VAR = 0

////////////////////////////////////////////////////////////////////////////////
// UNIT TESTING
////////////////////////////////////////////////////////////////////////////////

#define SMB_UNIT_DESCRIPTION_SIZE 20
#define SMB_UNIT_TESTS_PER_GROUP 20

/**
   TEST:

   Defines a single unit test.  Members should be modified with care, preferably
   not at all, except by using the smbunit functions.

   # Members #
   
   - char description[20]: a 20 character null-terminated string that identifies
     this particular test.

   - int (*run)(): function pointer to the test to run.  The function should
     return 0 if the test is successful.

   - int expected_errors: contains error flags to CHECK() for after execution of
     the test.  The test will fail if at least one of the errors in the flag are
     not raised.
 */
typedef struct smb_unit_test
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int (*run)();
  int expected_errors;
  int check_mem_leaks;

} TEST;

/**
   TEST_GROUP:

   A structure holding a group of unit tests that are all related.  Members
   shouldn't me modified by client code.  All should be managed by the functions
   in smbunit.

   # Members #

   - char description[]: a short description (length defined by
     SMB_UNIT_DESCRIPTION_SIZE) for the test.

   - int num_tests: the number of tests in the group.

   - TEST *tests[]: pointers to the tests contained.  Max tests defined by
     SMB_UNIT_TESTS_PER_GROUP.
 */
typedef struct smb_unit_test_group
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int num_tests;
  TEST *tests[SMB_UNIT_TESTS_PER_GROUP];

} TEST_GROUP;

/**
   su_create_test:
   
   Create and return a new unit test.

   # Parameters #

   - char *description: a description of the test.

   - int (*run)(): a function pointer to the test function.

   - int expected_errors: the errors you expect from the test function.  0 if
     non.  You can combine more than one error with &.

   - int check_mem_leaks: whether to check if the mallocs before = mallocs
     after.  0 for no, 1 for yes.

   # Returns #

   A pointer to the new test.
 */
TEST *su_create_test(char *description, int (*run)(), int expected_errors, int check_mem_leaks);

/**
   su_create_test_group:
   
   Create and return a new test group.

   # Parameters #

   - char *description: a short description for the group.

   # Returns #

   A pointer to the test group.
 */
TEST_GROUP *su_create_test_group(char *description);
void su_add_test(TEST_GROUP *group, TEST *test);
int su_run_test(TEST *test);
int su_run_group(TEST_GROUP *group);
void su_delete_test(TEST *test);
void su_delete_group(TEST_GROUP *group);

#define TEST_ASSERT(expr, retval) if(!(expr)) return retval

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DATA TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic data type for storage in the data structures.
 */
typedef union long_data {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;

} DATA;

////////////////////////////////////////////////////////////////////////////////
// LINKED LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Node structure for linked list.  This must be exposed in order for other data
   types to be public.  This should not be used by users of the library.
 */
typedef struct ll_node 
{
  struct ll_node *prev;
  struct ll_node *next;
  DATA data;

} NODE;

/**
   The actual linked list data type.  "Bare" functions return a pointer to this
   structure.
 */
typedef struct ll_obj
{
  NODE *head;
  NODE *tail;
  int length;

} LINKED_LIST;

/**
   A linked list iterator.
 */
typedef struct ll_iter
{
  LINKED_LIST *list;
  NODE *current;
  int index;

} LL_ITERATOR;

///////////////////////////////////////////////////////////////////////////////
// ARRAY LIST
///////////////////////////////////////////////////////////////////////////////

typedef struct al_obj
{
  DATA *data;
  int length;
  int allocated;

} ARRAY_LIST;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// GENERIC TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic list data structure.
 */
typedef struct smb_list 
{
  // Pointer to the actual structure in memory.  Could be any type.
  void *data;

  // Traditional List functions
  void (*append)(struct smb_list *l, DATA newData);
  void (*prepend)(struct smb_list *l, DATA newData);
  
  DATA (*get)(struct smb_list *l, int index);
  void (*set)(struct smb_list *l, int index, DATA newData);
  void (*remove)(struct smb_list *l, int index);
  void (*insert)(struct smb_list *l, int index, DATA newData);
  void (*delete)(struct smb_list *l);
  int (*length)(struct smb_list *l);

  // Stack/Queue/Deque Functions
  void (*push_back)(struct smb_list *l, DATA newData);
  DATA (*pop_back)(struct smb_list *l);
  DATA (*peek_back)(struct smb_list *l);
  
  void (*push_front)(struct smb_list *l, DATA newData);
  DATA (*pop_front)(struct smb_list *l);
  DATA (*peek_front)(struct smb_list *l);

  // Iterator functions may reside here in the near future.

} LIST;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// "BARE" DATA TYPE FUNCTION DECLARATION
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// LINKED LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Creates a new list with the given data.

   DATA newData - the data to store.

   return - a pointer to the list.
 */
LINKED_LIST *ll_create(DATA newData);

/**
   Creates a new list empty.
 */
LINKED_LIST *ll_create_empty();

/**
   Creates a new list with initial data.  Returns an instance of the interface.
 */
LIST ll_create_list(DATA newData);

/**
   Create a new list empty.  Returns an instance of the list interface.
 */
LIST ll_create_empty_list();

/**
   Cast a LINKED_LIST pointer to an instance of the list interface.
 */
LIST ll_cast_to_list(LINKED_LIST *list);

/**
   Append the given data to the end of the list.
 */
void ll_append(LINKED_LIST *list, DATA newData);

/**
   Prepend the given data to the beginning of the list.
 */
void ll_prepend(LINKED_LIST *list, DATA newData);

/**
   Push the data to the back of the list.  An alias for ll_append.
 */
void ll_push_back(LINKED_LIST *list, DATA newData);

/**
   Pop data from the back of the list.
 */
DATA ll_pop_back(LINKED_LIST *list);

/**
   Peek at the back of the list.
 */
DATA ll_peek_back(LINKED_LIST *list);

/**
   Push the data to the front of the list.  An alias for ll_prepend.
 */
void ll_push_front(LINKED_LIST *list, DATA newData);

/**
   Pop the data from the front of the list.
 */
DATA ll_pop_front(LINKED_LIST *list);

/**
   Peek at the front of the list.
 */
DATA ll_peek_front(LINKED_LIST *list);

/**
   Gets the data from the given index.  However, there is no guarantee that the
   index was valid.  An empty DATA object is returned in that case.
 */
DATA ll_get(LINKED_LIST *list, int index);

/**
   Removes the node at the given index.
 */
void ll_remove(LINKED_LIST *list, int index);

/**
   Inserts the item at the specified location in the list, pushing back
   elements.
 */
void ll_insert(LINKED_LIST *list, int index, DATA newData);

/**
   Removes the linked list.
 */
void ll_delete(LINKED_LIST *list);

/**
   Sets an existing element to a new value.
 */
void ll_set(LINKED_LIST *list, int index, DATA newData);

/**
   Returns the length of the given list.
 */
int ll_length(LINKED_LIST *list);

/**
   Get an iterator for the linked list.
 */
LL_ITERATOR ll_get_iter(LINKED_LIST *list);

/**
   Advance the iterator and return the data at it.
 */
DATA ll_iter_next(LL_ITERATOR *iterator);

/**
   Move the iterator back and return the data at it.
 */
DATA ll_iter_prev(LL_ITERATOR *iterator);

/**
   Get the current data.
 */
DATA ll_iter_curr(LL_ITERATOR *iterator);

/**
   Check if the iterator can be advanced.
 */
int ll_iter_has_next(LL_ITERATOR *iterator);

/**
  Check if the iterator can be moved back.
 */
int ll_iter_has_prev(LL_ITERATOR *iterator);

/**
   Check if the iterator is valid.
 */
int ll_iter_valid(LL_ITERATOR *iterator);

////////////////////////////////////////////////////////////////////////////////
// ARRAY LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Create an ARRAY_LIST initialized with the given first element.
 */
ARRAY_LIST *al_create(DATA newData);

/**
   Create an ARRAY_LIST with no data.
 */
ARRAY_LIST *al_create_empty();

void al_append(ARRAY_LIST *list, DATA newData);

void al_prepend(ARRAY_LIST *list, DATA newData);

DATA al_get(ARRAY_LIST *list, int index);

void al_set(ARRAY_LIST *list, int index, DATA newData);

void al_remove(ARRAY_LIST *list, int index);

void al_insert(ARRAY_LIST *list, int index, DATA newData);

void al_delete(ARRAY_LIST *list);

int al_length(ARRAY_LIST *list);

void al_push_back(ARRAY_LIST *list, DATA newData);

DATA al_pop_back(ARRAY_LIST *list);

DATA al_peek_back(ARRAY_LIST *list);

void al_push_front(ARRAY_LIST *list, DATA newData);

DATA al_pop_back(ARRAY_LIST *list);

DATA al_peek_back(ARRAY_LIST *list);

#endif // SMB___LIBSTEPHEN_H_
