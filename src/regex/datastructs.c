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

/**
   @brief Initialize a regex_hit.
   @param obj The (pre-allocated) regex_hit.
   @param start The start location of the hit.
   @param length The length of the hit.
 */
void regex_hit_init(regex_hit *obj, int start, int length)
{
  // Initialization logic
  obj->start = start;
  obj->length = length;
}

/**
   @brief Allocate a regex_hit.
   @param start The start location of the hit.
   @param length The end location of the hit.
   @return Pointer to the regex_hit.  Must be freed with regex_hit_delete().
 */
regex_hit *regex_hit_create(int start, int length)
{
  regex_hit *obj = smb_new(regex_hit, 1);
  regex_hit_init(obj, start, length);
  return obj;
}

/**
   @brief Clean up a regex_hit object.
   @param obj The regex_hit object to clean up.
 */
void regex_hit_destroy(regex_hit *obj)
{
  // Cleanup logic (none)
}

/**
   @brief Clean up and free a regex_hit object.
   @param obj The regex_hit to free.
 */
void regex_hit_delete(regex_hit *obj) {
  regex_hit_destroy(obj);
  smb_free(obj);
}
