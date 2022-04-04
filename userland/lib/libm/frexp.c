#include <math.h>

float frexp(float x, int *e) {
  union { float f; unsigned int i; } y;
  int ee;

  y.f = x;
  ee = y.i>>23 & 0xff;

  if (!ee) {
    if (x) {
      x = frexp(x * 1.84467440737e+19 /* 0x1p64 */, e);
      *e -= 64;
    } else *e = 0;
    return x;
  } else if (ee == 0xff) {
    return x;
  }

  *e = ee - 0x7e;
  y.i &= 0x807ffffful;
  y.i |= 0x3f000000ul;
  return y.f;
}
