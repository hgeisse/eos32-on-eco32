#include <stdio.h>
#include "mtest.h"

static struct f_ff t[] = {
#include "sanity/modf.h"
#include "special/modf.h"
};

int main(void) {
  float y, yi;
  float d, di;
  int e, i, err = 0, all = 0;
  struct f_ff *p;

  for (i = 0; i < sizeof t/sizeof *t; i++) {
    p = t + i;

    if (p->r < 0)
      continue;

    all++;
    resetfe();
    setrnd(p->r);
    y = modf(FLOAT(p->x), &yi);
    e = statusfe();

    /* TODO: fix inexact */
    if (!checkexceptall(e|INEXACT, p->e|INEXACT, p->r)) {
      printf("%s:%d: bad fp exception: %s modf(%f)=%f,%f, want %s",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->y), FLOAT(p->y2), estr(p->e));
      printf(" got %s\n", estr(e));
      err++;
    }
    d = ulperrf(y, FLOAT(p->y), FLOAT(p->dy));
    di = ulperrf(yi, FLOAT(p->y2), FLOAT(p->dy2));
    if (!checkcr(y, FLOAT(p->y), p->r) || !checkcr(yi, FLOAT(p->y2), p->r)) {
      printf("%s:%d: %s modf(%f) want %f,%f got %f,%f, ulperrf %.3f = %f + %f, %.3f = %f + %f\n",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->y), FLOAT(p->y2), y, yi, d, d-FLOAT(p->dy), FLOAT(p->dy), di, di-FLOAT(p->dy2), FLOAT(p->dy2));
      err++;
    }
  }
  printf("Results: %s [%f (%d/%d)]\n",
      err > 0 ? "FAIL" : "SUCCESS",
      (err / (float) all),
      err, all);

  return !!err;
}
