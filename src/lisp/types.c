#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libstephen/lisp.h"
#include "libstephen/ht.h"

// General functions for types.

static lisp_value *eval_error(lisp_scope *s, lisp_value *v)
{
  (void)s;
  return (lisp_value*) lisp_error_new("cannot evaluate this object");
}

static lisp_value *eval_same(lisp_scope *s, lisp_value *v)
{
  (void)s;
  lisp_incref(v);
  return v;
}

static lisp_value *call_error(lisp_scope *s, lisp_value *c, lisp_value *v)
{
  (void)s;
  (void)c;
  (void)v;
  return (lisp_value*) lisp_error_new("not callable!");
}

static lisp_value *call_same(lisp_scope *s, lisp_value *c, lisp_value *v)
{
  (void)s;
  (void)v;
  lisp_incref(c);
  return c;
}

static void nop_free(void *v)
{
  (void)v;
}

// type

static void type_print(FILE *f, lisp_value *v);
static lisp_value *type_new(void);

static lisp_type type_type_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="type",
  .print=type_print,
  .new=type_new,
  .eval=eval_error,
  .free=free,
  .call=call_error,
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
  type->type = type;
  return (lisp_value*)type;
}

// scope

static void scope_print(FILE *f, lisp_value*v);
static lisp_value *scope_new(void);
static void scope_free(void *v);

static lisp_type type_scope_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="scope",
  .print=scope_print,
  .new=scope_new,
  .eval=eval_error,
  .free=scope_free,
  .call=call_error,
};
lisp_type *type_scope = &type_scope_obj;

static unsigned int symbol_hash(DATA symbol)
{
  lisp_symbol *sym = symbol.data_ptr;
  return ht_string_hash(PTR(sym->sym));
}

static int symbol_compare(DATA d1, DATA d2)
{
  lisp_symbol *sym1 = d1.data_ptr;
  lisp_symbol *sym2 = d2.data_ptr;
  return data_compare_string(PTR(sym1->sym), PTR(sym2->sym));;
}

static lisp_value *scope_new(void)
{
  lisp_scope *scope = malloc(sizeof(lisp_scope));
  scope->type = type_scope;
  scope->refcount = 1;
  ht_init(&scope->scope, symbol_hash, symbol_compare);
  return (lisp_value*)scope;
}

static void scope_free(void *v)
{
  lisp_scope *scope = (lisp_scope*) v;
  smb_iter it = ht_get_iter(&scope->scope);
  while (it.has_next(&it)) {
    smb_status status = SMB_SUCCESS;
    lisp_value *key = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_value *value = ht_get(&scope->scope, PTR(key), &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_decref(key);
    lisp_decref(value);
  }
  ht_destroy(&scope->scope);
  free(scope);
}

static void scope_print(FILE *f, lisp_value *v)
{
  lisp_scope *scope = (lisp_scope*) v;
  smb_iter it = ht_get_iter(&scope->scope);
  fprintf(f, "(scope:");
  while (it.has_next(&it)) {
    smb_status status = SMB_SUCCESS;
    lisp_value *key = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_value *value = ht_get(&scope->scope, PTR(key), &status).data_ptr;
    assert(status == SMB_SUCCESS);
    fprintf(f, " ");
    lisp_print(f, key);
    fprintf(f, ": ");
    lisp_print(f, value);
  }
  fprintf(f, ")");
}

// list

static void list_print(FILE *f, lisp_value *v);
static lisp_value *list_new(void);
static void list_free(void *);
static lisp_value *list_eval(lisp_scope *scope, lisp_value *list);

static lisp_type type_list_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="list",
  .print=list_print,
  .new=list_new,
  .eval=list_eval,
  .free=list_free,
  .call=call_error,
};
lisp_type *type_list = &type_list_obj;

lisp_list lisp_nilv_obj = {
  .type=&type_list_obj,
  .refcount=1,
  .left=NULL,
  .right=NULL,
};
lisp_list *lisp_nilv = &lisp_nilv_obj;

static lisp_value *list_eval(lisp_scope *scope, lisp_value *v)
{
  lisp_list *list = (lisp_list*) v;
  if (list->right->type != type_list) {
    return (lisp_value*) lisp_error_new("bad function call syntax");
  }
  lisp_value *callable = lisp_eval(scope, list->left);
  return lisp_call(scope, callable, list->right);
}

static void list_print_internal(FILE *f, lisp_list *list)
{
  if (list == lisp_nilv) {
    return;
  }
  lisp_print(f, list->left);
  if (list->right->type != type_list) {
    fprintf(f, " . ");
    lisp_print(f, list->right);
    return;
  } else {
    fprintf(f, " ");
    list_print_internal(f, (lisp_list*)list->right);
  }
}

static void list_print(FILE *f, lisp_value *v)
{
  fprintf(f, "(");
  list_print_internal(f, (lisp_list*)v);
  fprintf(f, ")");
}

static lisp_value *list_new(void)
{
  lisp_list *list = malloc(sizeof(lisp_list));
  list->refcount = 1;
  list->type = type_list;
  list->left = NULL;
  list->right = NULL;
  return (lisp_value*) list;
}

static void list_free(void *l)
{
  lisp_list *list = (lisp_list*) l;
  lisp_decref(list->left);
  lisp_decref(list->right);
  free(l);
}

bool lisp_nil_p(lisp_value *l)
{
  return (l->type == type_list) && ((lisp_list*)l == lisp_nilv);
}

// symbol

static void symbol_print(FILE *f, lisp_value *v);
static lisp_value *symbol_new(void);
static lisp_value *symbol_eval(lisp_scope *scope, lisp_value *value);
static void symbol_free(void *v);

static lisp_type type_symbol_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="symbol",
  .print=symbol_print,
  .new=symbol_new,
  .eval=symbol_eval,
  .free=symbol_free,
  .call=call_error,
};
lisp_type *type_symbol = &type_symbol_obj;

static void symbol_print(FILE *f, lisp_value *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  fprintf(f, "%s", symbol->sym);
}

static lisp_value *symbol_new(void)
{
  lisp_symbol *symbol = malloc(sizeof(lisp_symbol));
  symbol->refcount = 1;
  symbol->type = type_symbol;
  symbol->sym = NULL;
  return (lisp_value*)symbol;
}

static lisp_value *symbol_eval(lisp_scope *scope, lisp_value *value)
{
  lisp_symbol *symbol = (lisp_symbol*) value;
  return lisp_scope_lookup(scope, symbol);
}

static void symbol_free(void *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  free(symbol->sym);
  free(symbol);
}

// error

static void error_print(FILE *f, lisp_value *v);
static lisp_value *error_new(void);
static void error_free(void *v);

static lisp_type type_error_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="error",
  .print=error_print,
  .new=error_new,
  .eval=eval_same,
  .free=error_free,
  .call=call_same,
};
lisp_type *type_error = &type_error_obj;

static void error_print(FILE *f, lisp_value *v)
{
  lisp_error *error = (lisp_error*) v;
  fprintf(f, "error: %s", error->message);
}

static lisp_value *error_new(void)
{
  lisp_error *error = malloc(sizeof(lisp_error));
  error->refcount = 1;
  error->type = type_error;
  error->message = NULL;
  return (lisp_value*)error;
}

static void error_free(void *v)
{
  lisp_error *error = (lisp_error*) v;
  free(error->message);
  free(error);
}

// integer

static void integer_print(FILE *f, lisp_value *v);
static lisp_value *integer_new(void);

static lisp_type type_integer_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="integer",
  .print=integer_print,
  .new=integer_new,
  .eval=eval_same,
  .free=free,
  .call=call_error,
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
  integer->type = type_integer;
  integer->x = 0;
  return (lisp_value*)integer;
}

// builtin

static void builtin_print(FILE *f, lisp_value *v);
static lisp_value *builtin_new(void);
static lisp_value *builtin_call(lisp_scope *scope, lisp_value *c,
                                lisp_value *arguments);

static lisp_type type_builtin_obj = {
  .type=&type_type_obj,
  .refcount=1,
  .name="builtin",
  .print=builtin_print,
  .new=builtin_new,
  .eval=eval_error,
  .free=free,
  .call=builtin_call,
};
lisp_type *type_builtin = &type_builtin_obj;

static void builtin_print(FILE *f, lisp_value *v)
{
  lisp_builtin *builtin = (lisp_builtin*) v;
  fprintf(f, "<builtin function %s>", builtin->name);
}

static lisp_value *builtin_new()
{
  lisp_builtin *builtin = malloc(sizeof(lisp_builtin));
  builtin->refcount = 1;
  builtin->type = type_builtin;
  builtin->call = NULL;
  builtin->name = NULL;
  return (lisp_value*) builtin;
}

static lisp_value *builtin_call(lisp_scope *scope, lisp_value *c,
                                lisp_value *arguments)
{
  lisp_builtin *builtin = (lisp_builtin*) c;
  return builtin->call(scope, arguments);
}

// Shortcuts for type objects.

void lisp_print(FILE *f, lisp_value *value)
{
  value->type->print(f, value);
}

void lisp_free(lisp_value *value)
{
  value->type->free(value);
}

lisp_value *lisp_eval(lisp_scope *scope, lisp_value *value)
{
  return value->type->eval(scope, value);
}

lisp_value *lisp_call(lisp_scope *scope, lisp_value *callable, lisp_value *args)
{
  if (callable->type == type_error) {
    return callable;
  }

  return callable->type->call(scope, callable, args);
}

lisp_value *lisp_incref(lisp_value *value)
{
  value->refcount++;
  return value;
}

void lisp_decref(lisp_value *value)
{
  value->refcount--;
  if (value->refcount <= 0) {
    value->type->free(value);
  }
}
