/*
 * coder.c -- code generator
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "coder.h"


#define INITIAL_BUFFER_SIZE	1000
#define EXPAND_BUFFER_FACTOR	2


static boolean debugPatch = FALSE;

static unsigned int *codeBuffer;
static int bufferSize;
static int codeSize;
static int dataSize;


void initCoder(void) {
  codeBuffer = allocate(INITIAL_BUFFER_SIZE * sizeof(unsigned int));
  bufferSize = INITIAL_BUFFER_SIZE;
  codeSize = 0;
  dataSize = 0;
}


static void expandBuffer(void) {
  int newSize;
  unsigned int *newBuffer;

  newSize = bufferSize * EXPAND_BUFFER_FACTOR;
  newBuffer = allocate(newSize * sizeof(unsigned int));
  memcpy(newBuffer, codeBuffer, bufferSize * sizeof(unsigned int));
  release(codeBuffer);
  codeBuffer = newBuffer;
  bufferSize = newSize;
}


void exitCoder(char *codeFileName) {
  FILE *codeFile;
  char cbuf[4];
  int version;

  codeFile = fopen(codeFileName, "w");
  if (codeFile == NULL) {
    error("cannot open code file '%s'", codeFileName);
  }
  cbuf[0] = 'N';
  cbuf[1] = 'J';
  cbuf[2] = 'B';
  cbuf[3] = 'F';
  if (fwrite(cbuf, 1, 4, codeFile) != 4) {
    error("cannot write code file '%s'", codeFileName);
  }
  version = NJBF_VERSION;
  if (fwrite(&version, sizeof(int), 1, codeFile) != 1) {
    error("cannot write code file '%s'", codeFileName);
  }
  if (fwrite(&codeSize, sizeof(int), 1, codeFile) != 1) {
    error("cannot write code file '%s'", codeFileName);
  }
  if (fwrite(&dataSize, sizeof(int), 1, codeFile) != 1) {
    error("cannot write code file '%s'", codeFileName);
  }
  if (fwrite(codeBuffer, sizeof(unsigned int),
             codeSize, codeFile) != codeSize) {
    error("cannot write code file '%s'", codeFileName);
  }
  fclose(codeFile);
}


void code0(int opcode) {
  if (codeSize == bufferSize) {
    expandBuffer();
  }
  codeBuffer[codeSize++] = opcode << 24;
}


void code1(int opcode, int immed) {
  if (codeSize == bufferSize) {
    expandBuffer();
  }
  codeBuffer[codeSize++] = (opcode << 24) | (immed & 0x00FFFFFF);
}


int getCurrAddr(void) {
  return codeSize;
}


void patchCode(int where, int value) {
  unsigned int instr;

  if (debugPatch) {
    printf("PATCH: where = %d, value = %d\n", where, value);
  }
  instr = codeBuffer[where];
  instr &= ~0x00FFFFFF;
  instr |= (value & 0x00FFFFFF);
  codeBuffer[where] = instr;
}


void data(int addr) {
  if (addr >= dataSize) {
    dataSize = addr + 1;
  }
}
