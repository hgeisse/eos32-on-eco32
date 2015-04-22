/*
 * sig2.c -- demonstrate signalling between two processes
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <eos32.h>


int myPid;
int yourPid;


void handler(int sig) {
  signal(SIGTERM, handler);
  printf("<%d>: received signal %d\n", myPid, sig);
}


int main(void) {
  int n, m;

  printf("sig2: signalling between two processes\n");
  myPid = getpid();
  yourPid = fork();
  if (yourPid < 0) {
    printf("cannot fork\n");
    exit(1);
  }
  if (yourPid == 0) {
    /* child */
    yourPid = myPid;
    myPid = getpid();
    printf("child: myPid = %d, yourPid = %d\n", myPid, yourPid);
    printf("child: installing handler\n");
    signal(SIGTERM, handler);
    printf("child: waiting for signal...\n");
    while (1) ;
  } else {
    /* parent */
    printf("parent: myPid = %d, yourPid = %d\n", myPid, yourPid);
    printf("parent: sleeping for 3 seconds\n");
    sleep(3);
    printf("parent: sending signal %d to process %d...\n",
           SIGTERM, yourPid);
    kill(yourPid, SIGTERM);
    printf("parent: sleeping for 3 seconds\n");
    sleep(3);
    printf("parent: sending signal %d to process %d...\n",
           SIGTERM, yourPid);
    kill(yourPid, SIGTERM);
    printf("parent: sleeping for 3 seconds\n");
    sleep(3);
    printf("parent: sending signal %d to process %d...\n",
           SIGKILL, yourPid);
    kill(yourPid, SIGKILL);
    printf("parent: waiting for child\n");
    n = wait(&m);
    printf("child %d dead, status = 0x%08X\n", n, m);
  }
  return 0;
}
