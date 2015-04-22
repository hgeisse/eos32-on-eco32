/*
 * assert.c -- diagnostics
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void _assertionFailed(char *exp, char *file, int line) {
  fprintf(stderr, "Assertion failed: %s, file %s, line %d\n",
          exp, file, line);
  abort();
}
