/***************************************************************************//**

  @file         iter.c

  @author       Stephen Brennan

  @date         Created Monday,  4 August 2014

  @brief        Libstephen: Iterator Operations

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

#include <stdio.h>
#include <assert.h>

#include "libstephen/base.h"
#include "libstephen/list.h"

/**
   @brief Prints anything with an iterator.
   @param it The iterator.  Will be invalidated after this use.
   @param f The file to print to.
   @param printer The printer for handling DATA objects.
 */
void iter_print(smb_iter it, FILE *f, DATA_PRINTER printer)
{
  DATA d;
  smb_status status;
  fprintf(f, "smb_iter {\n");
  while (it.has_next(&it)) {
    d = it.next(&it, &status);
    // used has_next
    assert(status == SMB_SUCCESS);
    printer(f, d);
    fprintf(f, ",\n");
  }
  it.destroy(&it);
  fprintf(f, "}\n");
}
