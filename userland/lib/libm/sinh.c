/* Source: musl libc 1.2.1 */
#include <math.h>
#include "libm.h"

float sinh(float x) {
  union {float f; unsigned int i;} u;
  unsigned int w;
  float t, h, absx;

  u.f = x;
  h = 0.5;
  if (u.i >> 31) {
    h = -h;
  }
  /* |x| */
  u.i &= 0x7fffffff;
  absx = u.f;
  w = u.i;

  /* |x| < log(FLT_MAX) */
  if (w < 0x42b17217) {

    t = expm1(absx);

    if (w < 0x3f800000) {
      if (w < 0x3f800000 - (12<<23)) {
        return x;
      }
      return h*(2*t - t*t/(t+1));
    }
    return h*(t + t/(t+1));
  }

  /* |x| > logf(FLT_MAX) or nan */
  t = __expo2(absx, 2*h);
  return t;
}
