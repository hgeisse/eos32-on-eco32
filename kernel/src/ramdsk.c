/*
 * ramdsk.c -- RAM disk driver
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/buf.h"

#include "klib.h"
#include "ramdsk.h"
#include "bio.h"


/*
 * Debugging.
 */
Bool debugRamDisk = FALSE;


/*
 * This buffer head holds the queue of pending disk tasks.
 */
struct buf ramTab;


/*
 * Determine the size of the device in blocks.
 */
int ramSize(dev_t dev) {
  if (debugRamDisk) {
    printf("-----  ramSize dev = 0x%08X  -----\n",
           dev);
  }
  return RAMDSK_SIZE;
}


/*
 * Open the device.
 */
void ramOpen(dev_t dev, int flag) {
  if (debugRamDisk) {
    printf("-----  ramOpen dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
}


/*
 * Close the device.
 */
void ramClose(dev_t dev, int flag) {
  if (debugRamDisk) {
    printf("-----  ramClose dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
}


/*
 * Copy a disk buffer; buffer and RAM disk block are int aligned.
 */
static void copyBuffer(int *dst, int *src) {
  int count;

  count = BSIZE / sizeof(unsigned int);
  while (count--) {
    *dst++ = *src++;
  }
}


/*
 * This routine is called once for every block on which
 * an I/O operation needs to be done.
 */
void ramStrategy(struct buf *bp) {
  char *ramPtr;

  if (debugRamDisk) {
    printf("-----  ramStrategy dev = 0x%08X, blk = 0x%08X, %s  -----\n",
           bp->b_dev, bp->b_blkno, bp->b_flags & B_READ ? "read" : "write");
  }
  ramPtr = RAMDSK_BASE + (bp->b_blkno << BSHIFT);
  if (bp->b_flags & B_READ) {
    copyBuffer((int *) bp->b_un.b_addr, (int *) ramPtr);
  } else {
    copyBuffer((int *) ramPtr, (int *) bp->b_un.b_addr);
  }
  /* hand buffer back to buffer cache */
  iodone(bp);
}
