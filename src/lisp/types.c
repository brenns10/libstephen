#include <stdio.h>
#include <stdlib.h>

#include "libstephen/lisp.h"
#include "libstephen/ht.h"

static lisp_value *generic_eval(lisp_scope *s, lisp_value *v)
{
  (void)s;
  lisp_incref(v);
  return v;
}

static void nop_free(void *v)
{
  (void)v;
}

// type

static void type_print(FILE *f, lisp_value *v);
static lisp_value *type_new(void);

static lisp_type type_type_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="type",
  .print=type_print,
  .new=type_new,
  .eval=generic_eval,
  .free=free,
};
lisp_type *type_type = &type_type_obj;

static void type_print(FILE *f, lisp_value *v)
{
  lisp_type *value = (lisp_type*) v;
  fprintf(f, "%s", value->name);
}

static lisp_value *type_new(void)
{
  lisp_type *type = malloc(sizeof(lisp_type));
  type->refcount = 1;
  type->type = (lisp_value*)type_type;
  return (lisp_value*)type;
}

// list

static void list_print(FILE *f, lisp_value *v);
static lisp_value *list_new(void);

static lisp_type type_list_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="list",
  .print=list_print,
  .new=list_new,
  .eval=generic_eval,
  .free=free,
};
lisp_type *type_list = &type_list_obj;

static void list_print(FILE *f, lisp_value *v)
{
  lisp_list *list = (lisp_list*) v;
  fprintf(f, "(");
  lisp_print(f, list->left);
  fprintf(f, " . ");
  lisp_print(f, list->right);
  fprintf(f, ")");
}

static lisp_value *list_new(void)
{
  lisp_list *list = malloc(sizeof(lisp_list));
  list->refcount = 1;
  list->type = (lisp_value*) type_list;
  list->left = NULL;
  list->right = NULL;
  return (lisp_value*) list;
}

// symbol

static void symbol_print(FILE *f, lisp_value *v);
static lisp_value *symbol_new(void);
static void symbol_free(void *v);

static lisp_type type_symbol_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="symbol",
  .print=symbol_print,
  .new=symbol_new,
  .eval=generic_eval,
  .free=symbol_free,
};
lisp_type *type_symbol = &type_symbol_obj;

static void symbol_print(FILE *f, lisp_value *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  fprintf(f, "'%s'", symbol->sym);
}

static lisp_value *symbol_new(void)
{
  lisp_symbol *symbol = malloc(sizeof(lisp_symbol*));
  symbol->refcount = 1;
  symbol->type = (lisp_value*) type_symbol;
  symbol->sym = NULL;
  return (lisp_value*)symbol;
}

static void symbol_free(void *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  free(symbol->sym);
  free(symbol);
}

// integer

static void integer_print(FILE *f, lisp_value *v);
static lisp_value *integer_new(void);

static lisp_type type_integer_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="integer",
  .print=integer_print,
  .new=integer_new,
  .eval=generic_eval,
  .free=free,
};
lisp_type *type_integer = &type_integer_obj;

static void integer_print(FILE *f, lisp_value *v)
{
  lisp_integer *integer = (lisp_integer*) v;
  fprintf(f, "%d", integer->x);
}

static lisp_value *integer_new(void)
{
  lisp_integer *integer = malloc(sizeof(lisp_integer*));
  integer->refcount = 1;
  integer->type = (lisp_value*)type_integer;
  integer->x = 0;
  return (lisp_value*)integer;
}

// nil

static void nil_print(FILE *f, lisp_value *v);
static lisp_value *nil_new();

static lisp_type type_nil_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="nil",
  .print=nil_print,
  .new=nil_new,
  .eval=generic_eval,
  .free=nop_free,
};
lisp_type *type_nil = &type_nil_obj;

static lisp_nil lisp_nil_obj = {
  .type=(lisp_value*)&type_nil_obj,
  .refcount=1,
};
lisp_value *lisp_nilv = (lisp_value*) &lisp_nil_obj;

static void nil_print(FILE *f, lisp_value *v)
{
  (void)v;
  fprintf(f, "'()");
}

static lisp_value *nil_new(void)
{
  return lisp_nilv;
}

void lisp_print(FILE *f, lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  type->print(f, value);
}

void lisp_free(lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  type->free(value);
}

lisp_value *lisp_eval(lisp_scope *scope, lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  return type->eval(scope, value);
}

void lisp_incref(lisp_value *value)
{
  value->refcount++;
}

void lisp_decref(lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  value->refcount--;
  if (value->refcount <= 0) {
    type->free(value);
  }
}
