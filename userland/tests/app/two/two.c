/*
 * two.c -- two processes running concurrently
 */


#include <stdio.h>
#include <stdlib.h>
#include <eos32.h>


void loop(void) {
  int i;

  for (i = 0; i < 100000; i++) {
    i = i + i;
    i /= 2;
  }
}


int main(void) {
  int pid;
  int i;
  int status;

  pid = fork();
  if (pid < 0) {
    printf("cannot fork\n");
    exit(1);
  }
  if (pid == 0) {
    /* child */
    for (i = 0; i < 10; i++) {
      loop();
      printf("child: %d\n", i);
    }
  } else {
    /* parent */
    for (i = 0; i < 15; i++) {
      loop();
      printf("parent: %d\n", i);
    }
    wait(&status);
  }
  return 0;
}
