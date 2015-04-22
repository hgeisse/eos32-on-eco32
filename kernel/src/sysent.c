/*
 * sysent.c -- system call switch table
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/dir.h"
#include "../include/user.h"

#include "sysent.h"
#include "sys1.h"
#include "sys2.h"
#include "sys3.h"
#include "sys4.h"
#include "acc.h"
#include "sig.h"
#include "pipe.h"
#include "tt.h"
#include "machdep.h"


/**************************************************************/


/*
 * nonexistent system call - set fatal error code
 */
static void nosys(void) {
  u.u_error = EINVAL;
}


/**************************************************************/


/*
 * This table is the switch used to transfer to the
 * appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */
struct sysent sysent[64] = {
	{ 1, getadr  },			/*  0 = getadr */
	{ 1, rexit   },			/*  1 = exit */
	{ 0, fork    },			/*  2 = fork */
	{ 3, read    },			/*  3 = read */
	{ 3, write   },			/*  4 = write */
	{ 2, open    },			/*  5 = open */
	{ 1, close   },			/*  6 = close */
	{ 0, wait    },			/*  7 = wait */
	{ 2, creat   },			/*  8 = creat */
	{ 2, link    },			/*  9 = link */
	{ 1, unlink  },			/* 10 = unlink */
	{ 0, nosys   },			/* 11 = unused */
	{ 1, chdir   },			/* 12 = chdir */
	{ 0, gtime   },			/* 13 = time */
	{ 3, mknod   },			/* 14 = mknod */
	{ 2, chmod   },			/* 15 = chmod */
	{ 3, chown   },			/* 16 = chown */
	{ 1, sbreak  },			/* 17 = break */
	{ 2, stat    },			/* 18 = stat */
	{ 3, seek    },			/* 19 = seek */
	{ 0, getpid  },			/* 20 = getpid */
	{ 3, smount  },			/* 21 = mount */
	{ 1, sumount },			/* 22 = umount */
	{ 1, setuid  },			/* 23 = setuid */
	{ 0, getuid  },			/* 24 = getuid */
	{ 1, stime   },			/* 25 = stime */
	{ 4, ptrace  },			/* 26 = ptrace */
	{ 1, alarm   },			/* 27 = alarm */
	{ 2, fstat   },			/* 28 = fstat */
	{ 0, pause   },			/* 29 = pause */
	{ 2, utime   },			/* 30 = utime */
	{ 0, nosys   },			/* 31 = unused */
	{ 0, nosys   },			/* 32 = unused */
	{ 2, saccess },			/* 33 = access */
	{ 1, nice    },			/* 34 = nice */
	{ 1, ftime   },			/* 35 = ftime */
	{ 0, sync    },			/* 36 = sync */
	{ 2, kill    },			/* 37 = kill */
	{ 0, nosys   },			/* 38 = unused */
	{ 0, nosys   },			/* 39 = unused */
	{ 0, nosys   },			/* 40 = unused */
	{ 3, dup     },			/* 41 = dup */
	{ 0, pipe    },			/* 42 = pipe */
	{ 1, times   },			/* 43 = times */
	{ 4, profil  },			/* 44 = profil */
	{ 0, nosys   },			/* 45 = unused */
	{ 1, setgid  },			/* 46 = setgid */
	{ 0, getgid  },			/* 47 = getgid */
	{ 3, ssig    },			/* 48 = signal */
	{ 0, sigret  },			/* 49 = sigret */
	{ 0, nosys   },			/* 50 = unused */
	{ 1, sysacct },			/* 51 = acct */
	{ 0, nosys   },			/* 52 = unused */
	{ 0, nosys   },			/* 53 = unused */
	{ 3, ioctl   },			/* 54 = ioctl */
	{ 0, nosys   },			/* 55 = unused */
	{ 0, nosys   },			/* 56 = unused */
	{ 0, nosys   },			/* 57 = unused */
	{ 0, nosys   },			/* 58 = unused */
	{ 3, exece   },			/* 59 = exece */
	{ 1, umask   },			/* 60 = umask */
	{ 1, chroot  },			/* 61 = chroot */
	{ 0, nosys   },			/* 62 = unused */
	{ 0, nosys   },			/* 63 = unused */
};
