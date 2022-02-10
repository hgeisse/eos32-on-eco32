#ifndef __LIBM_H_
#define __LIBM_H_

/* bfranken DEBUG/DOC: 
  in musl this is implemented as a macro using a union:
  #define GET_FLOAT_WORD(i,d) \
  do {                        \
    union {                   \
      float value;            \
      unsigned bits;          \
    } __u;                    \
    _u.value = (d);           \
    (i) = __u.bits;           \
  } while (0)

  This is not possible for our implementation, because C89 only
  allows local Variables at the beginning of a scope.
  TODO bfranken I could try to wrap the macro in { }

  As it is, this implementation requires __asuint from asuint.c
  */
#define GET_FLOAT_WORD(w,d)                       \
do {                                              \
  (w) = __asuint(d);                              \
} while (0)

#define SET_FLOAT_WORD(d,w)                       \
do {                                              \
  (d) = __asfloat(w);                               \
} while (0)


/* prototypes for internal math library functions */

/*
float __asfloat(unsigned int i);
unsigned int __asuint(float f);
int __ieee754_rem_pio2f(float x, float *y);
int __kernel_rem_pio2f(float *x, float *y, int e0, int nx, int prec, const int *ipio2);
float __kernel_cosf(float x, float y);
float __kernel_sinf(float x, float y, int iy);
*/
float sin(float x);

#endif /* __LIBM_H_ */
