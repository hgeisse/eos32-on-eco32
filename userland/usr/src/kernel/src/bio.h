/*
 * bio.h -- buffer I/O
 */


#ifndef _BIO_H_
#define _BIO_H_


extern Bool debugBinit;
extern Bool debugBread;
extern Bool debugBreada;
extern Bool debugBwrite;
extern Bool debugBdwrite;
extern Bool debugBawrite;
extern Bool debugBrelse;
extern Bool debugGetblk;
extern Bool debugGeteblk;
extern Bool debugIodone;
extern Bool debugClrbuf;
extern Bool debugSwap;
extern Bool debugBflush;
extern Bool debugPhysio;


void binit(void);
struct buf *bread(dev_t dev, daddr_t blkno);
struct buf *breada(dev_t dev, daddr_t blkno, daddr_t rablkno);
void bwrite(struct buf *bp);
void bdwrite(struct buf *bp);
void bawrite(struct buf *bp);
void brelse(struct buf *bp);
int incore(dev_t dev, daddr_t blkno);
struct buf *getblk(dev_t dev, daddr_t blkno);
struct buf *geteblk(void);
void iowait(struct buf *bp);
void notavail(struct buf *bp);
void iodone(struct buf *bp);
void clrbuf(struct buf *bp);
void swap(int blkno, int coreaddr, int count, int rdflg);
void bflush(dev_t dev);
void physio(void (*strat)(struct buf *), struct buf *bp, dev_t dev, int rw);
void geterror(struct buf *bp);


#endif /* _BIO_H_ */
