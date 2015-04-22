/*
 * sec2.c -- incremental display of the running seconds by sleeping
 */


#include <stdio.h>
#include <eos32.h>


int main(void) {
  int i;

  for (i = 1; i < 10; i++) {
    sleep(1);
    printf("seconds = %d\n", i);
  }
  return 0;
}
