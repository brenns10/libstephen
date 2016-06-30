#include <stdio.h>

#include "libstephen/str.h"
#include "libstephen/lisp.h"

int main(int argc, char **argv)
{
  lisp_scope *scope = (lisp_scope*)type_scope->new();
  lisp_scope_populate_builtins(scope);

  while (true) {
    fprintf(stdout, "> ");
    char *input = read_line(stdin);
    lisp_value *value = lisp_parse(input);
    lisp_value *result = lisp_eval(scope, value);
    lisp_print(stdout, result);
    fprintf(stdout, "\n");
    lisp_decref(value);
    lisp_decref(result);
  }

  lisp_decref((lisp_value*)scope);
  return 0;
}
