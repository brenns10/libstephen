/***************************************************************************//**

  @file         ad.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Contains functions to simplify processing command line args.

  The args library analyzes args as generally as possible.  It recognizes three
  different types.  First is the 'flag' or 'regular flag', which is simply one
  character.  Their order doesn't matter.  They're placed on the command line
  after a single hyphen.

  Then there are long flags, which are strings that come after two hyphens.
  Both flags and long flags are allowed to have parameters passed with them.

  Finally are bare strings, which are things that aren't passed as parameters or
  as flags.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_AD_H
#define LIBSTEPHEN_AD_H

#include <stdint.h> /* uint64_t */
#include "base.h"

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

/**
   @brief Initialize an smb_ad structure in memory that has already been
   allocated.
   @param data The pointer to the memory to allocate in.
 */
void arg_data_init(smb_ad *data);
/**
   @brief Allocate and initialize a smb_ad structure for argument parsing.
   @returns A pointer to the structure.
 */
smb_ad *arg_data_create();
/**
   @brief Free the resources of an arg_data object, but do not free it.
   @param data The arg data to destroy.
 */
void arg_data_destroy(smb_ad *data);
/**
   @brief Free the resources of an arg data object, and then free the object
   itself.
   @param data The arg data to delete.
 */
void arg_data_delete(smb_ad *data);

/**
   @brief Analyze the arguments passed to the program.

   Pass in the argc and argv, but make sure to decrement and increment each
   respective variable so they do not include the name of the program.  Unless,
   of course, you want the program name to be processed as an argument.  Once
   you have called this function, you can use the querying functions to find all
   the arguments.

   @param data A pointer to an smb_ad object.
   @param argc The number of arguments (not including program name).
   @param argv The arguments themselves (not including program name).
 */
void process_args(smb_ad *data, int argc, char **argv);
/**
   @brief Check whether a flag is raised.
   @param data The smb_ad returned by process_args().
   @param flag The character flag to check.  Alphabetical only.
   @retval 0 if the flag was not set.
   @retval 1 otherwise.
 */
int check_flag(smb_ad *data, char flag);
/**
   @brief Check whether a long flag appeared.  It must occur verbatim.
   @param data The smb_ad returned by process_args().
   @param flag The string flag to check for.
   @returns the index of the flag + 1 if it is set.
   @retval 0 iff the flag was not set.
 */
int check_long_flag(smb_ad *data, char *flag);
/**
   @brief Check whether a bare string appeared.  It must occur verbatim.
   @param data The smb_ad returned by process_args().
   @param string The string to search for.
   @returns the index of the flag + 1 if it is set.
   @retval 0 iff the flag was not set.
 */
int check_bare_string(smb_ad *data, char *string);
/**
   @brief Return the string parameter associated with the flag.
   @param data The smb_ad returned by process_args().
   @param flag The flag to find parameters of.
   @returns The parameter of the flag.
 */
char *get_flag_parameter(smb_ad *data, char flag);
/**
   @brief Return the string parameter associated with the long string.
   @param data The smb_ad returned by process_args().
   @param string The long flag to find parameters of.
   @returns The parameter of the long flag.
   @retval NULL if no parameter or if flag not found.
 */
char *get_long_flag_parameter(smb_ad *data, char *string);
/**
   @brief Print the contents of the arg data struct to a file for debugging.
   @param data The struct to print.
   @param f The file to print to.
 */
void ad_print(smb_ad *data, FILE *f);

#endif // LIBSTEPHEN_AD_H
