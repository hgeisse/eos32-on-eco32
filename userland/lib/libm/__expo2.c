#include <math.h>
#include "libm.h"

/* k is such that k*ln2 has minimal relative error and x - kln2 > log(FLT_MIN) */
static const int k = 235;
static const float kln2 = 162.889587402; /* 0x1.45c778p+7f; */

/* expf(x)/2 for x >= log(FLT_MAX), slightly better than 0.5f*expf(x/2)*expf(x/2) */
float __expo2(float x, float sign) {
  float scale;

  /* note that k is odd and scale*scale overflows */
  SET_FLOAT_WORD(scale, (unsigned int)(127.0f /* 0x7f */ + k/2) << 23);
  /* exp(x - k ln2) * 2**(k-1) */
  /* in directed rounding correct sign before rounding or overflow is important */
  return exp(x - kln2) * (sign * scale) * scale;
}