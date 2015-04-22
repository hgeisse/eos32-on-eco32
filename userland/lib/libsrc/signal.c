/*
 * signal.c -- signals
 */


#include <signal.h>
#include <eos32.h>


void (*signal(int sig, void (*handler)(int)))(int) {
  return _signal(sig, handler);
}


int raise(int sig) {
  return kill(getpid(), sig);
}
