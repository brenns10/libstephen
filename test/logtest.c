/***************************************************************************//**

  @file         logtest.c

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Tests for logging (can't use smbunit though)

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>

#include "libstephen/log.h"
#include "libstephen/ut.h"

int test_levels(void)
{
  smb_status status = SMB_SUCCESS;
  sl_clear_handlers(NULL);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_DEBUG, .dst=stdout}, &status);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_INFO, .dst=stdout}, &status);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_WARNING, .dst=stdout}, &status);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_ERROR, .dst=stdout}, &status);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_CRITICAL,  .dst=stdout}, &status);
  LOG(NULL, LEVEL_NOTSET, "this appears 0 times");
  DDEBUG("this appears 1 time");
  DINFO("this appears 2 times");
  DWARNING("this appears 3 times");
  DERROR("this appears 4 times");
  DCRITICAL("this appears 5 times");
  sl_set_level(NULL, LEVEL_DEBUG);
  DDEBUG("this appears 5 times");
  LOG(NULL, LEVEL_DEBUG + 1, "this also appears 5 times, with level 11");
  return 0;
}

int test_override(void)
{
  smb_status status = SMB_SUCCESS;
  smb_logger *logger = sl_create();
  sl_clear_handlers(NULL);
  sl_add_handler(logger, (smb_loghandler){.level=LEVEL_INFO, .dst=stdout}, &status);
  DDEBUG("you shouldn't see this");
  DINFO("you shouldn't see this");
  sl_set_default_logger(logger);
  DDEBUG("you shouldn't see this");
  DINFO("you should see this");
  sl_set_default_logger(NULL);
  DDEBUG("you shouldn't see this");
  DINFO("you shouldn't see this");
  sl_delete(logger);
  return 0;
}

int test_too_many_levels(void)
{
  smb_status status = SMB_SUCCESS;
  sl_clear_handlers(NULL);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_DEBUG, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_INFO, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_WARNING, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_ERROR, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_CRITICAL,  .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_DEBUG, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_INFO, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_WARNING, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_ERROR, .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_CRITICAL,  .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  sl_add_handler(NULL, (smb_loghandler){.level=LEVEL_CRITICAL,  .dst=stdout}, &status);
  TEST_ASSERT(status == SMB_INDEX_ERROR);
  return 0;
}

void log_test(void)
{
  smb_ut_group *group = su_create_test_group("test/logtest.c");

  smb_ut_test *levels = su_create_test("levels", test_levels);
  su_add_test(group, levels);

  smb_ut_test *override = su_create_test("override", test_override);
  su_add_test(group, override);

  smb_ut_test *too_many_levels = su_create_test("too_many_levels", test_too_many_levels);
  su_add_test(group, too_many_levels);

  printf("ALERT: Some logging tests are manual!\n");
  su_run_group(group);
  su_delete_group(group);
}
