/*
 * trm.c -- terminal driver
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/tty.h"
#include "../include/dir.h"
#include "../include/user.h"

#include "start.h"
#include "machdep.h"
#include "clock.h"
#include "slp.h"
#include "partab.h"
#include "prim.h"
#include "trm.h"
#include "tt.h"


#define NUM_TERMINALS	1
#define TERM_BASE	((struct device *) 0xF0300000)

#define XMT_RDY		0x01U
#define XMT_IEN		0x02U

#define RDV_RDY		0x01U
#define RCV_IEN		0x02U


struct device {
  unsigned int rcvCtrl;
  unsigned int rcvData;
  unsigned int xmtCtrl;
  unsigned int xmtData;
};


static Bool trmInitialized = FALSE;
static struct tty terminal[NUM_TERMINALS];


/**************************************************************/


static void xmtStart(struct tty *tp) {
  struct device *addr;
  int c;

  addr = (struct device *) tp->t_addr;
  if ((addr->xmtCtrl & XMT_RDY) == 0) {
    /* should never happen */
    return;
  }
  if ((c = getc(&tp->t_outq)) >= 0) {
    /* there is a character to transmit */
    if (tp->t_flags & RAW) {
      addr->xmtData = c;
      addr->xmtCtrl |= XMT_IEN;
    } else
    if (c <= 0x7F) {
      //addr->xmtData = c | (partab[c] & 0200);
      addr->xmtData = c;
      addr->xmtCtrl |= XMT_IEN;
    } else {
      timeout((void (*)(caddr_t)) ttrstrt, (caddr_t) tp, c & 0177);
      tp->t_state |= TIMEOUT;
    }
  } else {
    /* there is no character to transmit */
    addr->xmtCtrl &= ~XMT_IEN;
  }
}


static void xmtISR(int irq, InterruptContext *icp) {
  int dev;
  struct tty *tp;

  /* set process priority, re-enable interrupts */
  spl4();
  enableInt();
  /* restart transmitter */
  dev = irq / 2;
  tp = &terminal[dev];
  ttstart(tp);
  if ((tp->t_state & ASLEEP) != 0 && tp->t_outq.c_cc <= TTLOWAT) {
    wakeup((caddr_t) &tp->t_outq);
  }
}


static void rcvISR(int irq, InterruptContext *icp) {
  int dev;
  struct tty *tp;
  struct device *addr;
  int c;

  /* set process priority, re-enable interrupts */
  spl4();
  enableInt();
  /* process received character */
  dev = (irq - 1) / 2;
  tp = &terminal[dev];
  addr = (struct device *) tp->t_addr;
  c = addr->rcvData;
  if (c == '^') {
    showProcTable(NULL);
    return;
  }
  ttyinput(c, tp);
}


void trmOpen(dev_t dev, int flag) {
  int d;
  struct tty *tp;
  struct device *addr;

  d = minor(dev);
  if (d >= NUM_TERMINALS) {
    u.u_error = ENXIO;
    return;
  }
  tp = &terminal[d];
  addr = TERM_BASE + d;
  tp->t_addr = (caddr_t) addr;
  tp->t_oproc = xmtStart;
  if ((tp->t_state & ISOPEN) == 0) {
    tp->t_state = ISOPEN | CARR_ON;
    //tp->t_flags = EVENP | LCASE | ECHO | XTABS | CRMOD;
    tp->t_flags = EVENP | ECHO | XTABS | CRMOD;
    ttychars(tp);
  }
  setISR(2 * d + 0, xmtISR);
  setISR(2 * d + 1, rcvISR);
  addr->xmtCtrl = XMT_RDY;
  addr->rcvCtrl = RCV_IEN;
  ttyopen(dev, tp);
}


void trmClose(dev_t dev, int flag) {
  ttyclose(&terminal[minor(dev)]);
}


void trmRead(dev_t dev) {
  ttread(&terminal[minor(dev)]);
}


void trmWrite(dev_t dev) {
  ttwrite(&terminal[minor(dev)]);
}


void trmIoctl(dev_t dev, int cmd, caddr_t addr, int flag) {
  if (ttioccomm(cmd, &terminal[minor(dev)], addr, dev) == 0) {
    u.u_error = ENOTTY;
  }
}


/**************************************************************/


/*
 * Print a character on the console terminal.
 */
void terminalOut(char c) {
  while ((TERM_BASE->xmtCtrl & 0x01) == 0) ;
  TERM_BASE->xmtData = c;
}
