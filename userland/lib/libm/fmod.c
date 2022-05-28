#include <math.h>

float fmod(float x, float y) {
  union {float f; unsigned int i;} ux, uy;
  int ex, ey;
  unsigned int sx;
  unsigned int i;
  unsigned int uxi;

  ux.f = x;
  uy.f = y;
  ex = ux.i>>23 & 0xff;
  ey = uy.i>>23 & 0xff;
  sx = ux.i & 0x80000000;
  uxi = ux.i;

  if (uy.i<<1 == 0 || isnan(y) || ex == 0xff)
    return (x*y)/(x*y);
  if (uxi<<1 <= uy.i<<1) {
    if (uxi<<1 == uy.i<<1)
      return 0*x;
    return x;
  }

  /* normalize x and y */
  if (!ex) {
    for (i = uxi<<9; i>>31 == 0; ex--, i <<= 1);
    uxi <<= -ex + 1;
  } else {
    uxi &= -1U >> 9;
    uxi |= 1U << 23;
  }
  if (!ey) {
    for (i = uy.i<<9; i>>31 == 0; ey--, i <<= 1);
    uy.i <<= -ey + 1;
  } else {
    uy.i &= -1U >> 9;
    uy.i |= 1U << 23;
  }

  /* x mod y */
  for (; ex > ey; ex--) {
    i = uxi - uy.i;
    if (i >> 31 == 0) {
      if (i == 0)
        return 0*x;
      uxi = i;
    }
    uxi <<= 1;
  }
  i = uxi - uy.i;
  if (i >> 31 == 0) {
    if (i == 0)
      return 0*x;
    uxi = i;
  }
  for (; uxi>>23 == 0; uxi <<= 1, ex--);

  /* scale result up */
  if (ex > 0) {
    uxi -= 1U << 23;
    uxi |= (unsigned int)ex << 23;
  } else {
    uxi >>= -ex + 1;
  }
  uxi |= sx;
  ux.i = uxi;
  return ux.f;
}
