/***************************************************************************//**

  @file         ad.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Arg Data

  @copyright    Copyright (c) 2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef LIBSTEPHEN_AD_H
#define LIBSTEPHEN_AD_H

#include <stdint.h> /* uint64_t */

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

#endif // LIBSTEPHEN_AD_H
