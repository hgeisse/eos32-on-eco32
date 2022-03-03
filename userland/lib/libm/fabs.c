#include <math.h>

float fabs(float x)
{
  union {
    float f;
    int i;
  } u;

  u.f = x;
  u.i &= 0x7fffffff;
  return u.f;
}
