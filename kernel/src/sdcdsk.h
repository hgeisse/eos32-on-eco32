/*
 * sdcdsk.h -- SD card disk driver
 */


#ifndef _SDCDSK_H_
#define _SDCDSK_H_


#define EOS32_FSYS	0x0058	/* partition id for EOS32 file system */
#define EOS32_SWAP	0x0059	/* partition id for EOS32 swap space */

#define SECTOR_SIZE	512
#define SPB		(BSIZE / SECTOR_SIZE)
#define WPB		(BSIZE / sizeof(unsigned int))
#define WPS		(SECTOR_SIZE / sizeof(unsigned int))


extern Bool debugSdcDisk;

extern struct buf sdcTab;


int sdcGetRoot(unsigned startSector, unsigned numSectors);
int sdcGetSwap(void);

int sdcSize(dev_t dev);
void sdcOpen(dev_t dev, int flag);
void sdcClose(dev_t dev, int flag);
void sdcStrategy(struct buf *bp);
void sdcRead(dev_t dev);
void sdcWrite(dev_t dev);


#endif /* _SDCDSK_H_ */
