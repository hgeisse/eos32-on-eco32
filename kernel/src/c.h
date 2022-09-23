/*
 * c.h -- machine configuration
 */

#ifndef _C_H_
#define _C_H_

extern struct bdevsw bdevsw[];
extern struct cdevsw cdevsw[];
extern struct linesw linesw[];

extern unsigned bootDisk;
extern unsigned startSector;
extern unsigned numSectors;

extern dev_t    rootdev;
extern dev_t    swapdev;
extern dev_t    pipedev;
extern int      nldisp;

extern struct file   file[NFILE];
extern struct inode  inode[NINODE];
extern struct text   text[NTEXT];
extern struct buf    bfreelist;
extern struct acct   acctbuf;
extern struct inode  *acctp;

#endif /* _C_H_ */
