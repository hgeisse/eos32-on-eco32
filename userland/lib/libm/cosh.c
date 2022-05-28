/* Source: musl libc 1.2.1 */

#include <math.h>
#include "libm.h"

float cosh(float x) {
  union {float f; unsigned int i;} u;
  unsigned int w;
  float t;

  u.f = x;
  /* |x| */
  u.i &= 0x7fffffff;
  x = u.f;
  w = u.i;

  /* |x| < log(2) */
  if (w < 0x3f317217) {
    if (w < 0x3f800000 - (12<<23)) {
      FORCE_EVAL(x + 1329227995784915872903807060280344576.0f); /* 0x1p120f); */
      return 1;
    }
    t = expm1(x);
    return 1 + t*t/(2*(1+t));
  }

  /* |x| < log(FLT_MAX) */
  if (w < 0x42b17217) {
    t = exp(x);
    return 0.5f*(t + 1/t);
  }

  /* |x| > log(FLT_MAX) or nan */
  t = __expo2(x, 1.0f);
  return t;
}
