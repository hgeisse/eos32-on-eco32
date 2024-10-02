/*
 * idedsk.c -- IDE disk driver
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/buf.h"
#include "../include/conf.h"
#include "../include/dir.h"
#include "../include/user.h"

#include "idedsk.h"
#include "bio.h"
#include "klib.h"
#include "start.h"
#include "machdep.h"
#include "c.h"


/*
 * Debugging.
 */
Bool debugIdeDisk = TRUE;


/*
 * This buffer head holds the queue of pending disk tasks.
 */
struct buf ideTab;


/*
 * Has the disk already been initialized?
 */
static Bool ideInitialized = FALSE;


/**************************************************************/


/*
 * Get a 4-byte little-endian integer from memory.
 */
static unsigned int get4LE(unsigned char *addr) {
  return (((unsigned int) *(addr + 0)) <<  0) |
         (((unsigned int) *(addr + 1)) <<  8) |
         (((unsigned int) *(addr + 2)) << 16) |
         (((unsigned int) *(addr + 3)) << 24);
}


/*
 * Check a range of bytes for all zero.
 */
static Bool isZero(unsigned char *buf, int len) {
  unsigned char res;
  int i;

  res = 0;
  for (i = 0; i < len; i++) {
    res |= buf[i];
  }
  return res == 0;
}


/*
 * Compare a little-endian UUID with a big-endian prototype
 */
static Bool uuidIsEqualLE(unsigned char *dst, unsigned char *src) {
  int i;

  if (dst[0] != src[3] ||
      dst[1] != src[2] ||
      dst[2] != src[1] ||
      dst[3] != src[0] ||
      dst[4] != src[5] ||
      dst[5] != src[4] ||
      dst[6] != src[7] ||
      dst[7] != src[6]) {
    return 0;
  }
  for (i = 8; i < 16; i++) {
    if (dst[i] != src[i]) {
      return 0;
    }
  }
  return 1;
}


/*
 * This is the type UUID of an EOS32 swap partition.
 */
static unsigned char EOS32_SWAP[16] = {
  0xC1, 0xBD, 0x63, 0x61, 0x34, 0x2D, 0x48, 0x6E,
  0xAB, 0xBC, 0x35, 0x47, 0x54, 0x9A, 0x95, 0xF6,
};


/**************************************************************/


#define NUMBER_PART_ENTRIES	128
#define SIZEOF_PART_ENTRY	128
#define NUMBER_PART_BYTES	(NUMBER_PART_ENTRIES * SIZEOF_PART_ENTRY)
#define NUMBER_PART_SECTORS	(NUMBER_PART_BYTES) / SECTOR_SIZE


/*
 * The disk's partition table.
 */
static unsigned char partTable[NUMBER_PART_BYTES];


/*
 * Given a minor device number, return its start offset.
 */
static unsigned int getPartStart(int minorDev) {
  unsigned char *p;

  if (minorDev < 1 || minorDev > NUMBER_PART_ENTRIES) {
    return 0;
  }
  p = &partTable[(minorDev - 1) * SIZEOF_PART_ENTRY];
  return get4LE(p + 32);
}


/*
 * Given a minor device number, return its size.
 */
static unsigned int getPartSize(int minorDev) {
  unsigned char *p;

  if (minorDev < 1 || minorDev > NUMBER_PART_ENTRIES) {
    return 0;
  }
  p = &partTable[(minorDev - 1) * SIZEOF_PART_ENTRY];
  return get4LE(p + 40) - get4LE(p + 32) + 1;
}


/**************************************************************/


static void ideISR(int irq, InterruptContext *icp);


/*
 * Copy a block of data from src to dst.
 * Both addresses must be word-aligned.
 */
static void copyWords(unsigned int *dst, unsigned int *src, int n) {
  while (n--) {
    *dst++ = *src++;
  }
}


/*
 * Wait for the disk to get ready.
 * If it doesn't, panic.
 */
static void waitDiskReady(void) {
  int i;

  i = 0;
  while ((*DISK_CTRL & DISK_CTRL_READY) == 0) {
    if (++i == READY_RETRIES) {
      panic("IDE disk not responding");
    }
  }
}


/*
 * Read the partition table.
 * Attention: this operation is not interrupt driven
 * and therefore may take a while.
 */
static void readPartitionTable(void) {
  int s;

  /* read partition table sectors */
  for (s = 0; s < NUMBER_PART_SECTORS; s++) {
    *DISK_SCT = 2 + s;
    *DISK_CNT = 1;
    *DISK_CTRL = DISK_CTRL_STRT;
    while ((*DISK_CTRL & DISK_CTRL_DONE) == 0) ;
    copyWords(((unsigned int *) partTable) + s * WPS, DISK_BUFFER, WPS);
  }
}


/*
 * Show the partition table.
 */
static void showPartitionTable(void) {
  int i;
  unsigned char *p;
  unsigned int start;
  unsigned int end;
  unsigned int size;
  int j;
  char c;

  printf("Partitions:\n");
  printf("  # | start      | size       | description      \n");
  printf("----+------------+------------+------------------\n");
  for (i = 0; i < 128; i++) {
    p = &partTable[i * 128];
    if (isZero(p, 16)) {
      /* not used */
      continue;
    }
    start = get4LE(p + 32);
    end = get4LE(p + 40);
    size = end - start + 1;
    printf("%3d | ", i + 1);
    printf("0x%08X | 0x%08X | ", start, size);
    for (j = 0; j < 36; j++) {
      c = *(p + 56 + 2 * j);
      if (c == 0) {
        break;
      }
      if (c >= 0x20 && c < 0x7F) {
        printf("%c", c);
      } else {
        printf(".");
      }
    }
    printf("\n");
  }
}


/*
 * Initialize the disk driver.
 * Read the partition table.
 */
static void ideInitialize(void) {
  unsigned int totalSectors;

  /* determine disk size */
  waitDiskReady();
  totalSectors = *DISK_CAP;
  if (totalSectors == 0) {
    panic("IDE disk not found");
  }
  /* read partition table */
  readPartitionTable();
  if (debugIdeDisk) {
    printf("IDE disk has %d (0x%X) sectors\n",
           totalSectors, totalSectors);
    showPartitionTable();
  }
  /* disk queue is empty */
  ideTab.b_actf = NULL;
  ideTab.b_actl = NULL;
  /* no disk operation in progress, no errors */
  ideTab.b_active = 0;
  ideTab.b_errcnt = 0;
  /* set ISR and enable interrupts */
  setISR(DISK_IRQ, ideISR);
  setMask(getMask() | (1 << DISK_IRQ));
  /* the disk is now initialized */
  ideInitialized = TRUE;
}


/**************************************************************/


/*
 * Return the root partition number.
 */
int ideGetRoot(void) {
  if (!ideInitialized) {
    ideInitialize();
  }
  return bootPart;
}


/*
 * Return the swap partition number.
 */
int ideGetSwap(void) {
  int i;
  unsigned char *p;

  if (!ideInitialized) {
    ideInitialize();
  }
  for (i = 0; i < NUMBER_PART_ENTRIES; i++) {
    p = &partTable[i * SIZEOF_PART_ENTRY];
    if (isZero(p, 16)) {
      /* not used */
      continue;
    }
    if (uuidIsEqualLE(p, EOS32_SWAP)) {
      /* take the first partition with the right type */
      return i + 1;
    }
  }
  return -1;
}


/**************************************************************/


/*
 * Determine the size of the device in blocks.
 */
int ideSize(dev_t dev) {
  int minorDev;
  int numBlocks;

  if (debugIdeDisk) {
    printf("-----  ideSize dev = 0x%08X  -----\n",
           dev);
  }
  if (!ideInitialized) {
    ideInitialize();
  }
  minorDev = minor(dev);
  numBlocks = getPartSize(minorDev) / SPB;
  if (debugIdeDisk) {
    printf("       ideSize = 0x%08X\n",
           numBlocks);
  }
  return numBlocks;
}


/*
 * Open the device.
 */
void ideOpen(dev_t dev, int flag) {
  if (debugIdeDisk) {
    printf("-----  ideOpen dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!ideInitialized) {
    ideInitialize();
  }
}


/*
 * Close the device.
 */
void ideClose(dev_t dev, int flag) {
  if (debugIdeDisk) {
    printf("-----  ideClose dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!ideInitialized) {
    ideInitialize();
  }
}


/*
 * Start a disk command on the next block in the queue.
 */
static void ideStart(void) {
  struct buf *bp;
  unsigned int diskCtrl;
  int minorDev;
  unsigned int blkStart;

  bp = ideTab.b_actf;
  if (bp == NULL) {
    /* no work left */
    return;
  }
  ideTab.b_active = 1;
  diskCtrl = DISK_CTRL_IEN | DISK_CTRL_STRT;
  if (!(bp->b_flags & B_READ)) {
    /* this is a write operation - transfer block to disk buffer */
    copyWords(DISK_BUFFER, (unsigned int *) bp->b_un.b_addr, WPB);
    diskCtrl |= DISK_CTRL_WRT;
  }
  waitDiskReady();
  minorDev = minor(bp->b_dev);
  blkStart = bp->b_blkno * SPB;
  if (minorDev == 0) {
    /* whole disk, regardless of file systems */
    *DISK_SCT = blkStart;
  } else {
    /* this is a normal request within a file system */
    *DISK_SCT = getPartStart(minorDev) + blkStart;
  }
  *DISK_CNT = SPB;
  *DISK_CTRL = diskCtrl;
}


/*
 * This routine is called once for every block on which
 * an I/O operation needs to be done.
 */
void ideStrategy(struct buf *bp) {
  int minorDev;
  unsigned int blkStart;

  if (debugIdeDisk) {
    printf("-----  ideStrategy dev = 0x%08X, blk = 0x%08X, %s  -----\n",
           bp->b_dev, bp->b_blkno, bp->b_flags & B_READ ? "read" : "write");
  }
  if (!ideInitialized) {
    ideInitialize();
  }
  minorDev = minor(bp->b_dev);
  blkStart = bp->b_blkno * SPB;
  /* check parameters */
  if (minorDev != 0) {
    if (minorDev < 1 || minorDev > NUMBER_PART_ENTRIES ||
        blkStart + SPB > getPartSize(minorDev)) {
      /* illegal disk I/O request */
      bp->b_flags |= B_ERROR;
      iodone(bp);
      return;
    }
  }
  spl5();
  /* link buffer to disk queue */
  bp->av_forw = NULL;
  if (ideTab.b_actf == NULL) {
    ideTab.b_actf = bp;
  } else {
    ideTab.b_actl->av_forw = bp;
  }
  ideTab.b_actl = bp;
  /* start operation if none in progress */
  if (ideTab.b_active == 0) {
    ideStart();
  }
  spl0();
}


/*
 * This is the disk interrupt service routine.
 */
static void ideISR(int irq, InterruptContext *icp) {
  unsigned int diskCtrl;
  struct buf *bp;

  /* set process priority, re-enable interrupts */
  spl5();
  enableInt();
  /* disable disk IRQ */
  diskCtrl = *DISK_CTRL;
  *DISK_CTRL = diskCtrl & ~DISK_CTRL_IEN;
  if (ideTab.b_active == 0) {
    return;
  }
  bp = ideTab.b_actf;
  ideTab.b_active = 0;
  if (diskCtrl & DISK_CTRL_ERR) {
    /* an error occurred */
    deverror(bp->b_dev, bp->b_blkno, diskCtrl);
    if (++ideTab.b_errcnt <= 10) {
      ideStart();
      return;
    }
    bp->b_flags |= B_ERROR;
  } else {
    if (bp->b_flags & B_READ) {
      /* this was a read operation - transfer block from disk buffer */
      copyWords((unsigned int *) bp->b_un.b_addr, DISK_BUFFER, WPB);
    }
  }
  ideTab.b_errcnt = 0;
  ideTab.b_actf = bp->av_forw;
  bp->b_resid = 0;
  /* hand buffer back to buffer cache */
  iodone(bp);
  /* start next operation */
  ideStart();
}


/**************************************************************/


/*
 * This buffer head is used for "raw" disk accesses.
 */
static struct buf ideRawBuf;


/*
 * Raw disk read.
 */
void ideRead(dev_t dev) {
  if (debugIdeDisk) {
    printf("-----  ideRead dev = 0x%08X  -----\n",
           dev);
  }
  physio(ideStrategy, &ideRawBuf, dev, B_READ);
}


/*
 * Raw disk write.
 */
void ideWrite(dev_t dev) {
  if (debugIdeDisk) {
    printf("-----  ideWrite dev = 0x%08X  -----\n",
           dev);
  }
  physio(ideStrategy, &ideRawBuf, dev, B_WRITE);
}
