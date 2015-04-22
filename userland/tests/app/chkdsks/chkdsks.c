/*
 * chkdsks.c -- check concurrent transfers from serial and IDE disks
 */


#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define SER_FILE	"/usr/data/100"		/* size: 1MB = 0x100 blks */
#define SER_NBLKS	60
#define SER_RANGE	0x100
#define IDE_DISK	"/dev/idedsk"
#define IDE_NBLKS	1300
#define IDE_RANGE	0x100

#define UIPB		(BSIZE / sizeof(unsigned int))


int ser_fd;
unsigned int ser_buf[UIPB];

char ide_data_set[IDE_RANGE];
int ide_fd;
unsigned int ide_buf[UIPB];


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


void test1(void) {
  int errs;
  int blkno;
  int i, j;

  printf("test1: reading serial disk blocks\n");
  errs = 0;
  for (i = 0; i < SER_NBLKS; i++) {
    blkno = rand() % SER_RANGE;
    lseek(ser_fd, blkno * BSIZE, SEEK_SET);
    if (read(ser_fd, ser_buf, BSIZE) != BSIZE) {
      error("read error on serial disk");
    }
    for (j = 0; j < UIPB; j++) {
      if (ser_buf[j] != blkno) {
        break;
      }
    }
    if (j != UIPB) {
      errs++;
    }
  }
  printf("    %d serial blocks, %d errors\n", SER_NBLKS, errs);
}


void test2(void) {
  int errs;
  int blkno;
  int i, j;

  printf("test2: reading/writing IDE disk blocks\n");
  errs = 0;
  for (i = 0; i < IDE_NBLKS; i++) {
    blkno = rand() % IDE_RANGE;
    lseek(ide_fd, blkno * BSIZE, SEEK_SET);
    if (read(ide_fd, ide_buf, BSIZE) != BSIZE) {
      error("read error on IDE disk");
    }
    if (!ide_data_set[blkno]) {
      for (j = 0; j < UIPB; j++) {
        ide_buf[j] = ~blkno;
      }
      lseek(ide_fd, blkno * BSIZE, SEEK_SET);
      if (write(ide_fd, ide_buf, BSIZE) != BSIZE) {
        error("write error on IDE disk");
      }
      ide_data_set[blkno] = 1;
    } else {
      for (j = 0; j < UIPB; j++) {
        if (ide_buf[j] != ~blkno) {
          break;
        }
      }
      if (j != UIPB) {
        errs++;
      }
    }
  }
  printf("    %d IDE blocks, %d errors\n", IDE_NBLKS, errs);
}


void test3(void) {
  int ser_errs;
  int ide_errs;
  int blkno;
  int ser_cnt;
  int ide_cnt;
  int j;

  printf("test3: combined serial and IDE transfers\n");
  ser_errs = 0;
  ser_cnt = 0;
  ide_errs = 0;
  ide_cnt = 0;
  while (1) {
    /* SER_NBLKS serial blocks and IDE_NBLKS IDE blocks, randomized */
    if (rand() % IDE_NBLKS < SER_NBLKS) {
      /* serial disk */
      blkno = rand() % SER_RANGE;
      lseek(ser_fd, blkno * BSIZE, SEEK_SET);
      if (read(ser_fd, ser_buf, BSIZE) != BSIZE) {
        error("read error on serial disk");
      }
      for (j = 0; j < UIPB; j++) {
        if (ser_buf[j] != blkno) {
          break;
        }
      }
      if (j != UIPB) {
        ser_errs++;
      }
      if (++ser_cnt == SER_NBLKS) {
        break;
      }
    } else {
      /* IDE disk */
      blkno = rand() % IDE_RANGE;
      lseek(ide_fd, blkno * BSIZE, SEEK_SET);
      if (read(ide_fd, ide_buf, BSIZE) != BSIZE) {
        error("read error on IDE disk");
      }
      if (!ide_data_set[blkno]) {
        for (j = 0; j < UIPB; j++) {
          ide_buf[j] = ~blkno;
        }
        lseek(ide_fd, blkno * BSIZE, SEEK_SET);
        if (write(ide_fd, ide_buf, BSIZE) != BSIZE) {
          error("write error on IDE disk");
        }
        ide_data_set[blkno] = 1;
      } else {
        for (j = 0; j < UIPB; j++) {
          if (ide_buf[j] != ~blkno) {
            break;
          }
        }
        if (j != UIPB) {
          ide_errs++;
        }
      }
      if (++ide_cnt == IDE_NBLKS) {
        break;
      }
    }
  }
  printf("    %d serial blocks, %d errors\n", ser_cnt, ser_errs);
  printf("    %d IDE blocks, %d errors\n", ide_cnt, ide_errs);
}


static int serial(int seed) {
  int errs;
  int blkno;
  int i, j;

  srand(seed);
  errs = 0;
  for (i = 0; i < SER_NBLKS; i++) {
    blkno = rand() % SER_RANGE;
    lseek(ser_fd, blkno * BSIZE, SEEK_SET);
    if (read(ser_fd, ser_buf, BSIZE) != BSIZE) {
      error("read error on serial disk");
    }
    for (j = 0; j < UIPB; j++) {
      if (ser_buf[j] != blkno) {
        break;
      }
    }
    if (j != UIPB) {
      errs++;
    }
  }
  return errs;
}


static int ide(int seed) {
  int errs;
  int blkno;
  int i, j;

  srand(seed);
  errs = 0;
  for (i = 0; i < IDE_NBLKS; i++) {
    blkno = rand() % IDE_RANGE;
    lseek(ide_fd, blkno * BSIZE, SEEK_SET);
    if (read(ide_fd, ide_buf, BSIZE) != BSIZE) {
      error("read error on IDE disk");
    }
    if (!ide_data_set[blkno]) {
      for (j = 0; j < UIPB; j++) {
        ide_buf[j] = ~blkno;
      }
      lseek(ide_fd, blkno * BSIZE, SEEK_SET);
      if (write(ide_fd, ide_buf, BSIZE) != BSIZE) {
        error("write error on IDE disk");
      }
      ide_data_set[blkno] = 1;
    } else {
      for (j = 0; j < UIPB; j++) {
        if (ide_buf[j] != ~blkno) {
          break;
        }
      }
      if (j != UIPB) {
        errs++;
      }
    }
  }
  return errs;
}


void test4(void) {
  int pid;
  int errs;

  printf("test4: concurrently reading serial disk blocks\n");
  pid = fork();
  if (pid < 0) {
    error("cannot fork");
  }
  if (pid == 0) {
    /* child */
    /* note: file position must not be shared with parent */
    close(ser_fd);
    ser_fd = open(SER_FILE, O_RDONLY);
    errs = serial(pid);
    printf("    proc1: %d serial blocks, %d errors\n", SER_NBLKS, errs);
    exit(0);
  } else {
    /* parent*/
    errs = serial(pid);
    wait(NULL);
    printf("    proc2: %d serial blocks, %d errors\n", SER_NBLKS, errs);
  }
}


void test5(void) {
  int pid;
  int errs;

  printf("test5: concurrently reading/writing IDE disk blocks\n");
  pid = fork();
  if (pid < 0) {
    error("cannot fork");
  }
  if (pid == 0) {
    /* child */
    /* note: file position must not be shared with parent */
    close(ide_fd);
    ide_fd = open(IDE_DISK, O_RDWR);
    errs = ide(pid);
    printf("    proc1: %d IDE blocks, %d errors\n", IDE_NBLKS, errs);
    exit(0);
  } else {
    /* parent*/
    errs = ide(pid);
    wait(NULL);
    printf("    proc2: %d IDE blocks, %d errors\n", IDE_NBLKS, errs);
  }
}


void test6(void) {
  int pid;
  int errs;

  printf("test6: concurrent serial and IDE transfers\n");
  pid = fork();
  if (pid < 0) {
    error("cannot fork");
  }
  if (pid == 0) {
    /* child */
    errs = serial(pid);
    printf("    proc1: %d serial blocks, %d errors\n", SER_NBLKS, errs);
    exit(0);
  } else {
    /* parent*/
    errs = ide(pid);
    wait(NULL);
    printf("    proc2: %d IDE blocks, %d errors\n", IDE_NBLKS, errs);
  }
}


int main(void) {
  int i;

  /* mount the "/usr" file system */
  if (mount("/dev/serdsk2", "/usr", 0) < 0) {
    error("cannot mount '/dev/serdsk2' on '/usr'");
  }
  /* open file on serial disk */
  ser_fd = open(SER_FILE, O_RDONLY);
  if (ser_fd < 0) {
    error("cannot open serial file '%s'", SER_FILE);
  }
  /* open IDE disk */
  ide_fd = open(IDE_DISK, O_RDWR);
  if (ide_fd < 0) {
    error("cannot open IDE disk '%s'", IDE_DISK);
  }
  /* no block contains valid data yet */
  for (i = 0; i < IDE_RANGE; i++) {
    ide_data_set[i] = 0;
  }
  /* reading only serial disk blocks */
  test1();
  /* reading/writing only IDE disk blocks */
  test2();
  /* combined transfers on serial and IDE disks */
  test3();
  /* concurrently reading only serial disk blocks */
  test4();
  /* concurrently reading/writing only IDE disk blocks */
  test5();
  /* concurrent transfers on serial and IDE disks */
  test6();
  /* done */
  close(ser_fd);
  close(ide_fd);
  umount("/dev/serdsk2");
  return 0;
}
