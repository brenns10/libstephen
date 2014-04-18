/**

  @file    args.c

  @author  Stephen Brennan

  @date    Created Thursday, 31 October 2013

  @brief   Contains functions to simplify processing command line args.

  The args library analyzes args as generally as possible.  It recognizes three
  different types.  First is the 'flag' or 'regular flag', which is simply one
  character.  Their order doesn't matter.  They're placed on the command line
  after a single hyphen.

  Then there are long flags, which are strings that come after two hyphens.
  Both flags and long flags are allowed to have parameters passed with them.

  Finally are bare strings, which are things that aren't passed as parameters or
  as flags.

  @todo    Possibly add support for flags that aren't alphabetical.
*/

#include "libstephen.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************

                               Private Functions

*******************************************************************************/

/**
   @brief Returns the index of a flag for a specific character.

   Currently, only alphabetical characters are allowed.  This could change.  See
   the top of the file for the todo entry on this.

   @param c The character to find the index of.
   @returns The index of the flag.
 */
int flag_index(char c)
{
  int idx;
  if (c >= 'A' && c <= 'Z') {
    idx = 26 + (c - 'A');
  } else if (c >= 'a' && c <= 'z') {
    idx = (c - 'a');
  } else {
    idx = -1;
  }
  return idx;
}

/**
   @brief Add regular flags (i.e. characters) to the smb_ad structure.

   @param pData The structure containing the arg data.
   @param cFlags The string of flags.  Null terminated.
   @returns The last flag in the string (for parameter purposes).
 */
char process_flag(smb_ad *pData, char *cFlags)
{
  char last;
  int idx;

  while (*cFlags != '\0') {
    if ((idx = flag_index(*cFlags)) != -1) {
      last = *cFlags;
      pData->flags |= 1UL << idx;
    }
    cFlags++;
  }
  return last;
}

/**
   @brief Add the long flag to the data structure.

   @param pData The structure containing the arg data.
   @param sTitle The title of the long flag, including '--'
   @returns The adjusted pointer to the title, without '--'.
 */
char *process_long_flag(smb_ad *pData, char *sTitle)
{
  DATA d, e;
  d.data_ptr = sTitle + 2;
  ll_append(pData->long_flags, d); // Add the portion AFTER the "--"
  e.data_ptr = NULL;
  ll_append(pData->long_flag_strings, e);
  return sTitle + 2;
}

/**
   @brief Add the bare string to the data structure.

   This function will first attempt to add the string as an argument to the
   previous flag, then the previous long flag, and then, finally, it will add
   itself as just a bare string.

   @param pData The structure with argument data.
   @param sStr The string to add.
   @param previous_long_flag The last long flag encountered.
   @param previous_flag The last regular (character) flag encountered.
 */
void process_bare_string(smb_ad *pData, char *sStr, char *previous_long_flag, 
                         char previous_flag)
{
  if (previous_long_flag) {
    DATA d;
    d.data_ptr = sStr;
    ll_pop_back(pData->long_flag_strings);
    ll_push_back(pData->long_flag_strings, d);
  } else if (previous_flag != EOF) {
    int idx = flag_index(previous_flag);
    // The flag index would be negative if the previous flag was not a
    // letter.  We'll accept the segfault as an error.
    pData->flag_strings[idx] = sStr;
  } else {
    DATA d;
    d.data_ptr = sStr;
    ll_append(pData->bare_strings, d);        
  }
}

/**
   @brief Find the string in the list and return its index, or -1.

   @param toSearch Pointer to list to search.
   @param toFind String to find.
   @returns Index of the string.
   @retval -1 String is not in the list.
 */
int find_string(smb_ll *toSearch, char *toFind)
{
  DATA d;
  char *sCurr;
  smb_ll_iter iterator = ll_get_iter(toSearch);
  while (ll_iter_valid(&iterator)) {
    d = ll_iter_curr(&iterator);
    sCurr = (char *)d.data_ptr;
    if (strcmp(toFind, sCurr) == 0) {
      return iterator.index;
    }
    ll_iter_next(&iterator);
  }
  return -1;
}

/*******************************************************************************

                                Public Functions

*******************************************************************************/

/**
   @brief Initialize an smb_ad structure in memory that has already ben
   allocated.

   @param data The pointer to the memory to allocate in.
 */
void arg_data_init(smb_ad *data)
{
  data->flags = 0;
  for (int i = 0; i < MAX_FLAGS; i++) data->flag_strings[i] = NULL;
  data->long_flags = ll_create();
  data->long_flag_strings = ll_create();
  data->bare_strings = ll_create();
}

/**
   @brief Allocate and initialize a smb_ad structure for argument parsing.
   @returns A pointer to the structure.
 */
smb_ad *arg_data_create()
{
  smb_ad *data = (smb_ad*) malloc(sizeof(smb_ad));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(smb_ad));

  arg_data_init(data);

  return data;
}

/**
   @brief Free the resources of an arg_data object, but do not free it.

   @param data The arg data to destroy.
 */
void arg_data_destroy(smb_ad * data)
{
  ll_delete(data->long_flags);
  ll_delete(data->bare_strings);
  ll_delete(data->long_flag_strings);
}

/**
   @brief Free the resources of an arg data object, and then free the object
   itself.

   @param data The arg data to delete.
 */
void arg_data_delete(smb_ad *data)
{
  arg_data_destroy(data);

  free(data);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(smb_ad));
}

/**
   @brief Analyze the argument data passed to the program.  

   Pass in the argc and argv, but make sure to decrement and increment each
   respective variable so they do not include the name of the program.

   @param argc The number of arguments (not including program name).

   @param argv The arguments themselves (not including program name).

   @params A pointer to an smb_ad object.  Use provided functions to query the
   object about every desired flag.
 */
void process_args(smb_ad *data, int argc, char **argv)
{
  char *previous_long_flag = NULL;
  char previous_flag = EOF;

  while (argc--) {
    // At the beginning of the loop, argc refers to number of remaining args,
    // and argv points at the pointer to the current arg.

    switch (**argv) {
    case '-':
      // Processing new flag type, so set previous variables to invalid.
      previous_long_flag = NULL;
      previous_flag = EOF;
      switch (*(*argv + 1)) {
      case '-':
        // Long flag
        previous_long_flag = process_long_flag(data, *argv);
        break;
      case '\0':
        // A single '-'...counts as a bare string in my book
        process_bare_string(data, *argv, previous_long_flag, previous_flag);
        previous_long_flag = NULL;
        previous_flag = EOF;
        break;
      default:
        // The input is a short flag
        previous_flag = process_flag(data, *argv);
        break;
      }
      break;
      
    default:
      // This is a raw string.  We first need to check if it belongs to a flag.
      process_bare_string(data, *argv, previous_long_flag, previous_flag);
      previous_long_flag = NULL;
      previous_flag = EOF;
      break;
    }

    argv++;
  }
}

/**
   @brief Check whether a flag is raised.

   @param data The smb_ad returned by process_args().
   @param flag The character flag to check.  Alphabetical only.
   @retval 0 if the flag was not set.
   @retval 1 otherwise.
 */
int check_flag(smb_ad *pData, char flag)
{
  uint64_t idx;
  idx = flag_index(flag);
  if (idx != -1) {
    idx =  pData->flags & (1UL << idx);
    if (idx) return 1;
  }
  return 0;
}

/**
   @brief Check whether a long flag appeared.  It must occur verbatim.

   @param data The smb_ad returned by process_args().
   @param flag The string flag to check for.
   @returns the index of the flag + 1 if it is set.
   @retval 0 if the flag was not set.
 */
int check_long_flag(smb_ad *data, char *flag)
{
  return find_string(data->long_flags, flag) + 1;
}

/**
   @brief Check whether a bare string appeared.  It must occur verbatim.

   @param data The smb_ad returned by process_args().
   @param string The string to search for.
   @returns the index of the flag + 1 if it is set.
   @retval 0 iff the flag was not set.
 */
int check_bare_string(smb_ad *data, char *string)
{
  return find_string(data->bare_strings, string) + 1;
}

/**
   @brief Return the string parameter associated with the flag.

   @param data The smb_ad returned by process_args().
   @param flag The flag to find parameters of.
   @returns The parameter of the flag.
 */
char *get_flag_parameter(smb_ad *data, char flag)
{
  int index = flag_index(flag);
  if (index == -1)
    return NULL;
  else 
    return data->flag_strings[index];
}

/**
   @brief Return the string parameter associated with the long string.

   @param data The smb_ad returned by process_args().
   @param string The long flag to find parameters of.
   @returns The parameter of the long flag.  
   @retval NULL if no parameter or if flag not found.
 */
char *get_long_flag_parameter(smb_ad *data, char *string)
{
  int index = find_string(data->long_flags, string);
  if (index == -1)
    return NULL;
  else {
    DATA d;
    d = ll_get(data->long_flag_strings, index);
    return (char*)d.data_ptr;
  }
}
