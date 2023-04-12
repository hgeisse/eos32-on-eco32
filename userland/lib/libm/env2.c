#include <math.h>
#include "libm.h"

/* function to clear all floating point exceptions */
void resetfe(void) {
  _resetfe(FE_INEXACT|FE_UNDERFLOW|FE_OVERFLOW|FE_DIVBYZERO|FE_INVALID);
}
