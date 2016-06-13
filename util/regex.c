/***************************************************************************//**

  @file         regex.c

  @author       Stephen Brennan

  @date         Created Thursday, 12 September 2013

  @brief        Utility for exercising an arbitrary regex.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/


#include <stdlib.h>
#include <stdio.h>

#include "libstephen/re.h"


int main(int argc, char **argv)
{
  if (argc < 3) {
    fprintf(stderr, "too few arguments\n");
    fprintf(stderr, "usage: %s REGEXP string1 [string2 [...]]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Try to open first arg as file.
  Regex code;
  FILE *in = fopen(argv[1], "r");

  if (in == NULL) {
    // If it doesn't open, it's a regex we should compile.
    printf(";; Regex: \"%s\"\n\n", argv[1]);
    code = recomp(argv[1]);
    printf(";; BEGIN GENERATED CODE:\n");
  } else {
    // Otherwise, open it and read the code from it.
    code = refread(in);
    printf(";; BEGIN READ CODE:\n");
  }
  rewrite(code, stdout);

  int ns = renumsaves(code);
  printf(";; BEGIN TEST RUNS:\n");

  for (int i = 2; i < argc; i++) {
    size_t *saves = NULL;
    ssize_t match = reexec(code, argv[i], &saves);
    if (match != -1) {
      // It matches, report the captured groups.
      printf(";; \"%s\": match(%zd) ", argv[i], match);
      for (ssize_t j = 0; j < ns; j += 2) {
        printf("(%zd, %zd) ", saves[j], saves[j+1]);
      }
      printf("\n");
    } else {
      // Otherwise, report no match.
      printf(";; \"%s\": no match\n", argv[i]);
    }
  }

  refree(code);
}
