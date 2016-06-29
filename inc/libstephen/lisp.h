/***************************************************************************//**

  @file         libstephen/lisp.h

  @author       Stephen Brennan

  @date         Created Monday, 27 June 2016

  @brief        Lisp implementation for embedding.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef SMB_LIBSTEPHEN_LISP_H
#define SMB_LIBSTEPHEN_LISP_H

#include <stdio.h>

#include "libstephen/ht.h"

#define LISP_VALUE_HEAD             \
  struct {                          \
    const struct lisp_value *type;  \
    unsigned int refcount;          \
  }

typedef struct lisp_value {
  LISP_VALUE_HEAD;
} lisp_value;

typedef struct lisp_scope {
  LISP_VALUE_HEAD;
  smb_ht scope;
} lisp_scope;

typedef struct {
  LISP_VALUE_HEAD;
  const char *name;
  void (*print)(FILE *f, lisp_value *value);
  lisp_value * (*new)(void);
  void (*free)(void *value);
  lisp_value * (*eval)(lisp_scope *scope, lisp_value *value);
} lisp_type;

typedef struct {
  LISP_VALUE_HEAD;
  lisp_value *left;
  lisp_value *right;
} lisp_list;

typedef struct {
  LISP_VALUE_HEAD;
} lisp_nil;

typedef struct {
  LISP_VALUE_HEAD;
  char *sym;
} lisp_symbol;

typedef struct {
  LISP_VALUE_HEAD;
  int x;
} lisp_integer;

void lisp_print(FILE *f, lisp_value *value);
void lisp_free(lisp_value *value);
lisp_value *lisp_eval(lisp_scope *scope, lisp_value *value);
void lisp_incref(lisp_value *value);
void lisp_decref(lisp_value *value);
lisp_value *lisp_parse(char *input);

extern lisp_value *lisp_nilv;

extern lisp_type *type_type;
extern lisp_type *type_list;
extern lisp_type *type_symbol;
extern lisp_type *type_integer;
extern lisp_type *type_nil;

#endif//SMB_LIBSTEPHEN_LISP_H
