/***************************************************************************//**

  @file         ad.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Arg Data

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
void ad_print(smb_ad *data, FILE *f);

#endif // LIBSTEPHEN_AD_H
