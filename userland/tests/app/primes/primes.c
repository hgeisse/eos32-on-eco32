/*
 * primes.c -- compute primes
 */


#include <stdio.h>


int isPrime(int i) {
  int t;

  if (i == 0 || i == 1) {
    return 0;
  }
  if (i == 2) {
    return 1;
  }
  if (i % 2 == 0) {
    return 0;
  }
  for (t = 3; t * t <= i; t += 2) {
    if (i % t == 0) {
      return 0;
    }
  }
  return 1;
}


int main(void) {
  int i;

  for (i = 0; i < 100; i++) {
    if (isPrime(i)) {
      printf("%d is prime\n", i);
    }
  }
  return 0;
}
