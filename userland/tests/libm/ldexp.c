#include <stdio.h>
#include "mtest.h"

static struct fi_f t[] = {
#include "sanity/ldexp.h"
#include "special/ldexp.h"
};

int main(void) {
  float y;
  float d;
  int e, i, err = 0, all = 0;
  struct fi_f *p;

  for (i = 0; i < sizeof t/sizeof *t; i++) {
    p = t + i;

    if (p->r < 0)
      continue;
    
    all++;
    resetfe();
    setrnd(p->r);
    y = ldexp(FLOAT(p->x), p->i);
    e = statusfe();

    if (!checkexceptall(e, p->e, p->r)) {
      printf("%s:%d: bad fp exception: %s ldexp(%f, %lld)=%f, want %s",
        p->file, p->line, rstr(p->r), FLOAT(p->x), p->i, FLOAT(p->y), estr(p->e));
      printf(" got %s\n", estr(e));
      err++;
    }
    d = ulperrf(y, FLOAT(p->y), FLOAT(p->dy));
    if (!checkcr(y, FLOAT(p->y), p->r)) {
      printf("%s:%d: %s ldexp(%f, %lld) want %f got %f, ulperr %.3f = %f + %f\n",
        p->file, p->line, rstr(p->r), FLOAT(p->x), p->i, FLOAT(p->y), y, d, d-FLOAT(p->dy), FLOAT(p->dy));
      err++;
    }
  }
  printf("Results: %s [%f (%d/%d)]\n",
      err > 0 ? "FAIL" : "SUCCESS",
      (err / (float) all),
      err, all);

  return !!err;
}
