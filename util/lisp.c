#include <stdio.h>

#include "libstephen/str.h"
#include "libstephen/lisp.h"

int main(int argc, char **argv)
{
  char *input = read_file(stdin);
  lisp_value *v = lisp_parse(input);
  fprintf(stdout, "parsed:\n");
  lisp_print(stdout, v);
  fprintf(stdout, "\ntype:\n");
  lisp_print(stdout, (lisp_value *)v->type);
  printf("\n");

  lisp_value *r = lisp_eval((lisp_scope*)v, v);
  fprintf(stdout, "eval'd:\n");
  lisp_print(stdout, r);
  fprintf(stdout, "\ntype:\n");
  lisp_print(stdout, (lisp_value *)r->type);
  fprintf(stdout, "\n");

  lisp_decref(v);
  lisp_decref(r);
  return 0;
}
