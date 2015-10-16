/***************************************************************************//**

  @file         regex/datastructs.c

  @author       Stephen Brennan

  @date         Created Saturday, 26 July 2014

  @brief        Regular expression data structure definitions.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include "libstephen/base.h"
#include "libstephen/regex.h"   // functions we're implementing

void regex_hit_init(regex_hit *obj, int start, int length)
{
  // Initialization logic
  obj->start = start;
  obj->length = length;
}

regex_hit *regex_hit_create(int start, int length)
{
  regex_hit *obj = smb_new(regex_hit, 1);
  regex_hit_init(obj, start, length);
  return obj;
}

void regex_hit_destroy(regex_hit *obj)
{
  (void)obj; // unused
  // Cleanup logic (none)
}

void regex_hit_delete(regex_hit *obj) {
  regex_hit_destroy(obj);
  smb_free(obj);
}
