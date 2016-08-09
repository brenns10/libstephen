#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "libstephen/lisp.h"
#include "libstephen/cb.h"

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

char lisp_escape(char escape)
{
  switch (escape) {
  case 'a':
    return '\a';
  case 'b':
    return '\b';
  case 'f':
    return '\f';
  case 'n':
    return '\n';
  case 'r':
    return '\b';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  default:
    return escape;
  }
}

result lisp_parse_string(char *input, int index)
{
  int i = index + 1;
  cbuf cb;
  cb_init(&cb, 16);
  while (input[i] && input[i] != '"') {
    if (input[i] == '\\') {
      cb_append(&cb, lisp_escape(input[++i]));
    } else {
      cb_append(&cb, input[i]);
    }
    i++;
  }
  cb_trim(&cb);
  lisp_string *str = (lisp_string*)type_string->new();
  str->s = cb.buf;
  return (result){(lisp_value*)str, ++i};
}

result lisp_parse_list_or_sexp(char *input, int index)
{
  while (isspace(input[index])) {index++;}
  if (input[index] == ')') {
    return (result){(lisp_value*)lisp_nil_new(), index + 1};
  }

  result r = lisp_parse_value(input, index);
  index = r.index;
  lisp_list *rv = (lisp_list*)type_list->new();
  rv->left = r.result;
  lisp_list *l = rv;

  while (true) {
    while (isspace(input[index])) {
      index++;
    }

    if (input[index] == '.') {
      index++;
      result r = lisp_parse_value(input, index);
      index = r.index;
      l->right = r.result;
      return (result){(lisp_value*)rv, index};
    } else if (input[index] == ')') {
      index++;
      l->right = lisp_nil_new();
      return (result){(lisp_value*)rv, index};
    } else {
      result r = lisp_parse_value(input, index);
      l->right = type_list->new();
      l = (lisp_list*)l->right;
      l->left = r.result;
      index = r.index;
    }
  }
}

result lisp_parse_symbol(char *input, int index)
{
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
  lisp_list *l = (lisp_list*) type_list->new();
  lisp_symbol *q = lisp_symbol_new("quote");
  l->left = (lisp_value*)q;
  lisp_list *s = (lisp_list*) type_list->new();
  s->right = lisp_nil_new();
  l->right = (lisp_value*)s;
  result r = lisp_parse_value(input, index + 1);
  s->left = r.result;
  index = r.index;
  return (result){(lisp_value*)l, r.index};
}

result lisp_parse_value(char *input, int index)
{
  while (isspace(input[index])) {
    index++;
  }

  if (input[index] == '"') {
    return lisp_parse_string(input, index);
  }
  if (input[index] == '\0') {
    return (result){NULL, index};
  }
  if (input[index] == ')') {
    return (result){lisp_nil_new(), index + 1};
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
