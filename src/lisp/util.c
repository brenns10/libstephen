#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "libstephen/lisp.h"

void lisp_scope_bind(lisp_scope *scope, lisp_symbol *symbol, lisp_value *value)
{
  ht_insert(&scope->scope, PTR(symbol), PTR(value));
}

lisp_value *lisp_scope_lookup(lisp_runtime *rt, lisp_scope *scope,
                              lisp_symbol *symbol)
{
  smb_status status = SMB_SUCCESS;
  lisp_value *v = ht_get(&scope->scope, PTR(symbol), &status).data_ptr;
  if (status == SMB_NOT_FOUND_ERROR) {
    if (scope->up) {
      return lisp_scope_lookup(rt, scope->up, symbol);
    } else {
      return (lisp_value*)lisp_error_new(rt, "symbol not found in scope");
    }
  } else {
    return v;
  }
}

void lisp_scope_add_builtin(lisp_runtime *rt, lisp_scope *scope, char *name,
                            lisp_builtin_func call)
{
  lisp_symbol *symbol = lisp_symbol_new(rt, name);
  lisp_builtin *builtin = lisp_builtin_new(rt, name, call);
  lisp_scope_bind(scope, symbol, (lisp_value*)builtin);
}

void lisp_scope_replace_or_insert(lisp_scope *scope, lisp_symbol *key, lisp_value *value)
{
  lisp_scope *s = scope;

  // First go up the chain checking for the name.
  while (s) {
    if (ht_contains(&s->scope, PTR(key))) {
      // If we find it, replace it.
      ht_insert(&s->scope, PTR(key), PTR(value));
      return;
    }
    s = s->up;
  }

  // If we never find it, insert it in the "lowest" scope.
  ht_insert(&scope->scope, PTR(key), PTR(value));
}

lisp_symbol *lisp_symbol_new(lisp_runtime *rt, char *sym)
{
  lisp_symbol *err = (lisp_symbol*)lisp_new(rt, type_symbol);
  int len = strlen(sym);
  err->sym = malloc(len + 1);
  strncpy(err->sym, sym, len);
  err->sym[len] = '\0';
  return err;
}

lisp_error *lisp_error_new(lisp_runtime *rt, char *message)
{
  lisp_error *err = (lisp_error*)lisp_new(rt, type_error);
  int len = strlen(message);
  err->message = malloc(len + 1);
  strncpy(err->message, message, len);
  err->message[len] = '\0';
  return err;
}

lisp_builtin *lisp_builtin_new(lisp_runtime *rt, char *name,
                               lisp_builtin_func call)
{
  lisp_builtin *builtin = (lisp_builtin*)lisp_new(rt, type_builtin);
  builtin->call = call;
  builtin->name = name;
  return builtin;
}

lisp_value *lisp_nil_new(lisp_runtime *rt)
{
  if (rt->nil == NULL) {
    rt->nil = lisp_new(rt, type_list);
  }
  return rt->nil;
}

lisp_value *lisp_eval_list(lisp_runtime *rt, lisp_scope *scope, lisp_value *l)
{
  if (lisp_nil_p(l)) {
    return l;
  }
  lisp_list *list = (lisp_list*) l;
  lisp_list *result = (lisp_list*)lisp_new(rt, type_list);
  result->left = lisp_eval(rt, scope, list->left);
  result->right = lisp_eval_list(rt, scope, list->right);
  return (lisp_value*) result;
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
  case 'S':
    return type_string;
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
  while (!lisp_nil_p((lisp_value*)list) && *format != '\0') {
    lisp_type *type = lisp_get_type(*format);
    if (type != NULL && type != list->left->type) {
      return false;
    }
    v = va_arg(va, lisp_value**);
    *v = list->left;
    list = (lisp_list*)list->right;
    format += 1;
  }
  if (strlen(format) != 0 || !lisp_nil_p((lisp_value*)list)) {
    return false;
  }
  return true;
}

static lisp_value *lisp_builtin_eval(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *arguments)
{
  lisp_list *evald = (lisp_list*)lisp_eval_list(rt, scope, arguments);
  lisp_value *result = lisp_eval(rt, scope, evald->left);
  return result;
}

static lisp_value *lisp_builtin_car(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    return (lisp_value*)lisp_error_new(rt, "wrong arguments to car");
  }
  if (lisp_list_length(firstarg) == 0) {
    return (lisp_value*)lisp_error_new(rt, "expected at least one item");
  }
  return firstarg->left;
}

static lisp_value *lisp_builtin_cdr(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to cdr");
  }
  // save rv because firstarg may be deleted after decref
  return firstarg->right;
}

static lisp_value *lisp_builtin_quote(lisp_runtime *rt, lisp_scope *scope,
                                      lisp_value *a)
{
  (void)scope;
  lisp_value *firstarg;
  lisp_list *arglist = (lisp_list*) a;
  if (!lisp_get_args(arglist, "*", &firstarg)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to quote");
  }
  return arglist->left;
}

static lisp_value *lisp_builtin_cons(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *a)
{
  lisp_value *a1;
  lisp_value *l;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "**", &a1, &l)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to cons");
  }
  lisp_list *new = (lisp_list*)lisp_new(rt, type_list);
  new->left = a1;
  new->right = (lisp_value*)l;
  return (lisp_value*)new;
}

static lisp_value *lisp_builtin_lambda(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_list *argnames;
  lisp_value *code;
  lisp_list *our_args = (lisp_list*)a;
  (void)scope;

  if (!lisp_get_args(our_args, "l*", &argnames, &code)) {
    return (lisp_value*) lisp_error_new(rt, "expected argument list and code");
  }

  lisp_list *it = argnames;
  while (!lisp_nil_p((lisp_value*)it)) {
    if (it->left->type != type_symbol) {
      return (lisp_value*) lisp_error_new(rt, "argument names must be symbols");
    }
    it = (lisp_list*) it->right;
  }

  lisp_lambda *lambda = (lisp_lambda*)lisp_new(rt, type_lambda);
  lambda->args = argnames;
  lambda->code = code;
  lambda->closure = scope;
  return (lisp_value*) lambda;
}

static lisp_value *lisp_builtin_define(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_symbol *s;
  lisp_value *expr;

  if (!lisp_get_args((lisp_list*)a, "s*", &s, &expr)) {
    return (lisp_value*) lisp_error_new(rt, "expected name and expression");
  }

  lisp_value *evald = lisp_eval(rt, scope, expr);
  lisp_scope_replace_or_insert(scope, s, evald);
  //lisp_scope_bind(scope, s, evald);
  return evald;
}

static lisp_value *lisp_builtin_plus(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int sum = 0;

  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*) lisp_error_new(rt, "expect integers for addition");
    }
    i = (lisp_integer*) args->left;
    sum += i->x;
    args = (lisp_list*)args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = sum;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_minus(lisp_runtime *rt, lisp_scope *scope,
                                      lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int val = 0;
  int len = lisp_list_length(args);

  if (len < 1) {
    return (lisp_value*) lisp_error_new(rt, "expected at least one arg");
  } else if (len == 1) {
    i = (lisp_integer*) args->left;
    val = - i->x;
  } else {
    i = (lisp_integer*) args->left;
    val = i->x;
    args = (lisp_list*)args->right;
    while (!lisp_nil_p((lisp_value*)args)) {
      if (args->left->type != type_integer) {
        return (lisp_value*)lisp_error_new(rt, "expected integer");
      }
      i = (lisp_integer*) args->left;
      val -= i->x;
      args = (lisp_list*) args->right;
    }
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = val;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_multiply(lisp_runtime *rt, lisp_scope *scope,
                                         lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);
  int product = 1;

  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*) lisp_error_new(rt, "expect integers for multiplication");
    }
    i = (lisp_integer*) args->left;
    product *= i->x;
    args = (lisp_list*)args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = product;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_divide(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int val = 0;
  int len = lisp_list_length(args);

  if (len < 1) {
    return (lisp_value*) lisp_error_new(rt, "expected at least one arg");
  }
  i = (lisp_integer*) args->left;
  val = i->x;
  args = (lisp_list*)args->right;
  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*)lisp_error_new(rt, "expected integer");
    }
    i = (lisp_integer*) args->left;
    if (i->x == 0) {
      return (lisp_value*) lisp_error_new(rt, "divide by zero");
    }
    val /= i->x;
    args = (lisp_list*) args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = val;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_cmp_util(lisp_runtime *rt, lisp_scope *scope,
                                         lisp_value *a)
{
  lisp_integer *first, *second;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);

  if (!lisp_get_args((lisp_list*)args, "dd", &first, &second)) {
    return (lisp_value*) lisp_error_new(rt, "expected two integers");
  }

  lisp_integer *result = (lisp_integer*)lisp_new(rt, type_integer);
  result->x = first->x - second->x;
  return (lisp_value*)result;
}

static lisp_value *lisp_builtin_eq(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x == 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_gt(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x > 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_ge(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x >= 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_lt(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x < 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_le(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x <= 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_if(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_value *condition, *body_true, *body_false;

  if (!lisp_get_args((lisp_list*)a, "***", &condition, &body_true, &body_false)) {
    return (lisp_value*) lisp_error_new(rt, "expected condition and two bodies");
  }

  condition = lisp_eval(rt, scope, condition);
  if (condition->type == type_integer && ((lisp_integer*)condition)->x) {
    return lisp_eval(rt, scope, body_true);
  } else {
    return lisp_eval(rt, scope, body_false);
  }
}

static lisp_value *lisp_builtin_null_p(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_value *v;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);

  if (!lisp_get_args(args, "*", &v)) {
    return (lisp_value*) lisp_error_new(rt, "expected one argument");
  }

  lisp_integer *result = (lisp_integer*) lisp_new(rt, type_integer);
  result->x = (int) lisp_nil_p(v);
  return (lisp_value*)result;
}

static lisp_list *empty_list_of_length(lisp_runtime *rt, int l)
{
  if (l == 0) {
    return (lisp_list*)lisp_nil_new(rt);
  }
  lisp_list *list = (lisp_list*) lisp_new(rt, type_list);
  list->right = lisp_nil_new(rt);
  list->left = NULL;
  lisp_list *tmp;
  for (; l > 0; l--) {
    tmp = list;
    list = (lisp_list*) lisp_new(rt, type_list);
    list->right = (lisp_value*)tmp;
    list->left = NULL;
  }
  return list;
}

static lisp_list *get_arglist_advance(lisp_runtime *rt, lisp_list **ar, int len)
{
  lisp_list *l = (lisp_list*)lisp_new(rt, type_list);
  l->right = lisp_nil_new(rt);
  while (len--) {
    l->left = ar[len]->left;
    printf("we're at ");
    lisp_print(stdout, l->left);
    printf("\n");
    ar[len] = (lisp_list*) ar[len]->right;
    lisp_list *tmp = (lisp_list*) lisp_new(rt, type_list);
    tmp->right = (lisp_value*)l;
    l = tmp;
  }
  return l;
}

static lisp_value *lisp_builtin_map(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_value *c;
  lisp_list *arglists;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);

  c = args->left;
  arglists = (lisp_list*)args->right;

  if (arglists->type != type_list || lisp_nil_p((lisp_value*)arglists)) {
    return (lisp_value*)lisp_error_new(rt, "need at least one argument");
  }

  if (lisp_nil_p((lisp_value*)arglists->left)) {
    return lisp_nil_new(rt);
  }

  int nargs = lisp_list_length(arglists);
  lisp_list **first = smb_new(lisp_list*, nargs);
  for (int i = 0; i < nargs; i++) {
    lisp_print(stdout, arglists->left);
    printf("\n");
    first[i] = (lisp_list*)arglists->left;
    arglists = (lisp_list*)arglists->right;
    printf("a\n");
  }

  lisp_list *returns = (lisp_list*)lisp_new(rt, type_list);
  lisp_list *returns_orig = returns;
  while (!lisp_nil_p((lisp_value*)first[0])) {
    //lisp_print(stdout, first[0]);
    printf("foo\n");
    lisp_list *callargs = get_arglist_advance(rt, first, nargs);
    returns->left = lisp_call(rt, scope, (lisp_value*)callargs, c);
    if (lisp_nil_p((lisp_value*)first[0])) {
      returns->right = lisp_new(rt, type_list);
    } else {
      returns->right = lisp_nil_new(rt);
    }
    returns = (lisp_list*)returns->right;
    printf("bar\n");
  }
  return (lisp_value*)returns_orig;
}

void lisp_scope_populate_builtins(lisp_runtime *rt, lisp_scope *scope)
{
  lisp_scope_add_builtin(rt, scope, "eval", lisp_builtin_eval);
  lisp_scope_add_builtin(rt, scope, "car", lisp_builtin_car);
  lisp_scope_add_builtin(rt, scope, "cdr", lisp_builtin_cdr);
  lisp_scope_add_builtin(rt, scope, "quote", lisp_builtin_quote);
  lisp_scope_add_builtin(rt, scope, "cons", lisp_builtin_cons);
  lisp_scope_add_builtin(rt, scope, "lambda", lisp_builtin_lambda);
  lisp_scope_add_builtin(rt, scope, "define", lisp_builtin_define);
  lisp_scope_add_builtin(rt, scope, "+", lisp_builtin_plus);
  lisp_scope_add_builtin(rt, scope, "-", lisp_builtin_minus);
  lisp_scope_add_builtin(rt, scope, "*", lisp_builtin_multiply);
  lisp_scope_add_builtin(rt, scope, "/", lisp_builtin_divide);
  lisp_scope_add_builtin(rt, scope, "==", lisp_builtin_eq);
  lisp_scope_add_builtin(rt, scope, "=", lisp_builtin_eq);
  lisp_scope_add_builtin(rt, scope, ">", lisp_builtin_gt);
  lisp_scope_add_builtin(rt, scope, ">=", lisp_builtin_ge);
  lisp_scope_add_builtin(rt, scope, "<", lisp_builtin_lt);
  lisp_scope_add_builtin(rt, scope, "<=", lisp_builtin_le);
  lisp_scope_add_builtin(rt, scope, "if", lisp_builtin_if);
  lisp_scope_add_builtin(rt, scope, "null?", lisp_builtin_null_p);
  lisp_scope_add_builtin(rt, scope, "map", lisp_builtin_map);
}
