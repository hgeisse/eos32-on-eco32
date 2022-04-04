#include <math.h>

float ldexp(float x, int n) {
  return scalbn(x, n);
}
