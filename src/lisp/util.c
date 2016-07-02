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

lisp_value *lisp_eval_list(lisp_scope *scope, lisp_value *l)
{
  if (l == lisp_nilv) {
    lisp_incref(l);
    return l;
  }
  lisp_list *list = (lisp_list*) l;
  lisp_list *result = (lisp_list*) type_list->new();
  result->left = lisp_eval(scope, list->left);
  result->right = lisp_eval_list(scope, list->right);
  return (lisp_value*) result;
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
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  lisp_list *firstarg = (lisp_list*) arglist->left;
  lisp_incref(firstarg->left);
  lisp_decref((lisp_value*)arglist);
  return firstarg->left;
}

static lisp_value *lisp_builtin_cdr(lisp_scope *scope, lisp_value *a)
{
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  lisp_list *firstarg = (lisp_list*) arglist->left;
  lisp_incref(firstarg->right);
  lisp_decref((lisp_value*)arglist);
  return firstarg->right;
}

static lisp_value *lisp_builtin_quote(lisp_scope *scope, lisp_value *a)
{
  (void)scope;
  lisp_list *arglist = (lisp_list*) a;
  lisp_incref(arglist->left);
  return arglist->left;
}

void lisp_scope_populate_builtins(lisp_scope *scope)
{
  lisp_scope_add_builtin(scope, "eval", lisp_builtin_eval);
  lisp_scope_add_builtin(scope, "car", lisp_builtin_car);
  lisp_scope_add_builtin(scope, "cdr", lisp_builtin_cdr);
  lisp_scope_add_builtin(scope, "quote", lisp_builtin_quote);
}
