/*
 * recurse.c -- deep recursion using up the stack
 */


#include <stdio.h>


#define MAX_LEVEL	1000
#define INC_LEVEL	100


int f(int n) {
  if (n == 0) {
    return 0;
  }
  return n + f(n - 1);
}


int main(void) {
  int level;

  for (level = 0; level <= MAX_LEVEL; level += INC_LEVEL) {
    printf("f(%d) = %d\n", level, f(level));
  }
  printf("done\n");
  return 0;
}
