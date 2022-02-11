/*
 * subr.c -- common subroutines
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/systm.h"
#include "../include/conf.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/buf.h"

#include "klib.h"
#include "subr.h"
#include "bio.h"
#include "alloc.h"


Bool debugBmap = FALSE;
Bool debugBcopy = FALSE;


/*
 * Bmap defines the structure of file system storage
 * by returning the physical block number on a device
 * given the inode and the logical block number in a
 * file. When convenient, it also leaves the physical
 * block number of the next block of the file in rablock
 * for use in read-ahead.
 */
daddr_t bmap(struct inode *ip, daddr_t bn, int rwflg) {
  int i;
  struct buf *bp, *nbp;
  int j, sh;
  daddr_t nb, *bap;
  dev_t dev;

  if (debugBmap) {
    printf("-----  bmap ip = 0x%08X, bn = 0x%08X, rwflg = %s  -----\n",
           ip, bn,
           rwflg == B_READ ? "read" :
             rwflg == B_WRITE ? "write" :
             "unknown");
  }
  if (bn < 0) {
    u.u_error = EFBIG;
    if (debugBmap) {
      printf("       bmap = 0\n");
    }
    return 0;
  }
  dev = ip->i_dev;
  rablock = 0;

  /*
   * blocks 0..NADDR-3 are direct blocks
   */
  if (bn < NADDR-2) {
    if (debugBmap) {
      printf("       direct block\n");
    }
    i = bn;
    nb = ip->i_un.i_s1.i_addr[i];
    if (nb == 0) {
      if (rwflg == B_READ || (bp = alloc(dev)) == NULL) {
        if (debugBmap) {
          printf("       bmap = -1\n");
        }
        return -1;
      }
      nb = bp->b_blkno;
      bdwrite(bp);
      ip->i_un.i_s1.i_addr[i] = nb;
      ip->i_flag |= IUPD | ICHG;
    }
    if (i < NADDR-3) {
      rablock = ip->i_un.i_s1.i_addr[i + 1];
    }
    if (debugBmap) {
      printf("       bmap = 0x%08X\n", nb);
    }
    return nb;
  }

  /*
   * Addresses NADDR-2 and NADDR-1 have
   * single and double indirect blocks.
   * The first step is to determine
   * how many levels of indirection.
   */
  sh = 0;
  nb = 1;
  bn -= NADDR-2;
  for (j = 2; j > 0; j--) {
    sh += NSHIFT;
    nb <<= NSHIFT;
    if (bn < nb) {
      break;
    }
    bn -= nb;
  }
  if (j == 0) {
    u.u_error = EFBIG;
    if (debugBmap) {
      printf("       bmap = 0\n");
    }
    return 0;
  }
  if (debugBmap) {
    printf("       %s indirect block\n",
           j == 2 ? "single" : j == 1 ? "double" : "unknown");
  }

  /*
   * fetch the address from the inode
   */
  nb = ip->i_un.i_s1.i_addr[NADDR-j];
  if (nb == 0) {
    if (rwflg == B_READ || (bp = alloc(dev)) == NULL) {
      if (debugBmap) {
        printf("       bmap = -1\n");
      }
      return -1;
    }
    nb = bp->b_blkno;
    bdwrite(bp);
    ip->i_un.i_s1.i_addr[NADDR-j] = nb;
    ip->i_flag |= IUPD | ICHG;
  }

  /*
   * fetch through the indirect blocks
   */
  for (; j <= 2; j++) {
    bp = bread(dev, nb);
    if (bp->b_flags & B_ERROR) {
      brelse(bp);
      if (debugBmap) {
        printf("       bmap = 0\n");
      }
      return 0;
    }
    bap = bp->b_un.b_daddr;
    sh -= NSHIFT;
    i = (bn >> sh) & NMASK;
    nb = bap[i];
    if (nb == 0) {
      if (rwflg == B_READ || (nbp = alloc(dev)) == NULL) {
        brelse(bp);
        if (debugBmap) {
          printf("       bmap = -1\n");
        }
        return -1;
      }
      nb = nbp->b_blkno;
      bdwrite(nbp);
      bap[i] = nb;
      bdwrite(bp);
    } else {
      brelse(bp);
    }
  }

  /*
   * Calculate read-ahead.
   */
  if (i < NINDIR-1) {
    rablock = bap[i + 1];
  }
  if (debugBmap) {
    printf("       bmap = 0x%08X\n", nb);
  }
  return nb;
}


/*
 * Pass back c to the user at his location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * on the last character of the user's read.
 * u_base is in the user address space unless u_segflg
 * is set.
 */
int passc(int c) {
  if (u.u_segflg == 1) {
    *u.u_base = c;
  } else {
    if (subyte(u.u_base, c) < 0) {
      u.u_error = EFAULT;
      return -1;
    }
  }
  u.u_count--;
  u.u_offset++;
  u.u_base++;
  return u.u_count == 0 ? -1 : 0;
}


/*
 * Pick up and return the next character from the user's
 * write call at location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * when u_count is exhausted.  u_base is in the user's
 * address space unless u_segflg is set.
 */
int cpass(void) {
  int c;

  if (u.u_count == 0) {
    return -1;
  }
  if (u.u_segflg == 1) {
    c = *u.u_base;
  } else {
    if ((c = fubyte(u.u_base)) < 0) {
      u.u_error = EFAULT;
      return -1;
    }
  }
  u.u_count--;
  u.u_offset++;
  u.u_base++;
  return c & 0xFF;
}


/*
 * copy count bytes from from to to.
 */
void bcopy(caddr_t from, caddr_t to, int count) {
  char *f, *t;

  if (debugBcopy) {
    printf("-----  bcopy from = 0x%08X, to = 0x%08X, count = %d  -----\n",
           from, to, count);
  }
  f = from;
  t = to;
  do {
    *t++ = *f++;
  } while (--count);
}
