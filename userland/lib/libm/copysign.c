#include <math.h>

float copysign(float x, float y)
{
  union {
    float f; 
    unsigned int i;
  } ux, uy;

  ux.f = x;
  uy.f = y;
  ux.i &= 0x7fffffff;
  ux.i |= uy.i & 0x80000000;
  return ux.f;
}
