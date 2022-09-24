/*
 * sdcdsk.c -- SD card disk driver
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/buf.h"

#include "sdcdsk.h"
#include "klib.h"
#include "machdep.h"
#include "start.h"
#include "bio.h"


/**************************************************************/


/* SDC addresses */
#define SDC_BASE	((unsigned int *) 0xF0600000)
#define SDC_CTRL	(SDC_BASE + 0)
#define SDC_DATA	(SDC_BASE + 1)
#define SDC_CRC7	(SDC_BASE + 2)
#define SDC_CRC16	(SDC_BASE + 3)

/* SDC control bits */
#define SDC_CRC16MISO	((unsigned int) 0x04)
#define SDC_FASTCLK	((unsigned int) 0x02)
#define SDC_SELECT	((unsigned int) 0x01)

/* SDC status bits */
#define SDC_WPROT	((unsigned int) 0x02)
#define SDC_READY	((unsigned int) 0x01)


#define START_BLOCK_TOKEN	0xFE


/**************************************************************/


/*
 * Debugging.
 */
Bool debugSdcDisk = FALSE;
static Bool debugSDC = FALSE;


/*
 * This buffer head holds the queue of pending disk tasks.
 */
struct buf sdcTab;


/*
 * Has the disk already been initialized?
 */
static Bool sdcInitialized = FALSE;


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


static unsigned int lastSent = 0;


static void select(void) {
  lastSent |= SDC_SELECT;
  *SDC_CTRL = lastSent;
}


static void deselect(void) {
  lastSent &= ~SDC_SELECT;
  *SDC_CTRL = lastSent;
}


static void fastClock(void) {
  lastSent |= SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


static void slowClock(void) {
  lastSent &= ~SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


static unsigned char sndRcv(unsigned char b) {
  unsigned char r;

  *SDC_DATA = b;
  while ((*SDC_CTRL & SDC_READY) == 0) ;
  r = *SDC_DATA;
  return r;
}


static void resetCRC7(void) {
  *SDC_CRC7 = 0x01;
}


static unsigned char getCRC7(void) {
  return *SDC_CRC7;
}


static void resetCRC16(Bool fromMISO) {
  if (fromMISO) {
    /* compute CRC for bits from MISO */
    lastSent |= SDC_CRC16MISO;
    *SDC_CTRL = lastSent;
  } else {
    /* compute CRC for bits from MOSI */
    lastSent &= ~SDC_CRC16MISO;
    *SDC_CTRL = lastSent;
  }
  *SDC_CRC16 = 0x0000;
}


static unsigned short getCRC16(void) {
  return *SDC_CRC16;
}


/**************************************************************/


static int sndCmd(unsigned char *cmd,
                  unsigned char *rcv, int size) {
  int i;
  unsigned char r;

  select();
  /* send command */
  resetCRC7();
  for (i = 0; i < 5; i++) {
    (void) sndRcv(cmd[i]);
  }
  (void) sndRcv(getCRC7());
  /* receive answer */
  i = 8;
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF && --i > 0);
  if (i == 0) {
    return 0;
  }
  rcv[0] = r;
  /* possibly receive more answer bytes */
  for (i = 1; i < size; i++) {
    rcv[i] = sndRcv(0xFF);
  }
  deselect();
  (void) sndRcv(0xFF);
  return size;
}


static int sndCmdRcvData(unsigned char *cmd,
                         unsigned char *rcv,
                         unsigned char *ptr, int size) {
  int i;
  unsigned char r;
  unsigned short crc16;

  select();
  /* send command */
  resetCRC7();
  for (i = 0; i < 5; i++) {
    (void) sndRcv(cmd[i]);
  }
  (void) sndRcv(getCRC7());
  /* receive answer */
  i = 8;
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF && --i > 0);
  if (i == 0) {
    return 0;
  }
  rcv[0] = r;
  /* wait for start block token */
  i = 2048;
  do {
    r = sndRcv(0xFF);
  } while (r != START_BLOCK_TOKEN && --i > 0);
  if (i == 0) {
    return 0;
  }
  /* receive data bytes */
  resetCRC16(TRUE);
  for (i = 0; i < size; i++) {
    ptr[i] = sndRcv(0xFF);
  }
  /* receive CRC */
  (void) sndRcv(0xFF);
  (void) sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  (void) sndRcv(0xFF);
  if (crc16 != 0x0000) {
    /* CRC error */
    return 0;
  }
  return size;
}


static int sndCmdSndData(unsigned char *cmd,
                         unsigned char *rcv,
                         unsigned char *ptr, int size) {
  int i;
  unsigned char r;
  unsigned short crc16;

  select();
  /* send command */
  resetCRC7();
  for (i = 0; i < 5; i++) {
    (void) sndRcv(cmd[i]);
  }
  (void) sndRcv(getCRC7());
  /* receive answer */
  i = 8;
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF && --i > 0);
  if (i == 0) {
    return 0;
  }
  rcv[0] = r;
  /* send start block token */
  (void) sndRcv(START_BLOCK_TOKEN);
  /* send data bytes */
  resetCRC16(FALSE);
  for (i = 0; i < size; i++) {
    (void) sndRcv(ptr[i]);
  }
  /* send CRC */
  crc16 = getCRC16();
  (void) sndRcv((crc16 >> 8) & 0xFF);
  (void) sndRcv((crc16 >> 0) & 0xFF);
  /* receive data respose token */
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF);
  rcv[1] = r;
  if ((r & 0x1F) != 0x05) {
    /* rejected */
    return 0;
  }
  /* wait while busy */
  do {
    r = sndRcv(0xFF);
  } while (r == 0x00);
  deselect();
  (void) sndRcv(0xFF);
  return size;
}


/**************************************************************/


static void initCard(void) {
  int i;

  slowClock();
  deselect();
  for (i = 0; i < 10; i++) {
    (void) sndRcv(0xFF);
  }
}


static void resetCard(void) {
  unsigned char cmd[5] = { 0x40, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debugSDC) {
    printf("CMD0   (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static void sendInterfaceCondition(void) {
  unsigned char cmd[5] = { 0x48, 0x00, 0x00, 0x01, 0xAA };
  unsigned char rcv[5];
  int n;

  n = sndCmd(cmd, rcv, 5);
  if (debugSDC) {
    printf("CMD8   (0x000001AA) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
             rcv[0], rcv[1], rcv[2], rcv[3], rcv[4]);
    }
  }
}


static void setCrcOptionOn(void) {
  unsigned char cmd[5] = { 0x7B, 0x00, 0x00, 0x00, 0x01 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debugSDC) {
    printf("CMD59  (0x00000001) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static void prepAppCmd(void) {
  unsigned char cmd[5] = { 0x77, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debugSDC) {
    printf("CMD55  (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static unsigned char sendHostCap(void) {
  unsigned char cmd[5] = { 0x69, 0x40, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debugSDC) {
    printf("ACMD41 (0x40000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
  return n == 0 ? 0xFF : rcv[0];
}


static void activateCard(void) {
  int tries;
  unsigned char res;

  for (tries = 0; tries < 1000; tries++) {
    prepAppCmd();
    res = sendHostCap();
    if (res == 0x00) {
      /* SD card activated */
      return;
    }
  }
  /* cannot activate SD card */
}


static void readOCR(void) {
  unsigned char cmd[5] = { 0x7A, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[5];
  int n;

  n = sndCmd(cmd, rcv, 5);
  if (debugSDC) {
    printf("CMD58  (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
             rcv[0], rcv[1], rcv[2], rcv[3], rcv[4]);
    }
  }
}


static void sendCardSpecData(unsigned char *csd) {
  unsigned char cmd[5] = { 0x49, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;
  int i;

  n = sndCmdRcvData(cmd, rcv, csd, 16);
  if (debugSDC) {
    printf("CMD9   (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
    for (i = 0; i < 8; i++) {
      printf("0x%02X  ", csd[i]);
    }
    printf("\n");
    for (i = 8; i < 16; i++) {
      printf("0x%02X  ", csd[i]);
    }
    printf("\n");
  }
}


static void readSingleSector(unsigned int sct, unsigned char *ptr) {
  unsigned char cmd[5] = { 0x51, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  cmd[1] = (sct >> 24) & 0xFF;
  cmd[2] = (sct >> 16) & 0xFF;
  cmd[3] = (sct >>  8) & 0xFF;
  cmd[4] = (sct >>  0) & 0xFF;
  n = sndCmdRcvData(cmd, rcv, ptr, 512);
  if (debugSDC) {
    printf("CMD51  (0x%02X%02X%02X%02X) : ",
           cmd[1], cmd[2], cmd[3], cmd[4]);
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static void writeSingleSector(unsigned int sct, unsigned char *ptr) {
  unsigned char cmd[5] = { 0x58, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[2];
  int n;

  cmd[1] = (sct >> 24) & 0xFF;
  cmd[2] = (sct >> 16) & 0xFF;
  cmd[3] = (sct >>  8) & 0xFF;
  cmd[4] = (sct >>  0) & 0xFF;
  n = sndCmdSndData(cmd, rcv, ptr, 512);
  if (debugSDC) {
    printf("CMD58  (0x%02X%02X%02X%02X) : ",
           cmd[1], cmd[2], cmd[3], cmd[4]);
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
      printf("       data response token = 0x%02X\n", rcv[1]);
    }
  }
}


/**************************************************************/


/*
 * Initialize SD card.
 */
static void initSDC(void) {
  initCard();
  resetCard();
  sendInterfaceCondition();
  setCrcOptionOn();
  activateCard();
  readOCR();
  fastClock();
}


/*
 * Get total number of sectors of the SD card disk.
 */
static unsigned int getTotalSectors(void) {
  unsigned char csd[16];
  unsigned int csize;
  unsigned int numSectors;

  sendCardSpecData(csd);
  if ((csd[0] & 0xC0) != 0x40) {
    /* wrong CSD structure version */
    return 0;
  }
  csize = (((unsigned int) csd[7] & 0x3F) << 16) |
          (((unsigned int) csd[8] & 0xFF) <<  8) |
          (((unsigned int) csd[9] & 0xFF) <<  0);
  numSectors = (csize + 1) << 10;
  return numSectors;
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
static void sdcInitialize(void) {
  /* initialize SD card */
  initSDC();
  /* determine disk size */
  totalSectors = getTotalSectors();
  if (totalSectors == 0) {
    panic("SDC disk not found");
  }
  /* read partition table */
  readSingleSector(1, (unsigned char *) partTbl);
  if (debugSdcDisk) {
    printf("SDC disk has %d (0x%X) sectors\n",
           totalSectors, totalSectors);
    showPartitionTable();
  }
  /* disk queue is empty */
  sdcTab.b_actf = NULL;
  sdcTab.b_actl = NULL;
  /* no disk operation in progress, no errors */
  sdcTab.b_active = 0;
  sdcTab.b_errcnt = 0;
  /* the disk is now initialized */
  sdcInitialized = TRUE;
}


/**************************************************************/


/*
 * Start a disk command on the next block in the queue.
 */
static void sdcStart(void) {
  struct buf *bp;
  int minorDev;
  unsigned int sector;
  unsigned char *buffer;
  int i;

next:
  bp = sdcTab.b_actf;
  if (bp == NULL) {
    /* no work left */
    return;
  }
  sdcTab.b_active = 1;
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
      readSingleSector(sector, buffer);
    } else {
      /* this is a write operation */
      writeSingleSector(sector, buffer);
    }
    sector++;
    buffer += SECTOR_SIZE;
  }
  /* command has just been completely executed */
  if (sdcTab.b_active == 0) {
    /* should never happen */
    return;
  }
done:
  bp = sdcTab.b_actf;
  sdcTab.b_active = 0;
  sdcTab.b_errcnt = 0;
  sdcTab.b_actf = bp->av_forw;
  bp->b_resid = 0;
  /* hand buffer back to buffer cache */
  iodone(bp);
  /* start next operation */
  goto next;
}


/**************************************************************/


int sdcGetRoot(unsigned startSector, unsigned numSectors) {
  int i;

  if (!sdcInitialized) {
    sdcInitialize();
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


int sdcGetSwap(void) {
  int i;

  if (!sdcInitialized) {
    sdcInitialize();
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
int sdcSize(dev_t dev) {
  int minorDev;
  int numBlocks;

  if (debugSdcDisk) {
    printf("-----  sdcSize dev = 0x%08X  -----\n",
           dev);
  }
  if (!sdcInitialized) {
    sdcInitialize();
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
  if (debugSdcDisk) {
    printf("       sdcSize = 0x%08X\n",
           numBlocks);
  }
  return numBlocks;
}


/*
 * Open the device.
 */
void sdcOpen(dev_t dev, int flag) {
  if (debugSdcDisk) {
    printf("-----  sdcOpen dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!sdcInitialized) {
    sdcInitialize();
  }
}


/*
 * Close the device.
 */
void sdcClose(dev_t dev, int flag) {
  if (debugSdcDisk) {
    printf("-----  sdcClose dev = 0x%08X, flag = %d  -----\n",
           dev, flag);
  }
  if (!sdcInitialized) {
    sdcInitialize();
  }
}


/*
 * This routine is called once for every block on which
 * an I/O operation needs to be done.
 */
void sdcStrategy(struct buf *bp) {
  int minorDev;
  unsigned int blkStart;

  if (debugSdcDisk) {
    printf("-----  sdcStrategy dev = 0x%08X, blk = 0x%08X, %s  -----\n",
           bp->b_dev, bp->b_blkno, bp->b_flags & B_READ ? "read" : "write");
  }
  if (!sdcInitialized) {
    sdcInitialize();
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
  if (sdcTab.b_actf == NULL) {
    sdcTab.b_actf = bp;
  } else {
    sdcTab.b_actl->av_forw = bp;
  }
  sdcTab.b_actl = bp;
  /* start operation if none in progress */
  if (sdcTab.b_active == 0) {
    sdcStart();
  }
  spl0();
}


/**************************************************************/


/*
 * This buffer head is used for "raw" disk accesses.
 */
static struct buf sdcRawBuf;


/*
 * Raw disk read.
 */
void sdcRead(dev_t dev) {
  if (debugSdcDisk) {
    printf("-----  sdcRead dev = 0x%08X  -----\n",
           dev);
  }
  physio(sdcStrategy, &sdcRawBuf, dev, B_READ);
}


/*
 * Raw disk write.
 */
void sdcWrite(dev_t dev) {
  if (debugSdcDisk) {
    printf("-----  sdcWrite dev = 0x%08X  -----\n",
           dev);
  }
  physio(sdcStrategy, &sdcRawBuf, dev, B_WRITE);
}
