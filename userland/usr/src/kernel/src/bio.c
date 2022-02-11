/*
 * bio.c -- buffer I/O
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
#include "../include/buf.h"
#include "../include/conf.h"
#include "../include/proc.h"
#include "../include/seg.h"

#include "bio.h"
#include "klib.h"
#include "machdep.h"
#include "slp.h"

Bool debugBinit = FALSE;
Bool debugBread = FALSE;
Bool debugBreada = FALSE;
Bool debugBwrite = FALSE;
Bool debugBdwrite = FALSE;
Bool debugBawrite = FALSE;
Bool debugBrelse = FALSE;
Bool debugGetblk = FALSE;
Bool debugGeteblk = FALSE;
Bool debugIodone = FALSE;
Bool debugClrbuf = FALSE;
Bool debugSwap = FALSE;
Bool debugBflush = FALSE;
Bool debugPhysio = FALSE;

/*
 * This is the pool of buffer headers.
 */
struct buf buf[NBUF];

/*
 * swap I/O headers.
 * they are filled in to point
 * at the desired I/O operation.
 */
struct buf swbuf1;
struct buf swbuf2;

/*
 * This is the set of buffers proper, whose heads
 * were declared in buf.h.  There can exist buffer
 * headers not pointing here that are used purely
 * as arguments to the I/O routines to describe
 * I/O to be done-- e.g. swbuf for swapping.
 *
 * Buffers must be aligned on word boundaries in
 * order to allow block transfers by words.
 */
unsigned int buffers[NBUF][BSIZE / sizeof(unsigned int)];

/*
 * The following several routines allocate and free
 * buffers with various side effects.  In general the
 * arguments to an allocate routine are a device and
 * a block number, and the value is a pointer to
 * to the buffer header; the buffer is marked "busy"
 * so that no one else can touch it.  If the block was
 * already in core, no I/O need be done; if it is
 * already busy, the process waits until it becomes free.
 * The following routines allocate a buffer:
 *	getblk
 *	bread
 *	breada
 * Eventually the buffer must be released, possibly with the
 * side effect of writing it out, by using one of
 *	bwrite
 *	bdwrite
 *	bawrite
 *	brelse
 */

/*
 * Initialize the buffer I/O system by freeing all
 * buffers and setting all device buffer lists to empty.
 */
void binit(void)
{
	register struct buf *bp;
	register struct buf *dp;
	register int i;
	struct bdevsw *bdp;

  if (debugBinit) {
    printf("-----  binit  -----\n");
  }
	bfreelist.b_forw = bfreelist.b_back =
	    bfreelist.av_forw = bfreelist.av_back = &bfreelist;
	for (i=0; i<NBUF; i++) {
		bp = &buf[i];
		bp->b_dev = NODEV;
		bp->b_un.b_addr = (char *) buffers[i];
		bp->b_back = &bfreelist;
		bp->b_forw = bfreelist.b_forw;
		bfreelist.b_forw->b_back = bp;
		bfreelist.b_forw = bp;
		bp->b_flags = B_BUSY;
		brelse(bp);
	}
	for (bdp = bdevsw; bdp->d_open; bdp++) {
		dp = bdp->d_tab;
		if(dp) {
			dp->b_forw = dp;
			dp->b_back = dp;
		}
		nblkdev++;
	}
}

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */
struct buf *bread(dev_t dev, daddr_t blkno)
{
	register struct buf *bp;

  if (debugBread) {
    printf("-----  bread dev = 0x%08X, blk = 0x%08X  -----\n",
           dev, blkno);
  }
	bp = getblk(dev, blkno);
	if (bp->b_flags&B_DONE) {
		return(bp);
	}
	bp->b_flags |= B_READ;
	bp->b_bcount = BSIZE;
	(*bdevsw[major(dev)].d_strategy)(bp);
	iowait(bp);
	return(bp);
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
struct buf *breada(dev_t dev, daddr_t blkno, daddr_t rablkno)
{
	register struct buf *bp, *rabp;

  if (debugBreada) {
    printf("-----  breada dev = 0x%08X, blk = 0x%08X, rablk = 0x%08X  -----\n",
           dev, blkno, rablkno);
  }
	bp = NULL;
	if (!incore(dev, blkno)) {
		bp = getblk(dev, blkno);
		if ((bp->b_flags&B_DONE) == 0) {
			bp->b_flags |= B_READ;
			bp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(bp);
		}
	}
	if (rablkno && !incore(dev, rablkno)) {
		rabp = getblk(dev, rablkno);
		if (rabp->b_flags & B_DONE)
			brelse(rabp);
		else {
			rabp->b_flags |= B_READ|B_ASYNC;
			rabp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(rabp);
		}
	}
	if(bp == NULL)
		return(bread(dev, blkno));
	iowait(bp);
	return(bp);
}

/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
void bwrite(struct buf *bp)
{
	register flag;

  if (debugBwrite) {
    printf("-----  bwrite dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	flag = bp->b_flags;
	bp->b_flags &= ~(B_READ | B_DONE | B_ERROR | B_DELWRI | B_AGE);
	bp->b_bcount = BSIZE;
	(*bdevsw[major(bp->b_dev)].d_strategy)(bp);
	if ((flag&B_ASYNC) == 0) {
		iowait(bp);
		brelse(bp);
	} else if (flag & B_DELWRI)
		bp->b_flags |= B_AGE;
	else
		geterror(bp);
}

/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * This can't be done for magtape, since writes must be done
 * in the same order as requested.
 */
void bdwrite(struct buf *bp)
{
	register struct buf *dp;

  if (debugBdwrite) {
    printf("-----  bdwrite dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	dp = bdevsw[major(bp->b_dev)].d_tab;
	if(dp->b_flags & B_TAPE)
		bawrite(bp);
	else {
		bp->b_flags |= B_DELWRI | B_DONE;
		brelse(bp);
	}
}

/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
void bawrite(struct buf *bp)
{

  if (debugBawrite) {
    printf("-----  bawrite dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	bp->b_flags |= B_ASYNC;
	bwrite(bp);
}

/*
 * release the buffer, with no I/O implied.
 */
void brelse(struct buf *bp)
{
	register struct buf **backp;
	register s;

  if (debugBrelse) {
    printf("-----  brelse dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	if (bfreelist.b_flags&B_WANTED) {
		bfreelist.b_flags &= ~B_WANTED;
		wakeup((caddr_t)&bfreelist);
	}
	if (bp->b_flags&B_ERROR)
		bp->b_dev = NODEV;  /* no assoc. on error */
	s = spl6();
	if(bp->b_flags & B_AGE) {
		backp = &bfreelist.av_forw;
		(*backp)->av_back = bp;
		bp->av_forw = *backp;
		*backp = bp;
		bp->av_back = &bfreelist;
	} else {
		backp = &bfreelist.av_back;
		(*backp)->av_forw = bp;
		bp->av_back = *backp;
		*backp = bp;
		bp->av_forw = &bfreelist;
	}
	bp->b_flags &= ~(B_WANTED|B_BUSY|B_ASYNC|B_AGE);
	splx(s);
}

/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */
int incore(dev_t dev, daddr_t blkno)
{
	register struct buf *bp;
	register struct buf *dp;

	dp = bdevsw[major(dev)].d_tab;
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw)
		if (bp->b_blkno==blkno && bp->b_dev==dev)
			return(1);
	return(0);
}

/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 */
struct buf *getblk(dev_t dev, daddr_t blkno)
{
	register struct buf *bp;
	register struct buf *dp;

  if (debugGetblk) {
    printf("-----  getblk dev = 0x%08X, blk = 0x%08X  -----\n",
           dev, blkno);
  }
	if(major(dev) >= nblkdev)
		panic("blkdev");

    loop:
	spl0();
	dp = bdevsw[major(dev)].d_tab;
	if(dp == NULL)
		panic("devtab");
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw) {
		if (bp->b_blkno!=blkno || bp->b_dev!=dev)
			continue;
		spl6();
		if (bp->b_flags&B_BUSY) {
			bp->b_flags |= B_WANTED;
			sleep((caddr_t)bp, PRIBIO+1);
			goto loop;
		}
		spl0();
		notavail(bp);
		return(bp);
	}
	spl6();
	if (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep((caddr_t)&bfreelist, PRIBIO+1);
		goto loop;
	}
	spl0();
	notavail(bp = bfreelist.av_forw);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = dev;
	bp->b_blkno = blkno;
	return(bp);
}

/*
 * get an empty block,
 * not assigned to any particular device
 */
struct buf *geteblk(void)
{
	register struct buf *bp;
	register struct buf *dp;

  if (debugGeteblk) {
    printf("-----  geteblk  -----\n");
  }
loop:
	spl6();
	while (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep((caddr_t)&bfreelist, PRIBIO+1);
	}
	spl0();
	dp = &bfreelist;
	notavail(bp = bfreelist.av_forw);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = (dev_t)NODEV;
	return(bp);
}

/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
void iowait(struct buf *bp)
{

	spl6();
	while ((bp->b_flags&B_DONE)==0)
		sleep((caddr_t)bp, PRIBIO);
	spl0();
	geterror(bp);
}

/*
 * Unlink a buffer from the available list and mark it busy.
 * (internal interface)
 */
void notavail(struct buf *bp)
{
	register s;

	s = spl6();
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	bp->b_flags |= B_BUSY;
	splx(s);
}

/*
 * Mark I/O complete on a buffer, release it if I/O is asynchronous,
 * and wake up anyone waiting for it.
 */
void iodone(struct buf *bp)
{

  if (debugIodone) {
    printf("-----  iodone dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	bp->b_flags |= B_DONE;
	if (bp->b_flags&B_ASYNC)
		brelse(bp);
	else {
		bp->b_flags &= ~B_WANTED;
		wakeup((caddr_t)bp);
	}
}

/*
 * Zero the core associated with a buffer.
 */
void clrbuf(struct buf *bp)
{
	register *p;
	register c;

  if (debugClrbuf) {
    printf("-----  clrbuf dev = 0x%08X, blk = 0x%08X  -----\n",
           bp->b_dev, bp->b_blkno);
  }
	p = bp->b_un.b_words;
	c = BSIZE/sizeof(int);
	do
		*p++ = 0;
	while (--c);
	bp->b_resid = 0;
}

/*
 * swap I/O
 */
void swap(int blkno, int coreaddr, int count, int rdflg)
{
	register struct buf *bp;

  if (debugSwap) {
    printf("-----  swap blk = 0x%08X, addr = 0x%08X, ",
           blkno, coreaddr);
    printf("count = 0x%08X, rdflg = %d  -----\n",
           count, rdflg);
  }
	bp = &swbuf1;
	if(bp->b_flags & B_BUSY)
		if((swbuf2.b_flags&B_WANTED) == 0)
			bp = &swbuf2;
	spl6();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PSWP+1);
	}
	while (count) {
		bp->b_flags = B_BUSY | rdflg;
		bp->b_dev = swapdev;
		bp->b_bcount = BSIZE;
		bp->b_blkno = blkno;
		bp->b_un.b_addr = (caddr_t)FRAME2ADDR(coreaddr);
		(*bdevsw[major(swapdev)].d_strategy)(bp);
		spl6();
		while((bp->b_flags&B_DONE)==0)
			sleep((caddr_t)bp, PSWP);
		count--;
		coreaddr++;
		blkno += PAGES2BLOCKS(1);
	}
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	spl0();
	bp->b_flags &= ~(B_BUSY|B_WANTED);
	if (bp->b_flags & B_ERROR)
		panic("I/O err in swap");
}

/*
 * make sure all write-behind blocks
 * on dev (or NODEV for all)
 * are flushed out.
 * (from umount and update)
 */
void bflush(dev_t dev)
{
	register struct buf *bp;

  if (debugBflush) {
    printf("-----  bflush dev = 0x%08X  -----\n",
           dev);
  }
loop:
	spl6();
	for (bp = bfreelist.av_forw; bp != &bfreelist; bp = bp->av_forw) {
		if (bp->b_flags&B_DELWRI && (dev == NODEV||dev==bp->b_dev)) {
			bp->b_flags |= B_ASYNC;
			notavail(bp);
			bwrite(bp);
			goto loop;
		}
	}
	spl0();
}

/*
 * Raw I/O. The arguments are
 *	The strategy routine for the device
 *	A buffer, which will always be a special buffer
 *	  header owned exclusively by the device for this purpose
 *	The device number
 *	Read/write flag
 * Essentially all the work is computing physical addresses and
 * validating them.
 */
void physio(void (*strat)(struct buf *), struct buf *bp, dev_t dev, int rw)
{
	register unsigned base;
	register int nb;
	int ts;

  if (debugPhysio) {
    printf("-----  physio strat = 0x%08X, dev = 0x%08X, rw = %d  -----\n",
           strat, dev, rw);
  }
	base = (unsigned)u.u_base;
	/*
	 * Check odd base, odd count, and address wraparound
	 */
	if (base&01 || u.u_count&01 || base>=base+u.u_count)
		goto bad;
	ts = (u.u_tsize+127) & ~0177U;
	if (u.u_sep)
		ts = 0;
	nb = (base>>6) & 01777;
	/*
	 * Check overlap with text. (ts and nb now
	 * in 64-byte clicks)
	 */
	if (nb < ts)
		goto bad;
	/*
	 * Check that transfer is either entirely in the
	 * data or in the stack: that is, either
	 * the end is in the data or the start is in the stack
	 * (remember wraparound was already checked).
	 */
	if ((((base+u.u_count)>>6)&01777) >= ts+u.u_dsize
	    && nb < 1024-u.u_ssize)
		goto bad;
	spl6();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO+1);
	}
	bp->b_flags = B_BUSY | rw;
	bp->b_dev = dev;
	/*
	 * Compute physical address by simulating
	 * the segmentation hardware.
	 */
	ts = (u.u_sep? UDSA: UISA)->r[nb>>7] + (nb&0177);
	bp->b_un.b_addr = (caddr_t)((ts<<6) + (base&077));
	bp->b_xmem = (ts>>10) & 077;
	bp->b_blkno = u.u_offset >> BSHIFT;
	bp->b_bcount = u.u_count;
	bp->b_error = 0;
	u.u_procp->p_flag |= SLOCK;
	(*strat)(bp);
	spl6();
	while ((bp->b_flags&B_DONE) == 0)
		sleep((caddr_t)bp, PRIBIO);
	u.u_procp->p_flag &= ~SLOCK;
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	spl0();
	bp->b_flags &= ~(B_BUSY|B_WANTED);
	u.u_count = bp->b_resid;
	geterror(bp);
	return;
    bad:
	u.u_error = EFAULT;
}

/*
 * Pick up the device's error number and pass it to the user;
 * if there is an error but the number is 0 set a generalized
 * code.  Actually the latter is always true because devices
 * don't yet return specific errors.
 */
void geterror(struct buf *bp)
{

	if (bp->b_flags&B_ERROR)
		if ((u.u_error = bp->b_error)==0)
			u.u_error = EIO;
}
