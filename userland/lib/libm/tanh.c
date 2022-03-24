/* Source: musl libc 1.2.1 */
#include "libm.h"
#include <math.h>

float tanh(float x) {
  union {float f; unsigned int i;} u;
  unsigned int w;
  int sign;
  float t;

  u.f = x;
  /* x = |x| */
  sign = u.i >> 31;
  u.i &= 0x7fffffff;
  x = u.f;
  w = u.i;

  if (w > 0x3f0c9f54) {
    /* |x| > log(3)/2 ~= 0.5493 or nan */
    if (w > 0x41200000) {
      /* |x| > 10 */
      t = 1 + 0/x;
    } else {
      t = expm1(2*x);
      t = 1 - 2/(t+2);
    }
  } else if (w > 0x3e82c578) {
    /* |x| > log(5/3)/2 ~= 0.2554 */
    t = expm1(2*x);
    t = t/(t+2);
  } else if (w >= 0x00800000) {
    /* |x| >= 0x1p-126 */
    t = expm1(-2*x);
    t = -t/(t+2);
  } else {
    /* |x| is subnormal */
    x*x; /* TODO bfranken FORCE_EVAL(x*x); */
    t = x;
  }
  return sign ? -t : t;
}
