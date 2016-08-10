#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libstephen/lisp.h"
#include "libstephen/ht.h"

// General functions for types.

static lisp_value *eval_error(lisp_runtime *rt, lisp_scope *s, lisp_value *v)
{
  (void)s;
  (void)v;
  return (lisp_value*) lisp_error_new(rt, "cannot evaluate this object");
}

static lisp_value *eval_same(lisp_runtime *rt, lisp_scope *s, lisp_value *v)
{
  (void)rt;
  (void)s;
  return v;
}

static lisp_value *call_error(lisp_runtime *rt, lisp_scope *s, lisp_value *c,
                              lisp_value *v)
{
  (void)s;
  (void)c;
  (void)v;
  return (lisp_value*) lisp_error_new(rt, "not callable!");
}

static lisp_value *call_same(lisp_runtime *rt, lisp_scope *s, lisp_value *c,
                             lisp_value *v)
{
  (void)rt;
  (void)s;
  (void)v;
  return c;
}

static DATA next_nop(struct smb_iter *iter, smb_status *status)
{
  (void)iter;
  (void)status;
  return PTR(NULL);
}

static bool has_next_false(struct smb_iter *iter)
{
  (void)iter;
  return false;
}

static bool has_next_index_lt_state(struct smb_iter *iter)
{
  return iter->index < iter->state.data_llint;
}

static void destroy_nop(struct smb_iter *iter)
{
  (void)iter;
}

static void delete_filler(struct smb_iter *iter)
{
  iter->destroy(iter);
  free(iter);
}

static smb_iter expand_nothing(lisp_value *v)
{
  smb_iter it = {
    .ds=v,
    .state=LLINT(0),
    .index=0,
    .next=next_nop,
    .has_next=has_next_false,
    .destroy=destroy_nop,
    .delete=delete_filler,
  };
  return it;
}

// type

static void type_print(FILE *f, lisp_value *v);
static lisp_value *type_new(void);

static lisp_type type_type_obj = {
  .type=&type_type_obj,
  .name="type",
  .print=type_print,
  .new=type_new,
  .eval=eval_error,
  .free=free,
  .call=call_error,
  .expand=expand_nothing,
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
  return (lisp_value*)type;
}

// scope

static void scope_print(FILE *f, lisp_value*v);
static lisp_value *scope_new(void);
static void scope_free(void *v);
static smb_iter scope_expand(lisp_value *);

static lisp_type type_scope_obj = {
  .type=&type_type_obj,
  .name="scope",
  .print=scope_print,
  .new=scope_new,
  .eval=eval_error,
  .free=scope_free,
  .call=call_error,
  .expand=scope_expand,
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
  scope->up = NULL;
  ht_init(&scope->scope, symbol_hash, symbol_compare);
  return (lisp_value*)scope;
}

static void scope_free(void *v)
{
  lisp_scope *scope = (lisp_scope*) v;
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

static DATA scope_expand_next(struct smb_iter *it, smb_status *status)
{
  smb_iter *htiter = it->ds;
  if (it->index == 0) {
    it->index++;
    return it->state; // contains the upper scope
  } else if (it->index % 2 == 1) {
    // odd index means grab a new key
    it->state = htiter->next(htiter, status);
    it->index++;
    return it->state; // contains the symbol that is key
  } else {
    it->index++;
    return ht_get(htiter->ds, it->state, status);
  }
}

static bool scope_expand_has_next(struct smb_iter *it)
{
  smb_iter *htiter = it->ds;
  if (it->index % 2 == 0) {
    return true;
  } else {
    return htiter->has_next(htiter);
  }
}

static void scope_expand_destroy(struct smb_iter *it)
{
  smb_iter *htiter = it->ds;
  htiter->delete(htiter);
}

static smb_iter scope_expand(lisp_value *v)
{
  lisp_scope *scope = (lisp_scope*) v;
  smb_iter *htiter = malloc(sizeof(smb_iter));
  *htiter = ht_get_iter(&scope->scope);
  smb_iter it = {
    .ds=htiter,
    .state=PTR(scope),
    .index=0,
    .next=scope_expand_next,
    .has_next=scope_expand_has_next,
    .destroy=scope_expand_destroy,
    .delete=delete_filler,
  };
  return it;
}

// list

static void list_print(FILE *f, lisp_value *v);
static lisp_value *list_new(void);
static lisp_value *list_eval(lisp_runtime*, lisp_scope*, lisp_value*);
static smb_iter list_expand(lisp_value*);

static lisp_type type_list_obj = {
  .type=&type_type_obj,
  .name="list",
  .print=list_print,
  .new=list_new,
  .eval=list_eval,
  .free=free,
  .call=call_error,
  .expand=list_expand,
};
lisp_type *type_list = &type_list_obj;

static lisp_value *list_eval(lisp_runtime *rt, lisp_scope *scope, lisp_value *v)
{
  lisp_list *list = (lisp_list*) v;
  if (list->right->type != type_list) {
    return (lisp_value*) lisp_error_new(rt, "bad function call syntax");
  }
  lisp_value *callable = lisp_eval(rt, scope, list->left);
  lisp_value *rv = lisp_call(rt, scope, callable, list->right);
  return rv;
}

static void list_print_internal(FILE *f, lisp_list *list)
{
  if (lisp_nil_p((lisp_value*)list)) {
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
  list->left = NULL;
  list->right = NULL;
  return (lisp_value*) list;
}

bool lisp_nil_p(lisp_value *l)
{
  return (l->type == type_list) &&
    (((lisp_list*)l)->right == NULL) &&
    (((lisp_list*)l)->left == NULL);
}

static DATA list_expand_next(smb_iter *it, smb_status *status)
{
  (void)status;
  lisp_list *l = (lisp_list*) it->ds;
  it->index++;
  switch (it->index) {
  case 1:
    return PTR(l->left);
  case 2:
    return PTR(l->right);
  default:
    return PTR(NULL);
  }
}

static bool list_has_next(smb_iter *it)
{
  lisp_value *l = (lisp_value*)it->ds;
  if (lisp_nil_p(l)) {
    return false;
  } else {
    return it->index < it->state.data_llint;
  }
}

static smb_iter list_expand(lisp_value *v)
{
  smb_iter it = {
    .ds=v,
    .state=LLINT(2),
    .index=0,
    .next=list_expand_next,
    .has_next=list_has_next,
    .destroy=destroy_nop,
    .delete=delete_filler,
  };
  return it;
}

// symbol

static void symbol_print(FILE *f, lisp_value *v);
static lisp_value *symbol_new(void);
static lisp_value *symbol_eval(lisp_runtime*, lisp_scope*, lisp_value*);
static void symbol_free(void *v);
static smb_iter symbol_expand(lisp_value*v);

static lisp_type type_symbol_obj = {
  .type=&type_type_obj,
  .name="symbol",
  .print=symbol_print,
  .new=symbol_new,
  .eval=symbol_eval,
  .free=symbol_free,
  .call=call_error,
  .expand=expand_nothing,
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
  symbol->sym = NULL;
  return (lisp_value*)symbol;
}

static lisp_value *symbol_eval(lisp_runtime *rt, lisp_scope *scope,
                               lisp_value *value)
{
  (void)rt;
  lisp_symbol *symbol = (lisp_symbol*) value;
  return lisp_scope_lookup(rt, scope, symbol);
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
  .name="error",
  .print=error_print,
  .new=error_new,
  .eval=eval_same,
  .free=error_free,
  .call=call_same,
  .expand=expand_nothing,
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
  .name="integer",
  .print=integer_print,
  .new=integer_new,
  .eval=eval_same,
  .free=free,
  .call=call_error,
  .expand=expand_nothing,
};
lisp_type *type_integer = &type_integer_obj;

static void integer_print(FILE *f, lisp_value *v)
{
  lisp_integer *integer = (lisp_integer*) v;
  fprintf(f, "%d", integer->x);
}

static lisp_value *integer_new(void)
{
  lisp_integer *integer = malloc(sizeof(lisp_integer));
  integer->x = 0;
  return (lisp_value*)integer;
}

// string

static void string_print(FILE *f, lisp_value *v);
static lisp_value *string_new(void);
static void string_free(void *v);

static lisp_type type_string_obj = {
  .type=&type_type_obj,
  .name="string",
  .print=string_print,
  .new=string_new,
  .eval=eval_same,
  .free=string_free,
  .call=call_error,
  .expand=expand_nothing,
};
lisp_type *type_string = &type_string_obj;

static void string_print(FILE *f, lisp_value *v)
{
  lisp_string *str = (lisp_string*) v;
  fprintf(f, "%s", str->s);
}

static lisp_value *string_new(void)
{
  lisp_string *str = malloc(sizeof(lisp_string));
  str->s = NULL;
  return (lisp_value*)str;
}

static void string_free(void *v)
{
  lisp_string *str = (lisp_string*) v;
  free(str->s);
  free(str);
}

// builtin

static void builtin_print(FILE *f, lisp_value *v);
static lisp_value *builtin_new(void);
static lisp_value *builtin_call(lisp_runtime *rt, lisp_scope *scope,
                                lisp_value *c, lisp_value *arguments);

static lisp_type type_builtin_obj = {
  .type=&type_type_obj,
  .name="builtin",
  .print=builtin_print,
  .new=builtin_new,
  .eval=eval_error,
  .free=free,
  .call=builtin_call,
  .expand=expand_nothing,
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
  builtin->call = NULL;
  builtin->name = NULL;
  return (lisp_value*) builtin;
}

static lisp_value *builtin_call(lisp_runtime *rt, lisp_scope *scope,
                                lisp_value *c, lisp_value *arguments)
{
  lisp_builtin *builtin = (lisp_builtin*) c;
  return builtin->call(rt, scope, arguments);
}

// lambda

static void lambda_print(FILE *f, lisp_value *v);
static lisp_value *lambda_new(void);
static lisp_value *lambda_call(lisp_runtime *rt, lisp_scope *scope,
                               lisp_value *c, lisp_value *arguments);
static smb_iter lambda_expand(lisp_value *v);

static lisp_type type_lambda_obj = {
  .type=&type_type_obj,
  .name="lambda",
  .print=lambda_print,
  .new=lambda_new,
  .eval=eval_error,
  .free=free,
  .call=lambda_call,
  .expand=lambda_expand,
};
lisp_type *type_lambda = &type_lambda_obj;

static void lambda_print(FILE *f, lisp_value *v)
{
  (void)v;
  fprintf(f, "<lambda function>");
}

static lisp_value *lambda_new()
{
  lisp_lambda *lambda = malloc(sizeof(lisp_lambda));
  lambda->args = NULL;
  lambda->code = NULL;
  return (lisp_value*) lambda;
}

static lisp_value *lambda_call(lisp_runtime *rt, lisp_scope *scope,
                               lisp_value *c, lisp_value *arguments)
{
  lisp_lambda *lambda = (lisp_lambda*) c;
  lisp_list *argvalues = (lisp_list*)lisp_eval_list(rt, scope, arguments);
  lisp_scope *inner = (lisp_scope*)lisp_new(rt, type_scope);
  inner->up = lambda->closure;

  lisp_list *it1 = lambda->args, *it2 = argvalues;
  while (!lisp_nil_p((lisp_value*)it1) && !lisp_nil_p((lisp_value*)it2)) {
    lisp_scope_bind(inner, (lisp_symbol*) it1->left, it2->left);
    it1 = (lisp_list*) it1->right;
    it2 = (lisp_list*) it2->right;
  }

  if (!lisp_nil_p((lisp_value*)it1)) {
    return (lisp_value*) lisp_error_new(rt, "not enough arguments");
  }
  if (!lisp_nil_p((lisp_value*)it2)) {
    return (lisp_value*) lisp_error_new(rt, "too many arguments");
  }

  return lisp_eval(rt, inner, lambda->code);
}

static DATA lambda_expand_next(smb_iter *it, smb_status *status)
{
  (void)status;
  lisp_lambda *l = (lisp_lambda*)it->ds;
  it->index++;
  switch (it->index) {
  case 1:
    return PTR(l->args);
  case 2:
    return PTR(l->code);
  case 3:
    return PTR(l->closure);
  default:
    return PTR(NULL);
  }
}

static smb_iter lambda_expand(lisp_value *v)
{
  smb_iter it = {
    .ds=v,
    .state=LLINT(3),
    .index=0,
    .next=lambda_expand_next,
    .has_next=has_next_index_lt_state,
    .destroy=destroy_nop,
    .delete=delete_filler,
  };
  return it;
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

lisp_value *lisp_eval(lisp_runtime *rt, lisp_scope *scope, lisp_value *value)
{
  return value->type->eval(rt, scope, value);
}

lisp_value *lisp_call(lisp_runtime *rt, lisp_scope *scope,
                      lisp_value *callable, lisp_value *args)
{
  if (callable->type == type_error) {
    return callable;
  }

  return callable->type->call(rt, scope, callable, args);
}

lisp_value *lisp_new(lisp_runtime *rt, lisp_type *typ)
{
  lisp_value *new = typ->new();
  new->type = typ;
  new->next = NULL;
  new->mark = GC_NOMARK;
  if (rt->head == NULL) {
    rt->head = new;
    rt->tail = new;
  } else {
    rt->tail->next = new;
    rt->tail = new;
  }
  return new;
}
