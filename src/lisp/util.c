#include <stdarg.h>
#include <string.h>

#include "libstephen/lisp.h"

void lisp_scope_bind(lisp_scope *scope, lisp_symbol *symbol, lisp_value *value)
{
  ht_insert(&scope->scope, PTR(symbol), PTR(value));
}

lisp_value *lisp_scope_lookup(lisp_scope *scope, lisp_symbol *symbol)
{
  smb_status status = SMB_SUCCESS;
  lisp_value *v = ht_get(&scope->scope, PTR(symbol), &status).data_ptr;
  if (status == SMB_NOT_FOUND_ERROR) {
    if (scope->up) {
      return lisp_scope_lookup(scope->up, symbol);
    } else {
      return (lisp_value*)lisp_error_new("symbol not found in scope");
    }
  } else {
    return v;
  }
}

static void lisp_scope_add_builtin(lisp_scope *scope, char *name, lisp_value * (*call)(lisp_scope*,lisp_value*))
{
  lisp_symbol *symbol = lisp_symbol_new(name);
  lisp_builtin *builtin = lisp_builtin_new(name, call);
  lisp_scope_bind(scope, symbol, (lisp_value*)builtin);
}

lisp_symbol *lisp_symbol_new(char *sym)
{
  lisp_symbol *err = (lisp_symbol*)type_symbol->new();
  int len = strlen(sym);
  err->sym = malloc(len + 1);
  strncpy(err->sym, sym, len);
  err->sym[len] = '\0';
  return err;
}

lisp_error *lisp_error_new(char *message)
{
  lisp_error *err = (lisp_error*)type_error->new();
  int len = strlen(message);
  err->message = malloc(len + 1);
  strncpy(err->message, message, len);
  err->message[len] = '\0';
  return err;
}

lisp_builtin *lisp_builtin_new(char *name, lisp_value *(*call)(lisp_scope *, lisp_value *))
{
  lisp_builtin *builtin = (lisp_builtin*)type_builtin->new();
  builtin->call = call;
  builtin->name = name;
  return builtin;
}

lisp_value *lisp_nil_new()
{
  return lisp_incref((lisp_value*)lisp_nilv);
}

lisp_value *lisp_eval_list(lisp_scope *scope, lisp_value *l)
{
  if (lisp_nil_p(l)) {
    return lisp_incref(l);
  }
  lisp_list *list = (lisp_list*) l;
  lisp_list *result = (lisp_list*) type_list->new();
  result->left = lisp_eval(scope, list->left);
  result->right = lisp_eval_list(scope, list->right);
  return (lisp_value*) result;
}

/**
   Given a list of arguments to a function, check for errors. If one exists,
   return it.  Otherwise, return null.
 */
lisp_value *lisp_find_errors(lisp_value *l)
{
  while (l->type == type_list && l != (lisp_value*)lisp_nilv) {
    lisp_list *list = (lisp_list *) l;
    if (list->left->type == type_error) {
      return list->left;
    }
    l = list->right;
  }
  return NULL;
}

int lisp_list_length(lisp_list *list)
{
  int length = 0;
  while (list->type == type_list && !lisp_nil_p((lisp_value*)list)) {
    length++;
    list = (lisp_list*)list->right;
  }
  return length;
}

static lisp_type *lisp_get_type(char c)
{
  switch (c) {
  case 'd':
    return type_integer;
  case 'l':
    return type_list;
  case 's':
    return type_symbol;
  case 'o':
    return type_scope;
  case 'e':
    return type_error;
  case 'b':
    return type_builtin;
  case 't':
    return type_type;
  }
  return NULL;
}

bool lisp_get_args(lisp_list *list, char *format, ...)
{
  va_list va;
  va_start(va, format);
  lisp_value **v;
  while (list != lisp_nilv && *format != '\0') {
    lisp_type *type = lisp_get_type(*format);
    if (type != NULL && type != list->left->type) {
      return false;
    }
    v = va_arg(va, lisp_value**);
    *v = list->left;
    list = (lisp_list*)list->right;
    format += 1;
  }
  if (strlen(format) != 0 || list != lisp_nilv) {
    return false;
  }
  return true;
}

static lisp_value *lisp_builtin_eval(lisp_scope *scope, lisp_value *arguments)
{
  lisp_list *evald = (lisp_list*)lisp_eval_list(scope, arguments);
  lisp_value *result = lisp_eval(scope, evald->left);
  lisp_decref((lisp_value*)evald);
  return result;
}

static lisp_value *lisp_builtin_car(lisp_scope *scope, lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    lisp_decref((lisp_value*)arglist);
    return (lisp_value*)lisp_error_new("wrong arguments to car");
  }
  if (lisp_list_length(firstarg) == 0) {
    lisp_decref((lisp_value*)arglist);
    return (lisp_value*)lisp_error_new("expected at least one item");
  }
  // save rv because firstarg may be deleted after decref
  lisp_value *rv = lisp_incref(firstarg->left);
  lisp_decref((lisp_value*)arglist);
  return rv;
}

static lisp_value *lisp_builtin_cdr(lisp_scope *scope, lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    lisp_decref((lisp_value*)arglist);
    return (lisp_value*) lisp_error_new("wrong arguments to cdr");
  }
  // save rv because firstarg may be deleted after decref
  lisp_value *rv = lisp_incref(firstarg->right);
  lisp_decref((lisp_value*)arglist);
  return rv;
}

static lisp_value *lisp_builtin_quote(lisp_scope *scope, lisp_value *a)
{
  (void)scope;
  lisp_value *firstarg;
  lisp_list *arglist = (lisp_list*) a;
  if (!lisp_get_args(arglist, "*", &firstarg)) {
    lisp_decref((lisp_value*)arglist);
    return (lisp_value*) lisp_error_new("wrong arguments to quote");
  }
  return lisp_incref(arglist->left);
}

static lisp_value *lisp_builtin_cons(lisp_scope *scope, lisp_value *a)
{
  lisp_value *a1;
  lisp_list *l;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  if (!lisp_get_args(arglist, "*l", &a1, &l)) {
    lisp_decref((lisp_value*)arglist);
    return (lisp_value*) lisp_error_new("wrong arguments to cons");
  }
  lisp_list *new = (lisp_list*) type_list->new();
  new->left = a1;
  lisp_incref(a1);
  new->right = (lisp_value*)l;
  lisp_incref((lisp_value*)l);
  lisp_decref((lisp_value*)arglist);
  return (lisp_value*)new;
}

void lisp_scope_populate_builtins(lisp_scope *scope)
{
  lisp_scope_add_builtin(scope, "eval", lisp_builtin_eval);
  lisp_scope_add_builtin(scope, "car", lisp_builtin_car);
  lisp_scope_add_builtin(scope, "cdr", lisp_builtin_cdr);
  lisp_scope_add_builtin(scope, "quote", lisp_builtin_quote);
  lisp_scope_add_builtin(scope, "cons", lisp_builtin_cons);
}
