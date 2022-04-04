#include <stdio.h>
#include "mtest.h"

static struct f_fi t[] = {
#include "sanity/frexp.h"
#include "special/frexp.h"
};

int main(void) {
  int yi;
  double y;
  float d;
  int e, i, err = 0, all = 0;
  struct f_fi *p;

  for (i = 0; i < sizeof t/sizeof *t; i++) {
    p = t + i;

    if (p->r < 0)
      continue;

    all++;
    resetfe();
    setrnd(p->r);
    y = frexp(FLOAT(p->x), &yi);
    e = statusfe();

    if (!checkexceptall(e, p->e, p->r)) {
      printf("%s:%d: bad fp exception: %s frexp(%f)=%f,%lld, want %s",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->y), p->i, estr(p->e));
      printf(" got %s\n", estr(e));
      err++;
    }
    d = ulperrf(y, FLOAT(p->y), FLOAT(p->dy));
    if (!checkcr(y, FLOAT(p->y), p->r) || (isfinite(FLOAT(p->x)) && yi != p->i)) {
      printf("%s:%d: %s frexp(%f) want %f,%lld got %f,%d ulperr %.3f = %f + %f\n",
        p->file, p->line, rstr(p->r), FLOAT(p->x), FLOAT(p->y), p->i, y, yi, d, d-FLOAT(p->dy), FLOAT(p->dy));
      err++;
    }
  }
  printf("Results: %s [%f (%d/%d)]\n",
      err > 0 ? "FAIL" : "SUCCESS",
      (err / (float) all),
      err, all);

  return !!err;
}
