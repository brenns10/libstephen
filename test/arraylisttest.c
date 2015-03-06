/***************************************************************************//**

  @file         arraylisttest.c

  @author       Stephen Brennan

  @date         Created Saturday, 28 September 2013

  @brief        Test of the array list data structure.

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
