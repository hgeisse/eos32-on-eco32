/*
 * sec1.c -- incremental display of the running seconds by busy waiting
 */


#include <stdio.h>
#include <time.h>


int main(void) {
  int i;
  long then, now;

  then = time(NULL);
  for (i = 1; i < 10; i++) {
    do {
      now = time(NULL);
    } while (now == then);
    then = now;
    printf("seconds = %d\n", i);
  }
  return 0;
}
