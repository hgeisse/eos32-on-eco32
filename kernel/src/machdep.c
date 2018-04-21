/*
 * machdep.c -- machine dependent parts of kernel
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/acct.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/proc.h"
#include "../include/seg.h"
#include "../include/map.h"
#include "../include/reg.h"
#include "../include/buf.h"

#include "machdep.h"
#include "klib.h"
#include "malloc.h"
#include "start.h"
#include "ureg.h"
#include "sig.h"
#include "clock.h"


/**************************************************************/


/*
 * Interrupt and exception messages which will be shown if
 * the corresponding interrupt or exception is not handled.
 */
static char *exceptionCause[32] = {
  /*  0 */  "serial line 0 xmt interrupt",
  /*  1 */  "serial line 0 rcv interrupt",
  /*  2 */  "serial line 1 xmt interrupt",
  /*  3 */  "serial line 1 rcv interrupt",
  /*  4 */  "keyboard interrupt",
  /*  5 */  "unknown interrupt 5",
  /*  6 */  "unknown interrupt 6",
  /*  7 */  "unknown interrupt 7",
  /*  8 */  "disk interrupt",
  /*  9 */  "unknown interrupt 9",
  /* 10 */  "unknown interrupt 10",
  /* 11 */  "unknown interrupt 11",
  /* 12 */  "unknown interrupt 12",
  /* 13 */  "unknown interrupt 13",
  /* 14 */  "timer 0 interrupt",
  /* 15 */  "timer 1 interrupt",
  /* 16 */  "bus timeout exception",
  /* 17 */  "illegal instruction exception",
  /* 18 */  "privileged instruction exception",
  /* 19 */  "divide instruction exception",
  /* 20 */  "trap instruction exception",
  /* 21 */  "TLB miss exception",
  /* 22 */  "TLB write exception",
  /* 23 */  "TLB invalid exception",
  /* 24 */  "illegal address exception",
  /* 25 */  "privileged address exception",
  /* 26 */  "unknown exception 26",
  /* 27 */  "unknown exception 27",
  /* 28 */  "unknown exception 28",
  /* 29 */  "unknown exception 29",
  /* 30 */  "unknown exception 30",
  /* 31 */  "unknown exception 31"
};


/*
 * This is the default interrupt service routine.
 * It simply panics with a message that tells the cause.
 */
static void defaultISR(int irq, InterruptContext *icp) {
  /* set process priority, re-enable interrupts */
  spl7();
  enableInt();
  /* simply panic */
  panic(exceptionCause[irq]);
}


/*
 * Initialize all interrupts and exceptions to the default ISR.
 * Enable interrupts.
 */
void initInterrupts(void) {
  int i;

  for (i = 0; i < 32; i++) {
    setISR(i, defaultISR);
  }
  enableInt();
}


/**************************************************************/


/*
 * Variable to record a bus timeout.
 */
static Bool busTimeoutSeen;


/*
 * ISR to signal a bus timeout.
 */
static void busTimeoutISR(int irq, InterruptContext *icp) {
  /* remember the bus timeout */
  busTimeoutSeen = TRUE;
  /* skip the offending instruction */
  icp->ic_reg[30] += 4;
}


/*
 * Determine the amount of memory installed in this machine.
 * Do this by trying to read from physical memory (starting
 * at frame zero) until a bus timeout occurs, but don't step
 * past maximum possible RAM.
 * Return the total memory size as number of page frames.
 */
int getMemorySize(void) {
  ISR oldService;
  caddr_t ptr;
  char b;
  int numFrames;

  busTimeoutSeen = FALSE;
  oldService = setISR(16, busTimeoutISR);
  ptr = FRAME2ADDR(0);
  numFrames = 0;
  while (numFrames < MEM_MAX) {
    b = *ptr;
    if (busTimeoutSeen) {
      break;
    }
    ptr += PAGE_SIZE;
    numFrames++;
  }
  setISR(16, oldService);
  return numFrames;
}


/**************************************************************/


/*
 * Set priority level to "Low":
 * all devices can interrupt.
 */
unsigned int spl0(void) {
  return setMask(0xFFFF);
}


/*
 * Set priority level to "Low":
 * all devices can interrupt.
 */
unsigned int spl1(void) {
  return setMask(0xFFFF);
}


/*
 * Set priority level to "terminal":
 * clock and disks can interrupt, but
 * terminals can't.
 */
unsigned int spl4(void) {
  return setMask(0xFF0C);
}


/*
 * Set priority level to "disk":
 * the clock can interrupt, but
 * disks and terminals can't.
 */
unsigned int spl5(void) {
  return setMask(0xF000);
}


/*
 * Set priority level to "clock":
 * only emergency interrupts allowed.
 */
unsigned int spl6(void) {
  return setMask(0x8000);
}


/*
 * Set priority level to "High":
 * no device can interrupt.
 */
unsigned int spl7(void) {
  return setMask(0x0000);
}


/*
 * Set priority level to a
 * previously returned one.
 */
void splx(unsigned int mask) {
  setMask(mask);
}


/**************************************************************/


/*
 * This is the clock interrupt service routine.
 */
static void clockISR(int irq, InterruptContext *icp) {
  unsigned int dummy;

  /* set process priority, re-enable interrupts */
  spl6();
  enableInt();
  /* reset clock IRQ */
  dummy = *TIMER_CTRL;
  /* call clock handler */
  clock(icp);
}


/*
 * Initialize the clock.
 * Let it interrupt the CPU every MSECS_PER_TICK milliseconds,
 * i.e., with a frequency of 1/MSECS_PER_TICK kHz.
 */
void clockInit(void) {
  setISR(TIMER_IRQ, clockISR);
  *TIMER_DIV = CLKS_PER_TICK;
  *TIMER_CTRL = 2;
  setMask(getMask() | (1 << TIMER_IRQ));
}


/**************************************************************/


/*
 * Let a process handle a signal by simulating an interrupt.
 */
void sendsig(int signo, caddr_t fun, caddr_t ret) {
  unsigned sp;
  int i;
  unsigned j;

  /* store context on user stack, reserve one slot more for argument */
  sp = u.u_ar0[R29] - (32 + 1) * 4;
  grow(sp);
  for (i = 0, j = sp + 4; i < 32; i++, j += 4) {
    suword((caddr_t) j, u.u_ar0[i]);
  }
  u.u_ar0[R4] = signo;
  u.u_ar0[PC] = (int) fun;
  u.u_ar0[R31] = (int) ret;
  u.u_ar0[R29] = sp;
}


/*
 * Restore context, i.e., simulate a return from interrupt.
 */
void sigret(void) {
  unsigned sp;
  int i;
  unsigned j;

  sp = u.u_ar0[R29];
  for (i = 0, j = sp + 4; i < 32; i++, j += 4) {
    u.u_ar0[i] = fuword((caddr_t) j);
  }
}
