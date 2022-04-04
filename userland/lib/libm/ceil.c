/* Source: musl libc 1.2.1 */
#include <math.h>
#include "libm.h"

float ceil(float x) {
  union {float f; unsigned int i;} u;
  int e;
  unsigned int m;

  u.f = x;
  e = (int)(u.i >> 23 & 0xff) - 0x7f;
  
  if (e >= 23)
    return x;
  
  if (e >= 0) {
    m = 0x007fffff >> e;
    if ((u.i & m) == 0)
      return x;
    FORCE_EVAL(x + 1.32922799578e+36 /* 0x1p120f */);
    if (u.i >> 31 == 0)
      u.i += m;
    u.i &= ~m;
  } else {
    FORCE_EVAL(x + 1.32922799578e+36 /* 0x1p120f */);
    if (u.i >> 31)
      u.f = -0.0;
    else if (u.i << 1)
      u.f = 1.0;
  }
  return u.f;
}
