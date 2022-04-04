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

#define FE_TONEAREST  0x0000
#define FE_TOWARDZERO 0x0001
#define FE_DOWNWARD   0x0002
#define FE_UPWARD     0x0003


static unsigned __FLOAT_BITS(float __f)
{
  union {float __f; unsigned __i;} __u;
  __u.__f = __f;
  return __u.__i;
}

#define isnan(x) ( (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 )

#define isinf(x) ( (__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000 )  

#define isfinite(x) ( (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 )

#define signbit(x) ((int)(__FLOAT_BITS(x)>>31))

void resetfe(void);
unsigned int statusfe(void);
void setrnd(unsigned int);


float acos(float x);
float asin(float x);
float atan(float x);
float atan2(float y, float x);
float ceil(float x);
float copysign(float x, float y);
float cos(float x);
float cosh(float x);
float exp(float x);
float expm1(float x);
float fabs(float x);
float floor(float x);
float frexp(float x, int *e);
float ldexp(float x, int n);
float log(float x);
float log10(float x);
float log10(float x);
float modf(float x, float *iptr);
float pow(float x, float y);
float sin(float x);
float sinh(float x);
float scalbn(float x, int n);
float sqrt(float x);
float tan(float x);
float tanh(float x);
#endif /* _MATH_H_ */
