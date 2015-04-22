/*
 * setjmp.c -- non-local jump tests
 */


#include <stdio.h>
#include <assert.h>
#include <setjmp.h>


int count;
jmp_buf buf1;


void jumpTo(int n) {
  longjmp(buf1, n);
}


char *getSP(void) {
  char c;
  char *cp;

  cp = &c;
  return cp;
}


int doJumps(void) {
  jmp_buf buf2;
  char *sp;

  sp = getSP();
  count = 0;
  switch (setjmp(buf1)) {
    case 0:
      assert(sp == getSP());
      assert(count == 0);
      count++;
      jumpTo(0);
      break;
    case 1:
      assert(sp == getSP());
      assert(count == 1);
      count++;
      jumpTo(2);
      break;
    case 2:
      assert(sp == getSP());
      assert(count == 2);
      count++;
      switch (setjmp(buf2)) {
        case 0:
          assert(sp == getSP());
          assert(count == 3);
          count++;
          longjmp(buf2, -7);
          break;
        case -7:
          assert(sp == getSP());
          assert(count == 4);
          count++;
          jumpTo(3);
          break;
        case 5:
          return 13;
        default:
          return 0;
      }
      break;
    case 3:
      longjmp(buf2, 5);
      break;
  }
  return -1;
}


int main(void) {
  assert(doJumps() == 13);
  printf("setjmp tests ok\n");
  return 0;
}
