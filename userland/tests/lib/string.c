/*
 * string.c -- string tests
 */


#include <stdio.h>
#include <string.h>


#define DST_SIZE	10


char dst[DST_SIZE];


void null(void) {
  int i;

  for (i = 0; i < DST_SIZE; i++) {
    dst[i] = '@';
  }
}


int test(char *ref) {
  int i;

  for (i = 0; i < DST_SIZE; i++) {
    if (dst[i] != ref[i]) {
      return 0;
    }
  }
  return 1;
}


void strcpyTest(void) {
  char *r;

  null();
  r = strcpy(&dst[3], "abc");
  if (r != &dst[3] || !test("@@@abc\0@@@")) {
    printf("strcpy()  -  FAILURE\n");
  } else {
    printf("strcpy()  -  SUCCESS\n");
  }
}


int main(void) {
  strcpyTest();
  return 0;
}
