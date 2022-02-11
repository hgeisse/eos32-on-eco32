/*
 * alloc.h -- manage free inodes and disk blocks
 */

#ifndef _ALLOC_H_
#define _ALLOC_H_

extern Bool debugIinit;
extern Bool debugAlloc;
extern Bool debugFree;
extern Bool debugIalloc;
extern Bool debugIfree;
extern Bool debugUpdate;

void iinit(void);
struct buf *alloc(dev_t dev);
void free(dev_t dev, daddr_t bno);
int badblock(struct filsys *fp, daddr_t bn, dev_t dev);
struct inode *ialloc(dev_t dev);
void ifree(dev_t dev, ino_t ino);
struct filsys *getfs(dev_t dev);
void update(void);

#endif /* _ALLOC_H_ */
