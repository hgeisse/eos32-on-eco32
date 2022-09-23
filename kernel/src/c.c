/*
 * c.c -- machine configuration
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/buf.h"
#include "../include/tty.h"
#include "../include/conf.h"
#include "../include/proc.h"
#include "../include/text.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/file.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/acct.h"

#include "c.h"
#include "idedsk.h"
#include "serdsk.h"
#include "sdcdsk.h"
#include "ramdsk.h"
#include "trm.h"
#include "mem.h"
#include "sys.h"

/**************************************************************/

/*
 * Routines which set a user error; placed in
 * illegal entries in the bdevsw and cdevsw tables.
 */

static int noSize(dev_t dev) {
	u.u_error = ENODEV;
	return 0;
}

static void noOpen(dev_t dev, int flag) {
	u.u_error = ENODEV;
}

static void noClose(dev_t dev, int flag) {
	u.u_error = ENODEV;
}

static void noStrategy(struct buf *bp) {
	u.u_error = ENODEV;
}

static void noRead(dev_t dev) {
	u.u_error = ENODEV;
}

static void noWrite(dev_t dev) {
	u.u_error = ENODEV;
}

static void noIoctl(dev_t dev, int x, caddr_t y, int z) {
	u.u_error = ENODEV;
}

/**************************************************************/

struct bdevsw bdevsw[] = {
  { ideSize, ideOpen, ideClose, ideStrategy, &ideTab },  /* rk = 0 */
  { serSize, serOpen, serClose, serStrategy, &serTab },  /* rp = 1 */
  { ramSize, ramOpen, ramClose, ramStrategy, &ramTab },  /* rf = 2 */
  { noSize,  noOpen,  noClose,  noStrategy,  0       },  /* tm = 3 */
  { noSize,  noOpen,  noClose,  noStrategy,  0       },  /* tc = 4 */
  { noSize,  noOpen,  noClose,  noStrategy,  0       },  /* hs = 5 */
  { sdcSize, sdcOpen, sdcClose, sdcStrategy, &sdcTab },  /* hp = 6 */
  { noSize,  noOpen,  noClose,  noStrategy,  0       },  /* ht = 7 */
  { noSize,  noOpen,  noClose,  noStrategy,  0       },  /* rl = 8 */
  { 0,       0,       0,        0,           0       },  /* end */
};

int nblkdev;

struct cdevsw cdevsw[] = {
  { trmOpen, trmClose, trmRead, trmWrite, trmIoctl },  /* console = 0 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* pc = 1 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* lp = 2 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* dc = 3 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* dh = 4 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* dp = 5 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* dj = 6 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* dn = 7 */
  { mmopen,  mmclose,  mmread,  mmwrite,  noIoctl  },  /* mem = 8 */
  { ideOpen, ideClose, ideRead, ideWrite, noIoctl  },  /* rk = 9 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* rf = 10 */
  { serOpen, serClose, serRead, serWrite, noIoctl  },  /* rp = 11 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* tm = 12 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* hs = 13 */
  { sdcOpen, sdcClose, sdcRead, sdcWrite, noIoctl  },  /* hp = 14 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* ht = 15 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* du = 16 */
  { syopen,  syclose,  syread,  sywrite,  syioctl  },  /* tty = 17 */
  { noOpen,  noClose,  noRead,  noWrite,  noIoctl  },  /* rl = 18 */
  { 0,       0,        0,       0,        0        },  /* end */
};

int nchrdev;

struct linesw linesw[] =
{
//  { ttyopen, nulldev, ttread, ttwrite, nodev, ttyinput, ttstart },  /* 0 */
  { 0,       0,       0,      0,       0,     0,        0       },  /* end */
};

unsigned int bootDisk;		/* set by the bootstrap/startup procedures */
unsigned int startSector;	/* set by the bootstrap/startup procedures */
unsigned int numSectors;	/* set by the bootstrap/startup procedures */

dev_t	rootdev;
dev_t	swapdev;
dev_t	pipedev;
int	nldisp = 1;

struct	file	file[NFILE];
struct	inode	inode[NINODE];
struct	text	text[NTEXT];
struct	buf	bfreelist;
struct	acct	acctbuf;
struct	inode	*acctp;
