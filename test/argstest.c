/***************************************************************************//**

  @file         argstest.c

  @author       Stephen Brennan

  @date         Created Sunday,  3 November 2013

  @brief        Tests argument parsing functions.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
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

#include <stdio.h>

#include "libstephen/ad.h"
#include "libstephen/ll.h"
#include "libstephen/ut.h"

////////////////////////////////////////////////////////////////////////////////
// Example main function that will process and print results of args analysis.
////////////////////////////////////////////////////////////////////////////////

void iterate_flags(smb_ad *pArgData, char start, char end)
{
  for (char c = start; c <= end; c++) {
    if (check_flag(pArgData, c)) {
      printf("Flag \'%c\' set.\n", c);
      char *param = get_flag_parameter(pArgData, c);
      if (param)
        printf("-->Flag parameter: %s\n", param);
    }
  }
}

void iterate_long_flags(smb_ad *pArgData)
{
  smb_ll_iter iter = ll_get_iter(pArgData->long_flags);

  while (ll_iter_valid(&iter)) {
    DATA d = ll_iter_curr(&iter);
    char *string = d.data_ptr;
    printf("Flag \"%s\" set.\n", string);

    char *param = get_long_flag_parameter(pArgData, string);
    if (param)
      printf("-->Flag parameter: %s\n", param);
    ll_iter_next(&iter);
  }
}

void iterate_bare_strings(smb_ad *pArgData)
{
  smb_ll_iter iter = ll_get_iter(pArgData->bare_strings);

  while (ll_iter_valid(&iter)) {
    DATA d = ll_iter_curr(&iter);
    char *string = d.data_ptr;
    printf("String: \"%s\"\n", string);
    ll_iter_next(&iter);
  }
}

int args_test_main(int argc, char **argv)
{
  printf("Mallocs: %d\n", SMB_GET_MALLOC_COUNTER);
  smb_ad ArgData;
  arg_data_init(&ArgData);
  process_args(&ArgData, argc - 1, argv + 1);

  iterate_flags(&ArgData, 'A', 'Z');
  iterate_flags(&ArgData, 'a', 'z');

  iterate_long_flags(&ArgData);
  iterate_bare_strings(&ArgData);
  printf("Mallocs: %d\n", SMB_GET_MALLOC_COUNTER);
  arg_data_destroy(&ArgData);
  printf("Mallocs: %d\n", SMB_GET_MALLOC_COUNTER);
  return 0;
}
