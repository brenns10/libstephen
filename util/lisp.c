#include <editline/readline.h>
#include <stdio.h>

#include "libstephen/lisp.h"

int main(int argc, char **argv)
{
  lisp_runtime rt;
  lisp_init(&rt);
  lisp_scope *scope = (lisp_scope*)lisp_new(&rt, type_scope);
  lisp_scope_populate_builtins(&rt, scope);

  while (true) {
    char *input = readline("> ");
    if (input == NULL) {
      break;
    }
    lisp_value *value = lisp_parse(&rt, input);
    add_history(input);
    free(input);
    lisp_value *result = lisp_eval(&rt, scope, value);
    lisp_print(stdout, result);
    fprintf(stdout, "\n");
    lisp_mark(&rt, (lisp_value*)scope);
    lisp_sweep(&rt);
  }

  lisp_destroy(&rt);
  return 0;
}
