#include <stdio.h>
#include "mtest.h"

static struct ff_f t[] = {
#include "ucb/atan2.h"
#include "sanity/atan2.h"
#include "special/atan2.h"
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
    y = atan2(FLOAT(p->x), FLOAT(p->x2));
    e = statusfe();

    if (!checkexcept(e, p->e, p->r)) {
      printf("%s:%d: bad fp exception: %s atan2(%f,%f)=%f, want %s",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->x2), FLOAT(p->y), estr(p->e));
      printf(" got %s\n", estr(e));
      err++;
    }
    d = ulperrf(y, FLOAT(p->y), FLOAT(p->dy));
    if (!checkulp(d, p->r)) {
      printf("%s:%d: %s atan2(%f,%f) want %f got %f ulperr %.3f = %f + %f\n",
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
