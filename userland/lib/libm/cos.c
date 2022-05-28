/* Source: musl libc 0.8.6
 * k_cosf.c -- float version of k_cos.c
 * s_cosf.c -- float version of s_cos.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include <math.h>
#include "libm.h"

static const float
one =  1.0000000000e+00, /* 0x3f800000 */
    C1  =  4.1666667908e-02, /* 0x3d2aaaab */
    C2  = -1.3888889225e-03, /* 0xbab60b61 */
    C3  =  2.4801587642e-05, /* 0x37d00d01 */
    C4  = -2.7557314297e-07, /* 0xb493f27c */
    C5  =  2.0875723372e-09, /* 0x310f74f6 */
    C6  = -1.1359647598e-11; /* 0xad47d74e */

float __kernel_cos(float x, float y) {
  float a,hz,z,r,qx;
  int ix;
  GET_FLOAT_WORD(ix,x);
  ix &= 0x7fffffff;                       /* ix = |x|'s high word*/
  if(ix<0x32000000) {                     /* if x < 2**27 */
    if(((int)x)==0) return one;         /* generate inexact */
  }
  z  = x*x;
  r  = z*(C1+z*(C2+z*(C3+z*(C4+z*(C5+z*C6)))));
  if(ix < 0x3e99999a)                     /* if |x| < 0.3 */
    return one - ((float)0.5*z - (z*r - x*y));
  else {
    if(ix > 0x3f480000) {               /* x > 0.78125 */
      qx = (float)0.28125;
    } else {
      SET_FLOAT_WORD(qx,ix-0x01000000);       /* x/4 */
    }
    hz = (float)0.5*z-qx;
    a  = one-qx;
    return a - (hz - (z*r-x*y));
  }
}

float cos(float x) {
  float y[2],z=0.0;
  int n,ix;

  GET_FLOAT_WORD(ix,x);

  /* |x| ~< pi/4 */
  ix &= 0x7fffffff;
  if(ix <= 0x3f490fd8) return __kernel_cos(x,z);

  /* cos(Inf or NaN) is NaN */
  else if (ix>=0x7f800000) return x-x;

  /* argument reduction needed */
  else {
    n = __ieee754_rem_pio2(x,y);
    switch(n&3) {
      case 0: return  __kernel_cos(y[0],y[1]);
      case 1: return -__kernel_sin(y[0],y[1],1);
      case 2: return -__kernel_cos(y[0],y[1]);
      default:
              return  __kernel_sin(y[0],y[1],1);
    }
  }
}
