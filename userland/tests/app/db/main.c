/*
 * main.c -- main program
 */


#include <stdio.h>
#include <stdlib.h>

#include "db.h"


void usage(char *myself) {
  printf("Usage: %s name\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  int n;

  if (argc != 2) {
    usage(argv[0]);
  }
  prepare();
  n = lookup(argv[1]);
  if (n < 0) {
    printf("'%s' not found in data base.\n", argv[1]);
  } else {
    printf("The value of '%s' is %d.\n", argv[1], n);
  }
  return 0;
}
