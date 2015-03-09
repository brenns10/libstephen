/***************************************************************************//**

  @file         arraylisttest.c

  @author       Stephen Brennan

  @date         Created Saturday, 28 September 2013

  @brief        Test of the array list data structure.

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include "libstephen/al.h"
#include "libstephen/ut.h"
#include "tests.h"

////////////////////////////////////////////////////////////////////////////////
// TESTS

int al_test_create()
{
  DATA d;
  smb_status status = SMB_SUCCESS;
  d.data_llint = 13;

  smb_al *list = al_create();
  al_append(list, d);

  TEST_ASSERT(al_length(list) == 1);

  TEST_ASSERT(al_get(list, 0, &status).data_llint == 13);
  TEST_ASSERT(status == SMB_SUCCESS);

  al_delete(list);
  return 0;
}

int al_test_create_empty()
{
  smb_al *list = al_create();
  TEST_ASSERT(al_length(list) == 0);
  al_delete(list);
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// TEST LOADER AND RUNNER

void array_list_test()
{
  smb_ut_group *group = su_create_test_group("array list");

  smb_ut_test *create = su_create_test("create", al_test_create);
  su_add_test(group, create);

  smb_ut_test *create_empty = su_create_test("create_empty", al_test_create_empty);
  su_add_test(group, create_empty);

  su_run_group(group);
  su_delete_group(group);
}
