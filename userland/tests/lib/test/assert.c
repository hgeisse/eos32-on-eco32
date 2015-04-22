/*
 * assert.c -- diagnostics tests
 */


#define NDEBUG
#include <assert.h>


void f1(int x, int y) {
  assert(x + y == 3);
}


void f2(int x, int y) {
  assert(x + y != 3);
}


#undef NDEBUG
#include <assert.h>


void f3(int x, int y) {
  assert(x + y == 3);
}


void f4(int x, int y) {
  assert(x + y != 3);
}


int main(void) {
  f1(1, 2);
  f2(1, 2);
  f3(1, 2);
  f4(1, 2);
  return 0;
}
