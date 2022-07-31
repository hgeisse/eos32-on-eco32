/*
 * mem.c -- memory devices
 */

/*
 *	Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel virtual memory
 *	minor device 2 is EOF/RATHOLE
 *  minor device 3 is zero-device
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/conf.h"
#include "../include/seg.h"

#include "klib.h"
#include "mem.h"
#include "machdep.h"
#include "subr.h"


void mmopen(dev_t dev, int flag) {
}


void mmclose(dev_t dev, int flag) {
}


void mmread(dev_t dev) {
  caddr_t addr;
  int c;

  if(minor(dev) > 3) {
    /* illegal minor device */
    u.u_error = ENXIO;
    return;
  }
  if(minor(dev) == 3) {
    /* zero device */
    passc(0);
    return;
  }
  if(minor(dev) == 2) {
    /* null device */
    return;
  }
  /* kernel virtual memory or physical memory */
  do {
    if (minor(dev) == 1) {
      addr = (caddr_t) u.u_offset;
    } else {
      addr = PHYS2DIRECT(u.u_offset);
    }
    c = *addr;
  } while (u.u_error==0 && passc(c)>=0);
}


void mmwrite(dev_t dev) {
  caddr_t addr;
  int c;

  if(minor(dev) > 3) {
    /* illegal minor device */
    u.u_error = ENXIO;
    return;
  }
  if(minor(dev) == 3) {
    /* zero device */
    u.u_count = 0;
    return;
  }
  if(minor(dev) == 2) {
    /* null device */
    u.u_count = 0;
    return;
  }
  /* kernel virtual memory or physical memory */
  for (;;) {
    if (minor(dev) == 1) {
      addr = (caddr_t) u.u_offset;
    } else {
      addr = PHYS2DIRECT(u.u_offset);
    }
    if ((c = cpass()) < 0 || u.u_error != 0) {
      break;
    }
    *addr = c;
  }
}
