/***************************************************************************//**

  @file         argstest.c

  @author       Stephen Brennan

  @date         Created Sunday,  3 November 2013

  @brief        Tests argument parsing functions.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "libstephen/ad.h"
#include "libstephen/ll.h"
#include "libstephen/ut.h"


/**
   @brief Test memory allocation on the heap.
 */
int ad_test_heap(void)
{
  smb_ad *arg_data = arg_data_create();
  arg_data_delete(arg_data);
  return 0;
}

/**
   @brief Test memory allocation on the stack.
 */
int ad_test_stack(void)
{
  smb_ad ad;
  arg_data_init(&ad);
  arg_data_destroy(&ad);
  return 0;
}

char *basic_flags[] = {
  "-a",
  "-b",
  "-c",
  "-d",
  "-W",
  "-X",
  "-Y",
  "-Z"
};

/**
   @brief Test 'basic' single character, non grouped flags.
 */
int ad_test_basic_flags(void)
{
  smb_ad ad;
  arg_data_init(&ad);
  process_args(&ad, sizeof(basic_flags) / sizeof(char*), basic_flags);

  for (char c = 'a'; c <= 'z'; c++) {
    if (c <= 'd') {
      TEST_ASSERT(check_flag(&ad, c));
    } else {
      TEST_ASSERT(!check_flag(&ad, c));
    }
  }

  for (char c = 'A'; c <= 'Z'; c++) {
    if (c < 'W') {
      TEST_ASSERT(!check_flag(&ad, c));
    } else {
      TEST_ASSERT(check_flag(&ad, c));
    }
  }

  arg_data_destroy(&ad);
  return 0;
}

char *grouped_flags[] = {
  "-ab",
  "-c",
  "-WX",
  "-Y",
  "-dZ"
};

/**
   @brief Test single character grouped flags.
 */
int ad_test_grouped_flags(void)
{
  smb_ad ad;
  arg_data_init(&ad);
  process_args(&ad, sizeof(grouped_flags) / sizeof(char*), grouped_flags);

  for (char c = 'a'; c <= 'z'; c++) {
    if (c <= 'd') {
      TEST_ASSERT(check_flag(&ad, c));
    } else {
      TEST_ASSERT(!check_flag(&ad, c));
    }
  }

  for (char c = 'A'; c <= 'Z'; c++) {
    if (c < 'W') {
      TEST_ASSERT(!check_flag(&ad, c));
    } else {
      TEST_ASSERT(check_flag(&ad, c));
    }
  }

  arg_data_destroy(&ad);
  return 0;
}

char *flag_params[] = {
  "-a",
  "param_for_a",
  "-bc",
  "param_for_b",
  "-d"
};

/**
   @brief Test parameters for single character flags.
 */
int ad_test_flag_params(void)
{
  smb_ad ad;
  char *str;
  arg_data_init(&ad);
  process_args(&ad, sizeof(flag_params) / sizeof(char*), flag_params);

  TEST_ASSERT(check_flag(&ad, 'a'));
  TEST_ASSERT(check_flag(&ad, 'b'));
  TEST_ASSERT(check_flag(&ad, 'c'));
  TEST_ASSERT(check_flag(&ad, 'd'));

  str = get_flag_parameter(&ad, 'a');
  TEST_ASSERT(str && strncmp(str, flag_params[1], strlen(flag_params[1])) == 0);

  str = get_flag_parameter(&ad, 'c');
  TEST_ASSERT(str && strncmp(str, flag_params[3], strlen(flag_params[3])) == 0);

  str = get_flag_parameter(&ad, 'b');
  TEST_ASSERT(str == NULL);

  str = get_flag_parameter(&ad, 'd');
  TEST_ASSERT(str == NULL);

  arg_data_destroy(&ad);
  return 0;
}

char *long_flags[] = {
  "--this-is-a-long-flag",
  "--this-is-another-long-flag"
};

/**
   @brief Test long flags.
 */
int ad_test_long_flags(void)
{
  smb_ad ad;
  arg_data_init(&ad);
  process_args(&ad, sizeof(long_flags) / sizeof(char*), long_flags);

  TEST_ASSERT(check_long_flag(&ad, "this-is-a-long-flag"));
  TEST_ASSERT(check_long_flag(&ad, "this-is-another-long-flag"));
  TEST_ASSERT(!check_long_flag(&ad, "this-was-not-a-long-flag"));

  arg_data_destroy(&ad);
  return 0;
}

char *long_params[] = {
  "--long-flag1",
  "long param 1",
  "--long-flag2",
  "--long-flag3",
  "long param 3"
};

/**
   @brief Test long flag parameters.
 */
int ad_test_long_params(void)
{
  smb_ad ad;
  char *str;
  arg_data_init(&ad);
  process_args(&ad, sizeof(long_params) / sizeof(char*), long_params);

  TEST_ASSERT(check_long_flag(&ad, "long-flag1"));
  TEST_ASSERT(check_long_flag(&ad, "long-flag2"));
  TEST_ASSERT(check_long_flag(&ad, "long-flag3"));

  str = get_long_flag_parameter(&ad, "long-flag1");
  TEST_ASSERT(str && strncmp(str, long_params[1], strlen(long_params[1])) == 0);

  str = get_long_flag_parameter(&ad, "long-flag3");
  TEST_ASSERT(str && strncmp(str, long_params[4], strlen(long_params[4])) == 0);

  str = get_long_flag_parameter(&ad, "long-flag2");
  TEST_ASSERT(str == NULL);

  arg_data_destroy(&ad);
  return 0;
}

char *bare_strings[] = {
  "bs0",
  "-a",
  "param_a",
  "bs1",
  "--blah",
  "param_blah",
  "bs2",
  "-"
};

/**
   @brief Test whether bare strings work.
 */
int ad_test_bare_strings(void)
{
  smb_ad ad;
  char *str;
  arg_data_init(&ad);
  process_args(&ad, sizeof(bare_strings) / sizeof(char*), bare_strings);

  TEST_ASSERT(check_flag(&ad, 'a'));
  TEST_ASSERT(check_long_flag(&ad, "blah"));
  TEST_ASSERT(check_bare_string(&ad, "bs0"));
  TEST_ASSERT(check_bare_string(&ad, "bs1"));
  TEST_ASSERT(check_bare_string(&ad, "bs2"));
  TEST_ASSERT(check_bare_string(&ad, "-"));
  TEST_ASSERT(ll_length(ad.bare_strings) == 4);

  str = get_flag_parameter(&ad, 'a');
  TEST_ASSERT(str && strncmp(str, bare_strings[2], strlen(bare_strings[2])) == 0);

  str = get_long_flag_parameter(&ad, "blah");
  TEST_ASSERT(str && strncmp(str, bare_strings[5], strlen(bare_strings[5])) == 0);

  arg_data_destroy(&ad);
  return 0;
}

void args_test(void)
{
  smb_ut_group *group = su_create_test_group("args");

  smb_ut_test *heap = su_create_test("heap", &ad_test_heap);
  su_add_test(group, heap);

  smb_ut_test *stack = su_create_test("stack", &ad_test_stack);
  su_add_test(group, stack);

  smb_ut_test *basic_flags = su_create_test("basic_flags", &ad_test_basic_flags);
  su_add_test(group, basic_flags);

  smb_ut_test *grouped_flags = su_create_test("grouped_flags",
                                              &ad_test_grouped_flags);
  su_add_test(group, grouped_flags);

  smb_ut_test *flag_params = su_create_test("flag_params", &ad_test_flag_params);
  su_add_test(group, flag_params);

  smb_ut_test *long_flags = su_create_test("long_flags", &ad_test_long_flags);
  su_add_test(group, long_flags);

  smb_ut_test *long_params = su_create_test("long_params", &ad_test_long_params);
  su_add_test(group, long_params);

  smb_ut_test *bare_strings = su_create_test("bare_strings",
                                             &ad_test_bare_strings);
  su_add_test(group, bare_strings);

  su_run_group(group);
  su_delete_group(group);
}

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
  DATA d;
  smb_iter iter = ll_get_iter(pArgData->long_flags);
  smb_status status;

  while (iter.has_next(&iter)) {
    d = iter.next(&iter, &status);
    assert(status == SMB_SUCCESS);
    char *string = d.data_ptr;
    printf("Flag \"%s\" set.\n", string);

    char *param = get_long_flag_parameter(pArgData, string);
    if (param)
      printf("-->Flag parameter: %s\n", param);
  }
  iter.destroy(&iter);
}

void iterate_bare_strings(smb_ad *pArgData)
{
  DATA d;
  smb_iter iter = ll_get_iter(pArgData->bare_strings);
  smb_status status;

  while (iter.has_next(&iter)) {
    d = iter.next(&iter, &status);
    assert(status == SMB_SUCCESS);
    char *string = d.data_ptr;
    printf("String: \"%s\"\n", string);
  }
  iter.destroy(&iter);
}

int args_test_main(int argc, char **argv)
{
  smb_ad ArgData;
  arg_data_init(&ArgData);
  process_args(&ArgData, argc - 1, argv + 1);

  iterate_flags(&ArgData, 'A', 'Z');
  iterate_flags(&ArgData, 'a', 'z');

  iterate_long_flags(&ArgData);
  iterate_bare_strings(&ArgData);
  arg_data_destroy(&ArgData);
  return 0;
}
