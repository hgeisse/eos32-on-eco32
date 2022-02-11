/*
 * alloc.c -- manage free inodes and disk blocks
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/mount.h"
#include "../include/filsys.h"
#include "../include/fblk.h"
#include "../include/conf.h"
#include "../include/buf.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/ino.h"
#include "../include/dir.h"
#include "../include/user.h"

#include "alloc.h"
#include "start.h"
#include "klib.h"
#include "subr.h"
#include "iget.h"
#include "bio.h"
#include "slp.h"
#include "clock.h"


typedef	struct fblk *FBLKP;


Bool debugIinit = FALSE;
Bool debugAlloc = FALSE;
Bool debugFree = FALSE;
Bool debugIalloc = FALSE;
Bool debugIfree = FALSE;
Bool debugUpdate = FALSE;


struct mount mount[NMOUNT];
int updlock;


/*
 * Show the contents of a super block.
 */
void showSuperBlock(char *header, struct filsys *fp) {
  int i;

  if (header != NULL) {
    printf("-----  %s  -----\n", header);
  }
  printf("    magic  = 0x%08X\n", fp->s_magic);
  printf("    fsize  = %ld\n", fp->s_fsize);
  printf("    isize  = %ld\n", fp->s_isize);
  printf("    tfree  = %ld\n", fp->s_tfree);
  printf("    tinode = %d\n", fp->s_tinode);
  printf("    ninode = %d\n", fp->s_ninode);
  for (i = 0; i < 5; i++) {
    if (i == fp->s_ninode) {
      break;
    }
    printf("        inode[%d] = %d\n", i, fp->s_inode[i]);
  }
  if (fp->s_ninode > 5) {
    printf("        ...\n");
  }
  printf("    nfree  = %d\n", fp->s_nfree);
  for (i = 0; i < 5; i++) {
    if (i == fp->s_nfree) {
      break;
    }
    printf("        free[%d] = %ld\n", i, fp->s_free[i]);
  }
  if (fp->s_nfree > 5) {
    printf("        ...\n");
  }
  printf("    time   = %ld\n", fp->s_time);
  printf("    flock  = %d\n", (int) fp->s_flock);
  printf("    ilock  = %d\n", (int) fp->s_ilock);
  printf("    fmod   = %d\n", (int) fp->s_fmod);
  printf("    ronly  = %d\n", (int) fp->s_ronly);
}


/*
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root's super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an I/O error.
 */
void iinit(void) {
	struct buf *cp, *bp;
	struct filsys *fp;

  if (debugIinit) {
    printf("-----  iinit  -----\n");
  }
	(*bdevsw[major(rootdev)].d_open)(rootdev, 1);
	bp = bread(rootdev, SUPERB);
	cp = geteblk();
	if (u.u_error) {
		panic("error reading super block of root file system");
	}
	bcopy(bp->b_un.b_addr, cp->b_un.b_addr, sizeof(struct filsys));
	brelse(bp);
	mount[0].m_bufp = cp;
	mount[0].m_dev = rootdev;
	fp = cp->b_un.b_filsys;
	time = fp->s_time;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_fmod = 0;
	fp->s_ronly = 0;
  if (debugIinit) {
    showSuperBlock("super block of root file system", fp);
  }
}


/*
 * alloc will obtain the next available
 * free disk block from the free list of
 * the specified device.
 * The super block has up to NICFREE remembered
 * free blocks; the last of these is read to
 * obtain NICFREE more . . .
 *
 * no space on dev x/y -- when
 * the free list is exhausted.
 */
struct buf *alloc(dev_t dev) {
	daddr_t bno;
	struct filsys *fp;
	struct buf *bp;

  if (debugAlloc) {
    printf("-----  alloc dev = 0x%08X  -----\n", dev);
  }
	fp = getfs(dev);
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	do {
		if(fp->s_nfree <= 0)
			goto nospace;
		if (fp->s_nfree > NICFREE) {
			prdev("bad free count", dev);
			goto nospace;
		}
		bno = fp->s_free[--fp->s_nfree];
		if(bno == 0)
			goto nospace;
	} while (badblock(fp, bno, dev));
	if(fp->s_nfree <= 0) {
		fp->s_flock++;
		bp = bread(dev, bno);
		if ((bp->b_flags&B_ERROR) == 0) {
			fp->s_nfree = ((FBLKP)(bp->b_un.b_addr))->df_nfree;
			bcopy((caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			    (caddr_t)fp->s_free, sizeof(fp->s_free));
		}
		brelse(bp);
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
		if (fp->s_nfree <=0)
			goto nospace;
	}
	bp = getblk(dev, bno);
	clrbuf(bp);
	fp->s_fmod = 1;
	if (debugAlloc) {
	  printf("       alloc = 0x%08X\n", bp->b_blkno);
	}
	return(bp);

nospace:
	fp->s_nfree = 0;
	prdev("no space", dev);
	u.u_error = ENOSPC;
	return(NULL);
}


/*
 * place the specified disk block
 * back on the free list of the
 * specified device.
 */
void free(dev_t dev, daddr_t bno) {
	struct filsys *fp;
	struct buf *bp;

  if (debugFree) {
    printf("-----  free dev = 0x%08X, bno = 0x%08X  -----\n", dev, bno);
  }
	fp = getfs(dev);
	fp->s_fmod = 1;
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	if (badblock(fp, bno, dev))
		return;
	if(fp->s_nfree <= 0) {
		fp->s_nfree = 1;
		fp->s_free[0] = 0;
	}
	if(fp->s_nfree >= NICFREE) {
		fp->s_flock++;
		bp = getblk(dev, bno);
		((FBLKP)(bp->b_un.b_addr))->df_nfree = fp->s_nfree;
		bcopy((caddr_t)fp->s_free,
			(caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			sizeof(fp->s_free));
		fp->s_nfree = 0;
		bwrite(bp);
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
	}
	fp->s_free[fp->s_nfree++] = bno;
	fp->s_fmod = 1;
}


/*
 * Check that a block number is in the
 * range between the I list and the size
 * of the device.
 * This is used mainly to check that a
 * garbage file system has not been mounted.
 *
 * bad block on dev x/y -- not in range
 */
int badblock(struct filsys *fp, daddr_t bn, dev_t dev) {

	if (bn < SUPERB + 1 + fp->s_isize || bn >= fp->s_fsize) {
		prdev("bad block", dev);
		return(1);
	}
	return(0);
}


/*
 * Allocate an unused I node
 * on the specified device.
 * Used with file creation.
 * The algorithm keeps up to
 * NICINOD spare I nodes in the
 * super block. When this runs out,
 * a linear search through the
 * I list is instituted to pick
 * up NICINOD more.
 */
struct inode *ialloc(dev_t dev) {
	struct filsys *fp;
	struct buf *bp;
	struct inode *ip;
	int i;
	struct dinode *dp;
	ino_t ino;
	int index;
	daddr_t adr;

  if (debugIalloc) {
    printf("-----  ialloc dev = 0x%08X  -----\n", dev);
  }
	fp = getfs(dev);
	while(fp->s_ilock)
		sleep((caddr_t)&fp->s_ilock, PINOD);
loop:
	if(fp->s_ninode > 0) {
		ino = fp->s_inode[--fp->s_ninode];
		if (ino < ROOTINO)
			goto loop;
		ip = iget(dev, ino);
		if(ip == NULL)
			return(NULL);
		if(ip->i_mode == 0) {
			for (i=0; i<NADDR; i++)
				ip->i_un.i_s1.i_addr[i] = 0;
			fp->s_fmod = 1;
			if (debugIalloc) {
			  printf("       ialloc = 0x%08X\n", ip->i_number);
			}
			return(ip);
		}
		/*
		 * Inode was allocated after all.
		 * Look some more.
		 */
		iput(ip);
		goto loop;
	}
	fp->s_ilock++;
	ino = 0;  /* HG: we start numbering inodes with 0 */
	index = NICINOD;  /* HG: start filling at upper end */
	for(adr = SUPERB + 1; adr < SUPERB + 1 + fp->s_isize; adr++) {
		bp = bread(dev, adr);
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			ino += INOPB;
			continue;
		}
		dp = bp->b_un.b_dino;
		for(i=0; i<INOPB; i++) {
			if(dp->di_mode != 0)
				goto cont;
			for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
			if(dev==ip->i_dev && ino==ip->i_number)
				goto cont;
			fp->s_inode[--index] = ino;
			fp->s_ninode++;
			if(fp->s_ninode >= NICINOD)
				break;
		cont:
			ino++;
			dp++;
		}
		brelse(bp);
		if(fp->s_ninode >= NICINOD)
			break;
	}
	/* HG: if the list is not completely filled, shift entries down */
	if (index > 0) {
	  for (i = 0; i < fp->s_ninode; i++) {
	    fp->s_inode[i] = fp->s_inode[index++];
	  }
	}
	fp->s_ilock = 0;
	wakeup((caddr_t)&fp->s_ilock);
	if(fp->s_ninode > 0)
		goto loop;
	prdev("out of inodes", dev);
	u.u_error = ENOSPC;
	return(NULL);
}


/*
 * Free the specified I node
 * on the specified device.
 * The algorithm stores up
 * to NICINOD I nodes in the super
 * block and throws away any more.
 */
void ifree(dev_t dev, ino_t ino) {
	struct filsys *fp;

  if (debugIfree) {
    printf("-----  ifree dev = 0x%08X, ino = 0x%08X  -----\n", dev, ino);
  }
	fp = getfs(dev);
	if(fp->s_ilock)
		return;
	if(fp->s_ninode >= NICINOD)
		return;
	fp->s_inode[fp->s_ninode++] = ino;
	fp->s_fmod = 1;
}


/*
 * getfs maps a device number into
 * a pointer to the incore super
 * block.
 * The algorithm is a linear
 * search through the mount table.
 * A consistency check of the
 * in core free-block and i-node
 * counts.
 *
 * bad count on dev x/y -- the count
 *	check failed. At this point, all
 *	the counts are zeroed which will
 *	almost certainly lead to "no space"
 *	diagnostic
 * panic: no fs -- the device is not mounted.
 *	this "cannot happen"
 */
struct filsys *getfs(dev_t dev) {
	struct mount *mp;
	struct filsys *fp;

	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
	if(mp->m_bufp != NULL && mp->m_dev == dev) {
		fp = mp->m_bufp->b_un.b_filsys;
		if(fp->s_nfree > NICFREE || fp->s_ninode > NICINOD) {
			prdev("bad count", dev);
			fp->s_nfree = 0;
			fp->s_ninode = 0;
		}
		return(fp);
	}
	panic("no fs");
	return(NULL);
}


/*
 * update is the internal name of
 * 'sync'. It goes through the disk
 * queues to initiate sandbagged I/O;
 * goes through the I nodes to write
 * modified nodes; and it goes through
 * the mount table to initiate modified
 * super blocks.
 */
void update(void) {
	struct inode *ip;
	struct mount *mp;
	struct buf *bp;
	struct filsys *fp;

  if (debugUpdate) {
    printf("-----  update  -----\n");
  }
	if(updlock)
		return;
	updlock++;
	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if(mp->m_bufp != NULL) {
			fp = mp->m_bufp->b_un.b_filsys;
			if(fp->s_fmod==0 || fp->s_ilock!=0 ||
			   fp->s_flock!=0 || fp->s_ronly!=0)
				continue;
			bp = getblk(mp->m_dev, SUPERB);
			if (bp->b_flags & B_ERROR)
				continue;
			fp->s_fmod = 0;
			fp->s_time = time;
			bcopy((caddr_t)fp, bp->b_un.b_addr, BSIZE);
			bwrite(bp);
		}
	for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
		if((ip->i_flag&ILOCK)==0 && ip->i_count) {
			ip->i_flag |= ILOCK;
			ip->i_count++;
			iupdat(ip, &time, &time);
			iput(ip);
		}
	updlock = 0;
	bflush(NODEV);
}
