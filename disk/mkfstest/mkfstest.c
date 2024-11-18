/*
 * mkfstest.c -- generate files for file system test
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define K	1024


struct {
  char *name;
  int size;
  unsigned int chksum;
} files[] = {
  {    "1KiB",    1, 0 },
  {    "5KiB",    5, 0 },
  {   "25KiB",   25, 0 },
  {  "125KiB",  125, 0 },
  { "4121KiB", 4121, 0 },
  { "4221KiB", 4221, 0 },
  { "8217KiB", 8217, 0 },
  { "8317KiB", 8317, 0 },
};


#define CHECK	"check"


/**************************************************************/


#define CRC32_POLY	0x04C11DB7	/* normal form */
#define CRC32_POLY_REV	0xEDB88320	/* reverse form */
#define CRC32_INIT_XOR	0xFFFFFFFF
#define CRC32_FINAL_XOR	0xFFFFFFFF


static unsigned int crc32Table[256];


static void crc32Mktable(void) {
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


static unsigned int crc32;


static void crc32Init(void) {
  static int tableDone = 0;

  if (!tableDone) {
    crc32Mktable();
    tableDone = 1;
  }
  crc32 = CRC32_INIT_XOR;
}


static void crc32Sum(unsigned char *buffer, unsigned int size) {
  int i;

  for (i = 0; i < size; i++) {
    crc32 = (crc32 >> 8) ^ crc32Table[((crc32 ^ buffer[i]) & 0xFF)];
  }
}


static unsigned int crc32Fini(void) {
  return crc32 ^ CRC32_FINAL_XOR;
}


/**************************************************************/


void usage(char *myself) {
  printf("Usage: %s [--random]\n", myself);
}


int main(int argc, char *argv[]) {
  unsigned int seed;
  unsigned char buffer[K];
  FILE *outFile;
  int i, j, k;

  if (argc != 1 && argc != 2) {
    usage(argv[0]);
    return 1;
  }
  seed = 0x12345678;
  if (argc == 2) {
    if (strcmp(argv[1], "--random") == 0) {
      seed = (unsigned int) time(NULL);
    } else {
      usage(argv[0]);
      return 1;
    }
  }
  srand(seed);
  for (i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
    printf("generating data file '%s'\n    size   = %d bytes\n",
           files[i].name, files[i].size * K);
    crc32Init();
    outFile = fopen(files[i].name, "w");
    for (j = 0; j < files[i].size; j++) {
      for (k = 0; k < K; k++) {
        buffer[k] = rand() & 0xFF;
      }
      crc32Sum(buffer, K);
      if (fwrite(buffer, 1, K, outFile) != K) {
        printf("cannot write data file '%s'\n", files[i].name);
        exit(1);
      }
    }
    fclose(outFile);
    files[i].chksum = crc32Fini();
    printf("    chksum = 0x%08X\n", files[i].chksum);
  }
  outFile = fopen(CHECK, "w");
  for (i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
    fprintf(outFile, "%s %d 0x%08X\n",
            files[i].name, files[i].size, files[i].chksum);
  }
  fclose(outFile);
  return 0;
}
