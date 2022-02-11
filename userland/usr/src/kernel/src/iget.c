/*
 * iget.c -- inode handling
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/mount.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/filsys.h"
#include "../include/conf.h"
#include "../include/buf.h"

#include "iget.h"
#include "start.h"
#include "slp.h"
#include "pipe.h"
#include "alloc.h"
#include "klib.h"
#include "rdwri.h"
#include "bio.h"
#include "subr.h"
#include "clock.h"


Bool debugIget = FALSE;
Bool debugIput = FALSE;
Bool debugIupdat = FALSE;
Bool debugItrunc = FALSE;
Bool debugMaknode = FALSE;
Bool debugWdir = FALSE;


/*
 * Copy a disk inode into a memory inode.
 */
static void icopyFromDisk(struct inode *ip, struct dinode *dp) {
  int i;

  ip->i_mode = dp->di_mode;
  ip->i_nlink = dp->di_nlink;
  ip->i_uid = dp->di_uid;
  ip->i_gid = dp->di_gid;
  ip->i_size = dp->di_size;
  for (i = 0; i < NADDR; i++) {
    ip->i_un.i_s1.i_addr[i] = dp->di_addr[i];
  }
}


/*
 * Copy a memory inode into a disk inode.
 */
static void icopyToDisk(struct dinode *dp, struct inode *ip) {
  int i;

  dp->di_mode = ip->i_mode;
  dp->di_nlink = ip->i_nlink;
  dp->di_uid = ip->i_uid;
  dp->di_gid = ip->i_gid;
  dp->di_size = ip->i_size;
  for (i = 0; i < NADDR; i++) {
    dp->di_addr[i] = ip->i_un.i_s1.i_addr[i];
  }
}


/*
 * Look up an inode by device,inumber.
 * If it is in core (in the inode structure),
 * honor the locking protocol.
 * If it is not in core, read it in from the
 * specified device.
 * If the inode is mounted on, perform
 * the indicated indirection.
 * In all cases, a pointer to a locked
 * inode structure is returned.
 *
 * printf warning: no inodes -- if the inode
 *	structure is full
 * panic: no imt -- if the mounted file
 *	system is not in the mount table.
 *	"cannot happen"
 */
struct inode *iget(dev_t dev, ino_t ino) {
	struct inode *ip;
	struct mount *mp;
	struct inode *oip;
	struct buf *bp;
	struct dinode *dp;

  if (debugIget) {
    printf("-----  iget dev = 0x%08X, ino = 0x%08X  -----\n",
           dev, ino);
  }
loop:
	oip = NULL;
	for(ip = &inode[0]; ip < &inode[NINODE]; ip++) {
		if(ino == ip->i_number && dev == ip->i_dev) {
			if((ip->i_flag&ILOCK) != 0) {
				ip->i_flag |= IWANT;
				sleep((caddr_t)ip, PINOD);
				goto loop;
			}
			if((ip->i_flag&IMOUNT) != 0) {
				for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
				if(mp->m_inodp == ip) {
					dev = mp->m_dev;
					ino = ROOTINO;
					goto loop;
				}
				panic("no imt");
			}
			ip->i_count++;
			ip->i_flag |= ILOCK;
			return(ip);
		}
		if(oip==NULL && ip->i_count==0)
			oip = ip;
	}
	ip = oip;
	if(ip == NULL) {
		printf("inode table overflow\n");
		u.u_error = ENFILE;
		return(NULL);
	}
	ip->i_dev = dev;
	ip->i_number = ino;
	ip->i_flag = ILOCK;
	ip->i_count++;
	ip->i_un.i_s1.i_lastr = 0;
	bp = bread(dev, itod(ino));
	/*
	 * Check I/O errors
	 */
	if((bp->b_flags&B_ERROR) != 0) {
		brelse(bp);
		iput(ip);
		return(NULL);
	}
	dp = bp->b_un.b_dino;
	dp += itoo(ino);
	icopyFromDisk(ip, dp);
	brelse(bp);
	return(ip);
}


/*
 * Decrement reference count of
 * an inode structure.
 * On the last reference,
 * write the inode out and if necessary,
 * truncate and deallocate the file.
 */
void iput(struct inode *ip) {
  if (debugIput) {
    printf("-----  iput dev = 0x%08X, ino = 0x%08X  -----\n",
           ip->i_dev, ip->i_number);
  }
	if(ip->i_count == 1) {
		ip->i_flag |= ILOCK;
		if(ip->i_nlink <= 0) {
			itrunc(ip);
			ip->i_mode = 0;
			ip->i_flag |= IUPD|ICHG;
			ifree(ip->i_dev, ip->i_number);
		}
		iupdat(ip, &time, &time);
		prele(ip);
		ip->i_flag = 0;
		ip->i_number = 0;
	}
	ip->i_count--;
	prele(ip);
}


/*
 * Check accessed and update flags on
 * an inode structure.
 * If any are on, update the inode
 * with the current time.
 */
void iupdat(struct inode *ip, time_t *ta, time_t *tm) {
  struct buf *bp;
  struct dinode *dp;

  if (debugIupdat) {
    printf("-----  iupdat dev = 0x%08X, ino = 0x%08X  -----\n",
           ip->i_dev, ip->i_number);
  }
  if ((ip->i_flag & (IUPD | IACC | ICHG)) != 0) {
    if (getfs(ip->i_dev)->s_ronly != 0) {
      return;
    }
    bp = bread(ip->i_dev, itod(ip->i_number));
    if (bp->b_flags & B_ERROR) {
      brelse(bp);
      return;
    }
    dp = bp->b_un.b_dino;
    dp += itoo(ip->i_number);
    icopyToDisk(dp, ip);
    if (ip->i_flag & IACC) {
      dp->di_atime = *ta;
    }
    if (ip->i_flag & IUPD) {
      dp->di_mtime = *tm;
    }
    if (ip->i_flag & ICHG) {
      dp->di_ctime = time;
    }
    ip->i_flag &= ~(IUPD | IACC | ICHG);
    bdwrite(bp);
  }
}


/*
 * Free an indirect block, recursively freeing the referenced
 * indirect blocks, and finally freeing the data blocks. The
 * level of indirection, indirLevel, is 0 for single indirect
 * blocks, 1 for double indirect blocks, etc.
 */
static void freeIndir(dev_t dev, daddr_t bn, int indirLevel) {
	int i;
	struct buf *bp;
	daddr_t *bap;
	daddr_t nb;

	bp = NULL;
	for (i=NINDIR-1; i>=0; i--) {
		if (bp == NULL) {
			bp = bread(dev, bn);
			if (bp->b_flags & B_ERROR) {
				brelse(bp);
				return;
			}
			bap = bp->b_un.b_daddr;
		}
		nb = bap[i];
		if (nb == 0)
			continue;
		if (indirLevel) {
			brelse(bp);
			bp = NULL;
			freeIndir(dev, nb, indirLevel - 1);
		} else
			free(dev, nb);
	}
	if (bp != NULL)
		brelse(bp);
	free(dev, bn);
}


/*
 * Free all the disk blocks associated
 * with the specified inode structure.
 * The blocks of the file are removed
 * in reverse order. This FILO
 * algorithm will tend to maintain
 * a contiguous free list much longer
 * than FIFO.
 */
void itrunc(struct inode *ip) {
	int i;
	dev_t dev;
	daddr_t bn;

  if (debugItrunc) {
    printf("-----  itrunc dev = 0x%08X, ino = 0x%08X  -----\n",
           ip->i_dev, ip->i_number);
  }
	i = ip->i_mode & IFMT;
	if (i != IFREG && i != IFDIR) {
		return;
	}
	dev = ip->i_dev;
	for(i=NADDR-1; i>=0; i--) {
		bn = ip->i_un.i_s1.i_addr[i];
		if(bn == (daddr_t)0)
			continue;
		ip->i_un.i_s1.i_addr[i] = (daddr_t)0;
		switch (i) {

		case NADDR-1:
			/* double indirect block */
			freeIndir(dev, bn, 1);
			break;

		case NADDR-2:
			/* single indirect block */
			freeIndir(dev, bn, 0);
			break;

		default:
			/* direct block */
			free(dev, bn);
			break;

		}
	}
	ip->i_size = 0;
	ip->i_flag |= ICHG|IUPD;
}


/*
 * Make a new file.
 */
struct inode *maknode(int mode) {
	struct inode *ip;

  if (debugMaknode) {
    printf("-----  maknode mode = 0x%08X  -----\n",
           mode);
  }
	ip = ialloc(u.u_pdir->i_dev);
	if(ip == NULL) {
		iput(u.u_pdir);
		return(NULL);
	}
	ip->i_flag |= IACC|IUPD|ICHG;
	if ((mode & IFMT) == 0) {
		mode |= IFREG;
	}
	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_uid;
	ip->i_gid = u.u_gid;
	wdir(ip);
	return(ip);
}


/*
 * Write a directory entry with
 * parameters left as side effects
 * to a call to namei.
 */
void wdir(struct inode *ip) {
  if (debugWdir) {
    printf("-----  wdir dev = 0x%08X, ino = 0x%08X  -----\n",
           ip->i_dev, ip->i_number);
  }
	if (u.u_pdir->i_nlink <= 0) {
		u.u_error = ENOTDIR;
		goto out;
	}
	u.u_dent.d_ino = ip->i_number;
	bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_dent.d_name, DIRSIZ);
	u.u_count = sizeof(struct direct);
	u.u_segflg = 1;
	u.u_base = (caddr_t)&u.u_dent;
	writei(u.u_pdir);
out:
	iput(u.u_pdir);
}
