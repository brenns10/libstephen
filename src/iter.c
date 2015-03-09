/***************************************************************************//**

  @file         iter.c

  @author       Stephen Brennan

  @date         Created Monday,  4 August 2014

  @brief        Libstephen: Iterator Operations

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

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
