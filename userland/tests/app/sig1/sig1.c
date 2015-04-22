/*
 * sig1.c -- demonstrate signalling within a single process
 */


#include <stdio.h>
#include <signal.h>
#include <eos32.h>


void handler(int sig) {
  printf("signal %d received\n", sig);
}


int main(void) {
  int pid;

  printf("sig1: signalling within a single process\n");
  printf("registering the handler...\n");
  signal(SIGTERM, handler);
  printf("sending signal %d...\n", SIGTERM);
  pid = getpid();
  kill(pid, SIGTERM);
  printf("done\n");
  return 0;
}
