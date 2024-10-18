/*
 * gpt.c -- GUID partition table
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <uuid/uuid.h>

#include "gpt.h"


#define SECTOR_SIZE		512
#define MIN_NUMBER_SECTORS	4096
#define SECTORS_PER_MB		((1 << 20) / SECTOR_SIZE)

#define NUMBER_PART_ENTRIES	128
#define SIZEOF_PART_ENTRY	128
#define NUMBER_PART_BYTES	(NUMBER_PART_ENTRIES * SIZEOF_PART_ENTRY)
#define NUMBER_PART_SECTORS	(NUMBER_PART_BYTES) / SECTOR_SIZE


typedef enum { false, true } bool;


/**************************************************************/


static void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


static void warning(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Warning: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
}


/**************************************************************/


#define CRC32_POLY	0x04C11DB7	/* normal form */
#define CRC32_POLY_REV	0xEDB88320	/* reverse form */
#define CRC32_INIT_XOR	0xFFFFFFFF
#define CRC32_FINAL_XOR	0xFFFFFFFF


static unsigned int crc32Table[256];


static void crc32Init(void) {
  unsigned int c;
  int n, k;

  for (n = 0; n < 256; n++) {
    c = n;
    for (k = 0; k < 8; k++) {
      if ((c & 1) != 0) {
        c = (c >> 1) ^ CRC32_POLY_REV;
      } else {
        c = c >> 1;
      }
    }
    crc32Table[n] = c;
  }
}


static unsigned int crc32Sum(unsigned char *buffer, unsigned int size) {
  static bool initDone = false;
  int i;
  unsigned int crc32;

  if (!initDone) {
    crc32Init();
    initDone = true;
  }
  crc32 = CRC32_INIT_XOR;
  for (i = 0; i < size; i++) {
    crc32 = (crc32 >> 8) ^ crc32Table[((crc32 ^ buffer[i]) & 0xFF)];
  }
  return crc32 ^ CRC32_FINAL_XOR;
}


/**************************************************************/


static unsigned int get4LE(unsigned char *addr) {
  return (((unsigned int) *(addr + 0)) <<  0) |
         (((unsigned int) *(addr + 1)) <<  8) |
         (((unsigned int) *(addr + 2)) << 16) |
         (((unsigned int) *(addr + 3)) << 24);
}


static void put4LE(unsigned char *addr, unsigned int val) {
  *(addr + 0) = (val >>  0) & 0xFF;
  *(addr + 1) = (val >>  8) & 0xFF;
  *(addr + 2) = (val >> 16) & 0xFF;
  *(addr + 3) = (val >> 24) & 0xFF;
}


static bool isZero(unsigned char *buf, int len) {
  unsigned char res;
  int i;

  res = 0;
  for (i = 0; i < len; i++) {
    res |= buf[i];
  }
  return res == 0;
}


/**************************************************************/


static void uuid_copyLE(unsigned char *dst, unsigned char *src) {
  int i;

  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
  dst[4] = src[5];
  dst[5] = src[4];
  dst[6] = src[7];
  dst[7] = src[6];
  for (i = 8; i < 16; i++) {
    dst[i] = src[i];
  }
}


/**************************************************************/


static void rdSector(FILE *disk, unsigned int sectorNum, unsigned char *buf) {
  if (fseek(disk, (unsigned long) sectorNum * SECTOR_SIZE, SEEK_SET) < 0) {
    error("cannot position to sector %u (0x%X)", sectorNum, sectorNum);
  }
  if (fread(buf, 1, SECTOR_SIZE, disk) != SECTOR_SIZE) {
    error("cannot read sector %u (0x%X)", sectorNum, sectorNum);
  }
}


static void wrSector(FILE *disk, unsigned int sectorNum, unsigned char *buf) {
  if (fseek(disk, (unsigned long) sectorNum * SECTOR_SIZE, SEEK_SET) < 0) {
    error("cannot position to sector %u (0x%X)", sectorNum, sectorNum);
  }
  if (fwrite(buf, 1, SECTOR_SIZE, disk) != SECTOR_SIZE) {
    error("cannot write sector %u (0x%X)", sectorNum, sectorNum);
  }
}


/**************************************************************/


static unsigned char primaryTblHdr[SECTOR_SIZE];
static unsigned char primaryTable[NUMBER_PART_BYTES];

static unsigned char backupTblHdr[SECTOR_SIZE];
static unsigned char backupTable[NUMBER_PART_BYTES];


static void checkProtMBR(FILE *disk) {
  unsigned char protMBR[SECTOR_SIZE];
  int i;

  rdSector(disk, 0, protMBR);
  if (protMBR[450] != 0xEE) {
    error("protective MBR has wrong OS type in partition 1");
  }
  for (i = 1; i < 4; i++) {
    if (!isZero(&protMBR[446 + i * 16], 16)) {
      warning("MBR partition %d is not empty", i + 1);
    }
  }
  if (protMBR[510] != 0x55 || protMBR[511] != 0xAA) {
    error("protective MBR has wrong signature");
  }
  printf("Protective MBR verified.\n");
}


void gptRead(FILE *disk, unsigned int diskSize) {
  char signature[9];
  unsigned int oldHdrCRC;
  unsigned int newHdrCRC;
  unsigned int primaryLBAhi, primaryLBAlo;
  unsigned int backupLBAhi, backupLBAlo;
  int s;
  unsigned int oldTblCRC;
  unsigned int newTblCRC;
  unsigned int myLBAhi, myLBAlo;

  /* check protective MBR */
  checkProtMBR(disk);
  /* check primary table header */
  rdSector(disk, 1, primaryTblHdr);
  memset(signature, 0, 9);
  strncpy(signature, (char *) &primaryTblHdr[0], 8);
  if (strcmp(signature, "EFI PART") != 0) {
    error("primary table header has wrong signature");
  }
  oldHdrCRC = get4LE(&primaryTblHdr[16]);
  put4LE(&primaryTblHdr[16], 0x00000000);
  newHdrCRC = crc32Sum(primaryTblHdr, 92);
  put4LE(&primaryTblHdr[16], oldHdrCRC);
  if (oldHdrCRC != newHdrCRC) {
    error("primary table header has wrong CRC");
  }
  primaryLBAlo = get4LE(&primaryTblHdr[24]);
  primaryLBAhi = get4LE(&primaryTblHdr[28]);
  if (primaryLBAhi != 0 ||
      primaryLBAlo != 0x00000001) {
    error("primary table header's LBA is wrong");
  }
  backupLBAlo = get4LE(&primaryTblHdr[32]);
  backupLBAhi = get4LE(&primaryTblHdr[36]);
  if (backupLBAhi != 0 || backupLBAlo != diskSize - 1) {
    warning("backup table header is not located at end of disk");
  }
  for (s = 0; s < NUMBER_PART_SECTORS; s++) {
    rdSector(disk, 2 + s,
             &primaryTable[s * SECTOR_SIZE]);
  }
  oldTblCRC = get4LE(&primaryTblHdr[88]);
  newTblCRC = crc32Sum(primaryTable, NUMBER_PART_BYTES);
  if (oldTblCRC != newTblCRC) {
    error("primary ptbl CRC different from that stored in header");
  }
  printf("Valid primary GPT verified.\n");
  /* check backup table header */
  rdSector(disk, backupLBAlo, backupTblHdr);
  memset(signature, 0, 9);
  strncpy(signature, (char *) &backupTblHdr[0], 8);
  if (strcmp(signature, "EFI PART") != 0) {
    error("backup table header has wrong signature");
  }
  oldHdrCRC = get4LE(&backupTblHdr[16]);
  put4LE(&backupTblHdr[16], 0x00000000);
  newHdrCRC = crc32Sum(backupTblHdr, 92);
  put4LE(&backupTblHdr[16], oldHdrCRC);
  if (oldHdrCRC != newHdrCRC) {
    error("backup table header has wrong CRC");
  }
  myLBAlo = get4LE(&backupTblHdr[24]);
  myLBAhi = get4LE(&backupTblHdr[28]);
  if (myLBAhi != backupLBAhi ||
      myLBAlo != backupLBAlo) {
    error("backup table header's LBA is wrong");
  }
  for (s = 0; s < NUMBER_PART_SECTORS; s++) {
    rdSector(disk, backupLBAlo - NUMBER_PART_SECTORS + s,
             &backupTable[s * SECTOR_SIZE]);
  }
  oldTblCRC = get4LE(&backupTblHdr[88]);
  newTblCRC = crc32Sum(backupTable, NUMBER_PART_BYTES);
  if (oldTblCRC != newTblCRC) {
    error("backup ptbl CRC different from that stored in header");
  }
  printf("Valid backup GPT verified.\n");
}


void gptWrite(FILE *disk) {
  unsigned int crc;
  int s;
  unsigned int backupLBAlo;

  /* compute and store CRC of primary (and backup) table */
  crc = crc32Sum(primaryTable, NUMBER_PART_BYTES);
  put4LE(&primaryTblHdr[88], crc);
  put4LE(&backupTblHdr[88], crc);
  /* write primary table */
  for (s = 0; s < NUMBER_PART_SECTORS; s++) {
    wrSector(disk, 2 + s,
             &primaryTable[s * SECTOR_SIZE]);
  }
  /* compute CRC of primary header, write primary header */
  put4LE(&primaryTblHdr[16], 0);
  crc = crc32Sum(primaryTblHdr, 92);
  put4LE(&primaryTblHdr[16], crc);
  wrSector(disk, 1, primaryTblHdr);
  printf("Primary GPT written.\n");
  /* write backup table (copy of primary table) */
  backupLBAlo = get4LE(&primaryTblHdr[32]);
  for (s = 0; s < NUMBER_PART_SECTORS; s++) {
    wrSector(disk, backupLBAlo - NUMBER_PART_SECTORS + s,
             &primaryTable[s * SECTOR_SIZE]);
  }
  /* compute CRC of backup header, write backup header */
  put4LE(&backupTblHdr[16], 0);
  crc = crc32Sum(backupTblHdr, 92);
  put4LE(&backupTblHdr[16], crc);
  wrSector(disk, backupLBAlo, backupTblHdr);
  printf("Backup GPT written.\n");
}


/**************************************************************/


void gptGetEntry(int partNumber, GptEntry *entry) {
  unsigned char *p;
  unsigned char uuidBuf[16];
  int i;
  char c;

  if (partNumber < 1 || partNumber > NUMBER_PART_ENTRIES) {
    error("partition number out of range");
  }
  p = &primaryTable[(partNumber - 1) * SIZEOF_PART_ENTRY];
  uuid_copyLE(uuidBuf, p + 0);
  uuid_unparse_upper(uuidBuf, entry->type);
  uuid_copyLE(uuidBuf, p + 16);
  uuid_unparse_upper(uuidBuf, entry->uniq);
  entry->start = get4LE(p + 32);
  entry->end = get4LE(p + 40);
  entry->attr = get4LE(p + 48);
  for (i = 0; i < 36; i++) {
    c = *(p + 56 + 2 * i);
    entry->name[i] = c;
    if (c == 0) {
      break;
    }
  }
}


void gptSetEntry(int partNumber, GptEntry *entry) {
  unsigned char *p;
  unsigned char uuidBuf[16];
  int i;
  char c;

  if (partNumber < 1 || partNumber > NUMBER_PART_ENTRIES) {
    error("partition number out of range");
  }
  p = &primaryTable[(partNumber - 1) * SIZEOF_PART_ENTRY];
  memset(p, 0, SIZEOF_PART_ENTRY);
  uuid_parse(entry->type, uuidBuf);
  uuid_copyLE(p + 0, uuidBuf);
  uuid_parse(entry->uniq, uuidBuf);
  uuid_copyLE(p + 16, uuidBuf);
  put4LE(p + 32, entry->start);
  put4LE(p + 40, entry->end);
  put4LE(p + 48, entry->attr);
  for (i = 0; i < 35; i++) {
    c = entry->name[i];
    *(p + 56 + 2 * i) = c;
    if (c == 0) {
      break;
    }
  }
}
