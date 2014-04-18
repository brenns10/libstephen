/*******************************************************************************

  File:         args.c

  Author:       Stephen Brennan

  Date Created: Thursday, 31 October 2013

  Description:  Contains functions to simplify processing command line args.

*******************************************************************************/

#include "libstephen.h"
#include <stdio.h>
#include <string.h>

/*
  
  The args part of the library should be able to analyze args as general as
  possible.  It will accept three different types of arguments.  There is the
  "flag", which is a single character only.  Flags are placed on the command
  line after a single hyphen.  It represents something similar to a boolean
  value (ie, whether the flag is present or not).  A flag can also contain an
  argument.  This argument will come directly after the flag.  It is a single
  string.  Flags also can be grouped after a single hyphen.  For example, the
  command "pacman -Syu" has S, y, and u flags set.

  The second type of argument is the long flag.  It is a string value (no
  spaces) that comes after two hyphens.  It also can represent a boolean type
  value.  It is usually used as a verbose alternative so that the nature of that
  option is either hard to type, or easy to read.  It can also have a string
  value that occurs with it.

  The final type of argument is the bare string.  This is basically any string
  that is not preceeded by a flag or long flag.

 */

////////////////////////////////////////////////////////////////////////////////
// AUXILIARY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

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
   Add the regular to the flag to the data structure.
 */
char process_flag(ARG_DATA *pData, char *cFlags)
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
   Add the long flag to the data structure.
 */
char *process_long_flag(ARG_DATA *pData, char *sTitle)
{
  DATA d, e;
  d.data_ptr = sTitle + 2;
  ll_append(pData->long_flags, d); // Add the portion AFTER the "--"
  e.data_ptr = NULL;
  ll_append(pData->long_flag_strings, e);
  return sTitle + 2;
}

void process_bare_string(ARG_DATA *pData, char *sStr, char *previous_long_flag, 
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
   Find the string in the list and return its index, or -1.
 */
int find_string(struct smb_ll *toSearch, char *toFind)
{
  DATA d;
  char *sCurr;
  struct smb_ll_iter iterator = ll_get_iter(toSearch);
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

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

void arg_data_init(ARG_DATA *data)
{
  data->flags = 0;
  for (int i = 0; i < MAX_FLAGS; i++) data->flag_strings[i] = NULL;
  data->long_flags = ll_create();
  data->long_flag_strings = ll_create();
  data->bare_strings = ll_create();
}

ARG_DATA *arg_data_create()
{
  ARG_DATA *data = (ARG_DATA*) malloc(sizeof(ARG_DATA));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(ARG_DATA));

  arg_data_init(data);

  return data;
}

/**
   Process args.  ASSUMES THAT THE PROGRAM IS REMOVED FROM THE LIST OF ARGS.
 */
void process_args(ARG_DATA *data, int argc, char **argv)
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
   Free the resources of an arg_data object.
 */
void arg_data_destroy(ARG_DATA * data)
{
  ll_delete(data->long_flags);
  ll_delete(data->bare_strings);
  ll_delete(data->long_flag_strings);
}

void arg_data_delete(ARG_DATA *data)
{
  arg_data_destroy(data);

  free(data);
  SMB_DECREMENT_MALLOC_COUNTER(sizeof(ARG_DATA));
}

/**
   Checks whether a short flag was set.
 */
int check_flag(ARG_DATA *pData, char flag)
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
   Check whether a long flag was set.  Return the index + 1 of the flag.
 */
int check_long_flag(ARG_DATA *data, char *flag)
{
  return find_string(data->long_flags, flag) + 1;
}

/**
   Check whether a bare string appeared.
 */
int check_bare_string(ARG_DATA *data, char *string)
{
  return find_string(data->bare_strings, string) + 1;
}

/**
   Get the parameter associated with a flag.
 */
char *get_flag_parameter(ARG_DATA *data, char flag)
{
  int index = flag_index(flag);
  if (index == -1)
    return NULL;
  else 
    return data->flag_strings[index];
}

/**
   Get the parameter associated with a long flag.
 */
char *get_long_flag_parameter(ARG_DATA *data, char *string)
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
