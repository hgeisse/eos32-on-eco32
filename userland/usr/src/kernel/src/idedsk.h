/*
 * idedsk.h -- IDE disk driver
 */


#ifndef _IDEDSK_H_
#define _IDEDSK_H_


#define EOS32_FSYS	0x0058	/* partition id for EOS32 file system */
#define EOS32_SWAP	0x0059	/* partition id for EOS32 swap space */

#define SECTOR_SIZE	512
#define SPB		(BSIZE / SECTOR_SIZE)
#define WPB		(BSIZE / sizeof(unsigned int))
#define WPS		(SECTOR_SIZE / sizeof(unsigned int))

#define DISK_BASE	((unsigned *) 0xF0400000)  /* disk base address */
#define DISK_CTRL	(DISK_BASE + 0)		/* control/status register */
#define DISK_CNT	(DISK_BASE + 1)		/* sector count register */
#define DISK_SCT	(DISK_BASE + 2)		/* disk sector register */
#define DISK_CAP	(DISK_BASE + 3)		/* disk capacity register */
#define DISK_BUFFER	((unsigned *) 0xF0480000)  /* address of disk buffer */

#define DISK_CTRL_STRT	0x01U	/* a 1 written here starts the disk command */
#define DISK_CTRL_IEN	0x02U	/* enable disk interrupt */
#define DISK_CTRL_WRT	0x04U	/* command type: 0 = read, 1 = write */
#define DISK_CTRL_ERR	0x08U	/* 0 = ok, 1 = error; valid when DONE = 1 */
#define DISK_CTRL_DONE	0x10U	/* 1 = disk has finished the command */
#define DISK_CTRL_READY	0x20U	/* 1 = capacity valid, disk accepts command */

#define DISK_IRQ	8	/* disk interrupt number */

#define READY_RETRIES	1000000	/* retries to wait for disk to get ready */


extern Bool debugIdeDisk;

extern struct buf ideTab;


int ideSize(dev_t dev);
void ideOpen(dev_t dev, int flag);
void ideClose(dev_t dev, int flag);
void ideStrategy(struct buf *bp);
void ideRead(dev_t dev);
void ideWrite(dev_t dev);


#endif /* _IDEDSK_H_ */
