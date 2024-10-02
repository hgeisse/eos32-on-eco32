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


/* #define NUM_PARTS	(SECTOR_SIZE / sizeof(PartEntry)) */
#define NUM_PARTS	17
#define DESCR_SIZE	20


/*
 * The disk's partition table.
 */
typedef struct {
  unsigned int type;
  unsigned int start;
  unsigned int size;
  char descr[DESCR_SIZE];
} PartEntry;

static PartEntry partTbl[NUM_PARTS] = {
  /*  0 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /*  1 */  { 0x58, 0x00000800, 0x0001E800 - 0x00000800, "-- not set --" },
  /*  2 */  { 0x59, 0x0001E800, 0x00037800 - 0x0001E800, "-- not set --" },
  /*  3 */  { 0x58, 0x00037800, 0x00069800 - 0x00037800, "-- not set --" },
  /*  4 */  { 0x58, 0x00069800, 0x00091800 - 0x00069800, "-- not set --" },
  /*  5 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /*  6 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /*  7 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /*  8 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /*  9 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 10 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 11 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 12 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 13 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 14 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 15 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
  /* 16 */  { 0x00, 0x00000000, 0x00000000, "-- not set --" },
};


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
#if 0
  /* read disk sector 1 */
  *DISK_SCT = 1;
  *DISK_CNT = 1;
  *DISK_CTRL = DISK_CTRL_STRT;
  while ((*DISK_CTRL & DISK_CTRL_DONE) == 0) ;
  /* copy sector from disk buffer */
  copyWords((unsigned int *) partTbl, DISK_BUFFER, WPS);
#endif
}


/*
 * Show the partition table.
 */
static void showPartitionTable(void) {
  unsigned int lastSector;
  int i, j;
  char c;

  printf("Partitions:\n");
  printf(" # b type       start      last       size       description\n");
  for (i = 0; i < NUM_PARTS; i++) {
    if (partTbl[i].type == 0) {
      /* skip unused partitions */
      continue;
    }
    if (partTbl[i].type != 0) {
      lastSector = partTbl[i].start + partTbl[i].size - 1;
    } else {
      lastSector = 0;
    }
    printf("%2d %s 0x%08lX 0x%08lX 0x%08lX 0x%08lX ",
           i,
           partTbl[i].type & 0x80000000 ? "*" : " ",
           partTbl[i].type & 0x7FFFFFFF,
           partTbl[i].start,
           lastSector,
           partTbl[i].size);
    for (j = 0; j < DESCR_SIZE; j++) {
      c = partTbl[i].descr[j];
      if (c == '\0') {
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


int ideGetRoot(unsigned startSector, unsigned numSectors) {
  if (!ideInitialized) {
    ideInitialize();
  }
  /* !!!!! the wizard knows: root is partition 1 */
  return 1;
}


#if 0
int ideGetRoot(unsigned startSector, unsigned numSectors) {
  int i;

  if (!ideInitialized) {
    ideInitialize();
  }
  for (i = 0; i < NUM_PARTS; i++) {
    if ((partTbl[i].type & 0x7FFFFFFF) == EOS32_FSYS &&
        partTbl[i].start == startSector &&
        partTbl[i].size == numSectors) {
      return i;
    }
  }
  return -1;
}
#endif


int ideGetSwap(void) {
  if (!ideInitialized) {
    ideInitialize();
  }
  /* !!!!! the wizard knows: swap is partition 2 */
  return 2;
}


#if 0
int ideGetSwap(void) {
  int i;

  if (!ideInitialized) {
    ideInitialize();
  }
  for (i = 0; i < NUM_PARTS; i++) {
    if ((partTbl[i].type & 0x7FFFFFFF) == EOS32_SWAP) {
      return i;
    }
  }
  return -1;
}
#endif


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
  if (minorDev >= NUM_PARTS) {
    return 0;
  }
  numBlocks = partTbl[minorDev].size / SPB;
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
    *DISK_SCT = partTbl[minorDev].start + blkStart;
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
  if (minorDev != 16) {
    if (minorDev < 0 || minorDev > 15 ||
        ((partTbl[minorDev].type & 0x7FFFFFFF) != EOS32_FSYS &&
         (partTbl[minorDev].type & 0x7FFFFFFF) != EOS32_SWAP) ||
        blkStart + SPB > partTbl[minorDev].size) {
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
