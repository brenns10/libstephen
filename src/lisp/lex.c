#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "libstephen/lisp.h"

typedef struct {
  lisp_value *result;
  int index;
} result;

result lisp_parse_value(char *input, int index);

result lisp_parse_integer(char *input, int index)
{
  //printf("lisp_parse_integer(%s, %d)\n", input, index);
  int n;
  lisp_integer *v = (lisp_integer*)type_integer->new();
  sscanf(input + index, "%d%n", &v->x, &n);
  return (result){(lisp_value*)v, index + n};
}

result lisp_parse_list_or_sexp(char *input, int index)
{
  //printf("lisp_parse_list_or_sexp(%s, %d)\n", input, index);
  bool sexpr = false;
  result r = lisp_parse_value(input, index);
  index = r.index;

  if (r.result == lisp_nilv) {
    return (result){type_nil->new(), index};
  }

  lisp_list *rv = (lisp_list*)type_list->new();
  rv->left = r.result;
  lisp_list *l = rv;

  while (true) {
    while (isspace(input[index])) {
      index++;
    }

    if (input[index] == '.') {
      sexpr=true;
      index++;
      result r = lisp_parse_value(input, index);
      index = r.index;
      l->right = r.result;
      return (result){(lisp_value*)rv, index};
    } else {
      result r = lisp_parse_value(input, index);
      index = r.index;
      if (r.result == lisp_nilv) {
        l->right = r.result;
        return (result){(lisp_value*)rv, index};
      } else {
        l->right = type_list->new();
        l = (lisp_list*)l->right;
        l->left = r.result;
      }
    }
  }
}

result lisp_parse_symbol(char *input, int index)
{
  //printf("lisp_parse_symbol(%s, %d)\n", input, index);
  int n = 0;
  while (input[index + n] && !isspace(input[index + n]) &&
         input[index + n] != ')' && input[index + n] != '.' &&
         input[index + n] != '\'') {
    n++;
  }
  lisp_symbol *s = (lisp_symbol*)type_symbol->new();
  s->sym = malloc(n + 1);
  strncpy(s->sym, input + index, n);
  s->sym[n] = '\0';
  return (result){(lisp_value*)s, index + n};
}

result lisp_parse_quote(char *input, int index)
{
  //printf("lisp_parse_quote(%s, %d)\n", input, index);
  // ' VALUE => (quote VALUE)
  lisp_list *l = (lisp_list*) type_list->new();
  lisp_symbol *q = lisp_symbol_new("quote");
  l->left = (lisp_value*)q;
  lisp_list *s = (lisp_list*) type_list->new();
  s->right = type_nil->new();
  l->right = (lisp_value*)s;
  result r = lisp_parse_value(input, index + 1);
  s->left = r.result;
  index = r.index;
  return (result){(lisp_value*)l, r.index};
}

result lisp_parse_value(char *input, int index)
{
  //printf("lisp_parse_value(%s, %d)\n", input, index);
  while (isspace(input[index])) {
    index++;
  }

  if (input[index] == '\0') {
    return (result){NULL, index};
  }
  if (input[index] == ')') {
    return (result){type_nil->new(), index + 1};
  }
  if (input[index] == '(') {
    return lisp_parse_list_or_sexp(input, index + 1);
  }
  if (input[index] == '\'') {
    return lisp_parse_quote(input, index);
  }
  if (isdigit(input[index])) {
    return lisp_parse_integer(input, index);
  }
  return lisp_parse_symbol(input, index);
}

lisp_value *lisp_parse(char *input)
{
  return lisp_parse_value(input, 0).result;
}
