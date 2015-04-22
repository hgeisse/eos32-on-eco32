/*
 * init.c -- the mother of all processes
 */


#include <eos32.h>
#include <string.h>


void putString(char *str) {
  write(2, str, strlen(str));
}


void panic(char *msg) {
  putString("INIT panic: ");
  putString(msg);
  putString("\n");
  while (1) ;
}


int main(void) {
  int pid;
  char *args[2];

  open("/dev/console", 2);
  dup(0);
  dup(0);
  while (1) {
    pid = fork();
    if (pid < 0) {
      panic("cannot fork shell process");
    }
    if (pid == 0) {
      /* child */
      args[0] = "/tst/sh";
      args[1] = NULL;
      execv(args[0], args);
      panic("cannot exec shell");
    }
    /* parent */
    pid = wait(NULL);
    if (pid < 0) {
      panic("cannot wait for shell termination");
    }
  }
  /* never reached */
  return 0;
}
