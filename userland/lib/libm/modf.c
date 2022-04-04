/* Source: musl libc 1.2.1 */
#include <math.h>
#include "libm.h"

float modf(float x, float *iptr) {
  union {float f; unsigned int i;} u;
  unsigned int mask;
  int e;

  u.f = x;
  e = (int)(u.i>>23 & 0xff) - 0x7f;

  /* no fractional part */
  if (e >= 23) {
    *iptr = x;
    if (e == 0x80 && u.i<<9 != 0) { /* nan */
      return x;
    }
    u.i &= 0x80000000;
    return u.f;
  }
  /* no integral part */
  if (e < 0) {
    u.i &= 0x80000000;
    *iptr = u.f;
    return x;
  }

  mask = 0x007fffff>>e;
  if ((u.i & mask) == 0) {
    *iptr = x;
    u.i &= 0x80000000;
    return u.f;
  }
  u.i &= ~mask;
  *iptr = u.f;
  return x - u.f;
}
