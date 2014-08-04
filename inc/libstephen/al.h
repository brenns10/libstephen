/***************************************************************************//**

  @file         al.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Array List

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

#ifndef LIBSTEPHEN_AL_H
#define LIBSTEPHEN_AL_H

#include "base.h"  /* DATA     */
#include "list.h"  /* smb_list */

/**
   @brief The actual array list data type.

   "Bare" functions return a pointer to this structure.  You should not use any
   of the members, as they are implementation specific and subject to change.
 */
typedef struct smb_al
{
  /**
     @brief The area of memory containing the data.
   */
  DATA *data;

  /**
     @brief The number of items in the list.
   */
  int length;

  /**
     @brief The space allocated for the list.
   */
  int allocated;

} smb_al;

void al_init(smb_al *list);
smb_al *al_create();
void al_destroy(smb_al *list);
void al_delete(smb_al *list);

void al_append(smb_al *list, DATA newData);
void al_prepend(smb_al *list, DATA newData);
DATA al_get(const smb_al *list, int index);
void al_remove(smb_al *list, int index);
void al_insert(smb_al *list, int index, DATA newData);
void al_set(smb_al *list, int index, DATA newData);
void al_push_back(smb_al *list, DATA newData);
DATA al_pop_back(smb_al *list);
DATA al_peek_back(smb_al *list);
void al_push_front(smb_al *list, DATA newData);
DATA al_pop_front(smb_al *list);
DATA al_peek_front(smb_al *list);
int al_length(const smb_al *list);
int al_index_of(const smb_al *list, DATA d);
smb_iter al_get_iter(const smb_al *list);

#endif // LIBSTEPHEN_AL_H
