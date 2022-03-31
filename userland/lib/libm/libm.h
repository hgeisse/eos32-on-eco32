#ifndef __LIBM_H_
#define __LIBM_H_
  
#define GET_FLOAT_WORD(i,d) \
  {                           \
    union {                   \
      float value;            \
      unsigned int bits;      \
    } __u;                    \
    __u.value = (d);          \
    (i) = __u.bits;           \
  }

#define SET_FLOAT_WORD(d,i)                       \
{                                                 \
  union {                                         \
    float value;                                  \
    unsigned int bits;                            \
  } __u;                                          \
  __u.bits = (i);                                \
  (d) = __u.value;                                 \
}


/* FORCE_EVAL(x)
 * prevent optimization (=elimination) of expressions without
 * sideeffects */
#ifndef fp_force_eval
#define fp_force_eval fp_force_eval
static void fp_force_eval(float x) {
  volatile float y;
  y = x;
}
#endif /* fp_force_eval */

#define FORCE_EVAL(x) do { \
  fp_force_eval(x); \
} while (0);

/* prototypes for internal math library functions */

float __expo2(float x, float sign);
float __kernel_cos(float x, float y);
float __kernel_sin(float x, float y, int iy);
int __ieee754_rem_pio2(float x, float *y);
int __kernel_rem_pio2(float *x, float *y, int e0, int nx, int prec, const int *ipio2);

#endif /* __LIBM_H_ */
