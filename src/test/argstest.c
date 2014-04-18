/*******************************************************************************

  File:         argstest.c

  Author:       Stephen Brennan

  Date Created: Sunday,  3 November 2013

  Description: Tests the command line functions, using both a real world data
  example and example data sets.

*******************************************************************************/

#include "../libstephen.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Example main function that will process and print results of args analysis.
////////////////////////////////////////////////////////////////////////////////

void iterate_flags(struct smb_ad *pArgData, char start, char end)
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

void iterate_long_flags(struct smb_ad *pArgData)
{
  struct smb_ll_iter iter = ll_get_iter(pArgData->long_flags);

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

void iterate_bare_strings(struct smb_ad *pArgData)
{
  struct smb_ll_iter iter = ll_get_iter(pArgData->bare_strings);

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
  struct smb_ad ArgData;
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
