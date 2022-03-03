/*
 * math.h -- mathematical functions
 */

#ifndef _MATH_H_
#define _MATH_H_

#define NAN       (0.0f/0.0f)
#define INFINITY  1e40f


/* bfranken TODO: evtl umbenennen und dann tests anpassen */
#define FE_INVALID   0x0100
#define FE_DIVBYZERO 0x0080
#define FE_OVERFLOW  0x0040
#define FE_UNDERFLOW 0x0020
#define FE_INEXACT   0x0010


static unsigned __FLOAT_BITS(float __f)
{
  union {float __f; unsigned __i;} __u;
  __u.__f = __f;
  return __u.__i;
}

#define isnan(x) ( (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 )

#define isinf(x) ( (__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000 )  

#define signbit(x) ((int)(__FLOAT_BITS(x)>>31))

float copysign(float x, float y);
float cos(float x);
float fabs(float x);
float floor(float x);
float sin(float x);
float scalbn(float x, int n);

#endif /* _MATH_H_ */
