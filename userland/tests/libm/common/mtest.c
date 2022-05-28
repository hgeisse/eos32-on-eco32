#include <stdio.h>
#include "mtest.h"

int eulpf(float x)
{
  union { 
    float f;
    unsigned int i;
  } u;
  int e;

  u.f = x;
  e = u.i>>23 & 0xff;

  if (!e)
    e++;
  return e - 0x7f - 23;
}

float ulperrf(float got, float want, float dwant)
{
  if (isnan(got) && isnan(want)) {
    return 0;
  }
  if (got == want) {
    if (signbit(got) == signbit(want)) {
      return dwant;
    }
    return inf;
  }
  if (isinf(got)) {
    got = copysign(170141183460469231731687303715884105728.0f, got);
    want *= 0.5;
  }
  return scalbn(got - want, -eulpf(want)) + dwant;
  /* bfranken TODO / TOCHECK*/
  /* return scalbn(got - want, -eulpf(want)) + dwant; */
}


#define length(a) (sizeof(a)/sizeof*(a))
#define flag(x) {x, #x}
static struct {
  int flag;
  char *s;
} eflags[] = {
  flag(INEXACT),
  flag(INVALID),
  flag(DIVBYZERO),
  flag(UNDERFLOW),
  flag(OVERFLOW)
};

char *estr(int f)
{
  static char buf[256];
  char *p = buf;
  int i, all = 0;

  for (i = 0; i < length(eflags); i++)
    if (f & eflags[i].flag) {
      p += sprintf(p, "%s%s", all ? "|" : "", eflags[i].s);
      all |= eflags[i].flag;
    }
  if (all != f) {
    p += sprintf(p, "%s%d", all ? "|" : "", f & ~all);
    all = f;
  }
  p += sprintf(p, "%s", all ? "" : "0");
  return buf;
}

char *rstr(int r)
{
  switch (r) {
  case RN: return "RN";
#ifdef FE_TOWARDZERO
  case RZ: return "RZ";
#endif
#ifdef FE_UPWARD
  case RU: return "RU";
#endif
#ifdef FE_DOWNWARD
  case RD: return "RD";
#endif
  }
  return "R?";
}
