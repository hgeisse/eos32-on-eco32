#include "libm.h"
#include <stdio.h>

float sinf(float x) {
  printf("bfranken DEBUG called sin(%f)\n", x);
  return 42.0;
}
