/*
 * blk.h -- disk block data type
 */


#ifndef _BLK_H_
#define _BLK_H_


#define	BSHIFT	12		/* LOG2(BSIZE) */
#define	BSIZE	(1 << BSHIFT)	/* size of disk block (bytes) */
#define	BMASK	(BSIZE - 1)	/* BSIZE-1 */


/* representation of a disk block number */
typedef long daddr_t;


#endif /* _BLK_H_ */
