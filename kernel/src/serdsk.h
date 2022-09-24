/*
 * serdsk.h -- serial line disk driver
 */


#ifndef _SERDSK_H_
#define _SERDSK_H_


#define EOS32_FSYS	0x0058	/* partition id for EOS32 file system */
#define EOS32_SWAP	0x0059	/* partition id for EOS32 swap space */

#define SECTOR_SIZE	512
#define SPB		(BSIZE / SECTOR_SIZE)
#define WPB		(BSIZE / sizeof(unsigned int))
#define WPS		(SECTOR_SIZE / sizeof(unsigned int))


extern Bool debugSerDisk;

extern struct buf serTab;


int serGetRoot(unsigned startSector, unsigned numSectors);
int serGetSwap(void);

int serSize(dev_t dev);
void serOpen(dev_t dev, int flag);
void serClose(dev_t dev, int flag);
void serStrategy(struct buf *bp);
void serRead(dev_t dev);
void serWrite(dev_t dev);


#endif /* _SERDSK_H_ */
