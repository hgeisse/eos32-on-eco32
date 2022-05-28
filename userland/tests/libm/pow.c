#include <stdio.h>
#include "mtest.h"

static struct ff_f t[] = {
#include "special/pow.h"
#include "ucb/pow.h"
#include "sanity/pow.h"
};

int main(void) {
  float y;
  float d;
  int e, i, err = 0, all = 0;
  struct ff_f *p;

  for (i = 0; i < sizeof t/sizeof *t; i++) {
    p = t + i;

    if (p->r < 0)
      continue;

    all++;
    resetfe();
    setrnd(p->r);
    y = pow(FLOAT(p->x), FLOAT(p->x2));
    e = statusfe();

    if (!checkexcept(e, p->e, p->r)) {
      if (fabs(y) < 1.17549435082e-38 /* 0x1p-126f = 0x00800000 */ 
          && (e|INEXACT) == (INEXACT|UNDERFLOW))
        printf("X ");
      else
        err++;
      printf("%s:%d: bad fp exception: %s pow(%f,%f)=%f, want %s",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->x2), FLOAT(p->y), estr(p->e));
      printf(" got %s\n", estr(e));
    }
    d = ulperrf(y, FLOAT(p->y), FLOAT(p->dy));
    if (!checkulp(d, p->r)) {
      printf("%s:%d: %s pow(%f,%f) want %f got %f ulperr %.3f = %f + %f\n",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->x2), FLOAT(p->y), y, d, d-FLOAT(p->dy), FLOAT(p->dy));
      err++;
    }
  }
  printf("Results: %s [%f (%d/%d)]\n",
      err > 0 ? "FAIL" : "SUCCESS",
      (err / (float) all),
      err, all);

  return !!err;
}
