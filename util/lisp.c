#include <editline/readline.h>
#include <stdio.h>

#include "libstephen/lisp.h"

int main(int argc, char **argv)
{
  lisp_scope *scope = (lisp_scope*)type_scope->new();
  lisp_scope_populate_builtins(scope);

  while (true) {
    char *input = readline("> ");
    if (input == NULL) {
      break;
    }
    lisp_value *value = lisp_parse(input);
    add_history(input);
    free(input);
    lisp_value *result = lisp_eval(scope, value);
    lisp_decref(value);
    lisp_print(stdout, result);
    fprintf(stdout, "\n");
    lisp_decref(result);
  }

  lisp_decref((lisp_value*)scope);
  return 0;
}
