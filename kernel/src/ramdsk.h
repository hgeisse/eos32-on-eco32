/*
 * ramdsk.h -- RAM disk driver
 */


#ifndef _RAMDSK_H_
#define _RAMDSK_H_


#define RAMDSK_SIZE	((2 * M) / BSIZE)
#define RAMDSK_BASE	PHYS2DIRECT(2 * M)


extern Bool debugRamDisk;

extern struct buf ramTab;


int ramSize(dev_t dev);
void ramOpen(dev_t dev, int flag);
void ramClose(dev_t dev, int flag);
void ramStrategy(struct buf *bp);


#endif /* _RAMDSK_H_ */
