/*
 * serdsk.c -- serial line disk driver
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/buf.h"

#include "serdsk.h"
#include "klib.h"
#include "machdep.h"
#include "start.h"
#include "bio.h"


#define SYN		0x16
#define ACK		0x06

#define RESULT_OK	0


/*
 * Debugging.
 */
Bool debugSerDisk = FALSE;


/*
 * This buffer head holds the queue of pending disk tasks.
 */
struct buf serTab;


/*
 * Has the disk already been initialized?
 */
static Bool serInitialized = FALSE;


/*
 * The disk's total size.
 */
static unsigned int totalSectors;


/**************************************************************/


#define NUM_PARTS	(SECTOR_SIZE / sizeof(PartEntry))
#define DESCR_SIZE	20


/*
 * Structure of a partition table entry.
 */
typedef struct {
  unsigned int type;
  unsigned int start;
  unsigned int size;
  char descr[DESCR_SIZE];
} PartEntry;


/*
 * The disk's partition table.
 */
static PartEntry partTbl[NUM_PARTS];


/**************************************************************/


static char hex[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};


static void showValue(int x, int y, unsigned char b) {
  static int oldPos = 0;
  unsigned int *base;
  int pos;

  if (x < 0 || x >= 26) {
    return;
  }
  if (y < 0 || y >= 30) {
    return;
  }
  base = (unsigned int *) 0xF0100000;
  pos = (y << 7) + (x << 1) + x;
  *(base + pos + 0) = (0x07 << 8) | hex[(b >> 4) & 0x0F];
  *(base + pos + 1) = (0x07 << 8) | hex[b & 0x0F];
  *(base + pos + 2) = (0x07 << 8) | '#';
  *(base + oldPos + 2) = (0x07 << 8) | ' ';
  oldPos = pos;
}


/**************************************************************/


#define SER_BASE	((unsigned int *) 0xF0300010)

#define SER_RCV_CTRL	(SER_BASE + 0)
#define SER_RCV_DATA	(SER_BASE + 1)
#define SER_XMT_CTRL	(SER_BASE + 2)
#define SER_XMT_DATA	(SER_BASE + 3)

#define SER_RCV_RDY	0x01
#define SER_RCV_IEN	0x02

#define SER_XMT_RDY	0x01
#define SER_XMT_IEN	0x02

#define SER_RCV_IRQ	3
#define SER_XMT_IRQ	2


/*
 * Try to receive a single byte.
 */
static int serialRcv(unsigned char *bp) {
  if ((*SER_RCV_CTRL & SER_RCV_RDY) == 0) {
    return 0;
  }
  *bp = *SER_RCV_DATA;
  return 1;
}


/*
 * Try to send a single byte.
 */
static int serialSnd(unsigned char b) {
  if ((*SER_XMT_CTRL & SER_XMT_RDY) == 0) {
    return 0;
  }
  *SER_XMT_DATA = b;
  return 1;
}


/**************************************************************/


#define MAX_TRIES	10
#define MAX_DELAY	200000


/*
 * Try to connect to the serial disk server.
 */
static int serialConnect(void) {
  int i, j;
  unsigned char b;
  int result;

  if (debugSerDisk) {
    printf("-----  serialConnect  -----\n");
  }
  for (i = 0; i < MAX_TRIES; i++) {
    while (!serialSnd(SYN)) ;
    for (j = 0; j < MAX_DELAY; j++) {
      if (serialRcv(&b)) {
        break;
      }
    }
    if (j != MAX_DELAY) {
      if (b != ACK) {
        result = 0;
      } else {
        while (!serialSnd(ACK)) ;
        result = 1;
      }
      break;
    }
  }
  if (i == MAX_TRIES) {
    result = 0;
  }
  if (debugSerDisk) {
    printf("       serialConnect = %s\n",
           result != 0 ? "SUCCESS" : "FAILURE");
  }
  return result;
}


/*
 * Get total number of sectors of the serial disk.
 */
static unsigned int getTotalSectors(void) {
  int i;
  unsigned char b;
  unsigned int totalSectors;

  while (!serialSnd('c')) ;
  while (!serialRcv(&b)) ;
  if (b != RESULT_OK) {
    return 0;
  }
  totalSectors = 0;
  for (i = 0; i < 4; i++) {
    while (!serialRcv(&b)) ;
    totalSectors = (totalSectors << 8) | b;
  }
  return totalSectors;
}


/*
 * Read a sector from the serial disk.
 */
static void readSector(unsigned int sector, unsigned char *buffer) {
  int i;
  unsigned char b;

  while (!serialSnd('r')) ;
  for (i = 0; i < 4; i++) {
    b = (sector >> (8 * (3 - i))) & 0xFF;
    while (!serialSnd(b)) ;
  }
  while (!serialRcv(&b)) ;
  if (b != RESULT_OK) {
    panic("cannot read sector from serial disk");
  }
  for (i = 0; i < SECTOR_SIZE; i++) {
    while (!serialRcv(&buffer[i])) ;
  }
}


/*
 * Write a sector to the serial disk.
 */
static void writeSector(unsigned int sector, unsigned char *buffer) {
  int i;
  unsigned char b;

  while (!serialSnd('w')) ;
  for (i = 0; i < 4; i++) {
    b = (sector >> (8 * (3 - i))) & 0xFF;
    while (!serialSnd(b)) ;
  }
  for (i = 0; i < SECTOR_SIZE; i++) {
    while (!serialSnd(buffer[i])) ;
  }
  while (!serialRcv(&b)) ;
  if (b != RESULT_OK) {
    panic("cannot write sector to serial disk");
  }
}


/**************************************************************/


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
static void serInitialize(void) {
  /* connect to disk server */
  if (!serialConnect()) {
    panic("cannot connect to serial disk server");
  }
  /* determine disk size */
  totalSectors = getTotalSectors();
  if (totalSectors == 0) {
    panic("SER disk not found");
  }
  /* read partition table */
  readSector(1, (unsigned char *) partTbl);
  if (debugSerDisk) {
    printf("SER disk has %d (0x%X) sectors\n",
           totalSectors, totalSectors);
    showPartitionTable();
  }
  /* disk queue is empty */
  serTab.b_actf = NULL;
  serTab.b_actl = NULL;
  /* no disk operation in progress, no errors */
  serTab.b_active = 0;
  serTab.b_errcnt = 0;
  /* the disk is now initialized */
  serInitialized = TRUE;
}


/**************************************************************/


/*
 * Start a disk command on the next block in the queue.
 */
static void serStart(void) {
  struct buf *bp;
  int minorDev;
  unsigned int sector;
  unsigned char *buffer;
  int i;

next:
  bp = serTab.b_actf;
  if (bp == NULL) {
    /* no work left */
    return;
  }
  serTab.b_active = 1;
  minorDev = minor(bp->b_dev);
  if (minorDev > NUM_PARTS) {
    /* illegal disk I/O request */
    bp->b_flags |= B_ERROR;
    goto done;
  }
  if (minorDev == NUM_PARTS) {
    /* whole disk, regardless of file systems */
    sector = bp->b_blkno * SPB;
  } else {
    /* this is a normal request within a file system */
    sector = partTbl[minorDev].start + bp->b_blkno * SPB;
  }
  buffer = (unsigned char *) bp->b_un.b_addr;
  for (i = 0; i < SPB; i++) {
    if (bp->b_flags & B_READ) {
      /* this is a read operation */
      readSector(sector, buffer);
    } else {
      /* this is a write operation */
      writeSector(sector, buffer);
    }
    sector++;
    buffer += SECTOR_SIZE;
  }
  /* command has just been completely executed */
  if (serTab.b_active == 0) {
    /* should never happen */
    return;
  }
done:
  bp = serTab.b_actf;
  serTab.b_active = 0;
  serTab.b_errcnt = 0;
  serTab.b_actf = bp->av_forw;
  bp->b_resid = 0;
  /* hand buffer back to buffer cache */
  iodone(bp);
  /* start next operation */
  goto next;
}


/**************************************************************/


int serGetRoot(unsigned startSector, unsigned numSectors) {
  int i;

  if (!serInitialized) {
    serInitialize();
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


int serGetSwap(void) {
  int i;

  if (!serInitialized) {
    serInitialize();
  }
  for (i = 0; i < NUM_PARTS; i++) {
    if ((partTbl[i].type & 0x7FFFFFFF) == EOS32_SWAP) {
      return i;
    }
  }
  return -1;
}


/**************************************************************/


/*
 * Determine the size of the device in blocks.
 */
int serSize(dev_t dev) {
  int minorDev;
  int numBlocks;

  if (debugSerDisk) {
    printf("-----  serSize dev = 0x%08X  -----\n",
           dev);
  }
  if (!serInitialized) {
    serInitialize();
  }
  minorDev = minor(dev);
  if (minorDev > NUM_PARTS) {
    numBlocks = 0;
  } else
  if (minorDev == NUM_PARTS) {
    numBlocks = totalSectors / SPB;
  } else {
    numBlocks = partTbl[minorDev].size / SPB;
  }
  if (debugSerDisk) {
    printf("       serSize = 0x%08X\n",
           numBlocks);
  }
  return numBlocks;
}


/*
 * Open the device.
 */
void serOpen(dev_t dev, int flag) {
  if (debugSerDisk) {
    printf("-----  serOpen dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!serInitialized) {
    serInitialize();
  }
}


/*
 * Close the device.
 */
void serClose(dev_t dev, int flag) {
  if (debugSerDisk) {
    printf("-----  serClose dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!serInitialized) {
    serInitialize();
  }
}


/*
 * This routine is called once for every block on which
 * an I/O operation needs to be done.
 */
void serStrategy(struct buf *bp) {
  int minorDev;
  unsigned int blkStart;

  if (debugSerDisk) {
    printf("-----  serStrategy dev = 0x%08X, blk = 0x%08X, %s  -----\n",
           bp->b_dev, bp->b_blkno, bp->b_flags & B_READ ? "read" : "write");
  }
  if (!serInitialized) {
    serInitialize();
  }
  minorDev = minor(bp->b_dev);
  blkStart = bp->b_blkno * SPB;
  /* check parameters */
  if (minorDev != NUM_PARTS) {
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
  if (serTab.b_actf == NULL) {
    serTab.b_actf = bp;
  } else {
    serTab.b_actl->av_forw = bp;
  }
  serTab.b_actl = bp;
  /* start operation if none in progress */
  if (serTab.b_active == 0) {
    serStart();
  }
  spl0();
}


/**************************************************************/


/*
 * This buffer head is used for "raw" disk accesses.
 */
static struct buf serRawBuf;


/*
 * Raw disk read.
 */
void serRead(dev_t dev) {
  if (debugSerDisk) {
    printf("-----  serRead dev = 0x%08X  -----\n",
           dev);
  }
  physio(serStrategy, &serRawBuf, dev, B_READ);
}


/*
 * Raw disk write.
 */
void serWrite(dev_t dev) {
  if (debugSerDisk) {
    printf("-----  serWrite dev = 0x%08X  -----\n",
           dev);
  }
  physio(serStrategy, &serRawBuf, dev, B_WRITE);
}
