/*
 * alloc.c -- program to check huge memory allocation
 */


#include <stdio.h>
#include <stdlib.h>


#define MAX_CHUNKS	60
#define CHUNK_SIZE	10000


int main(void) {
  void *addr[MAX_CHUNKS];
  int i;

  for (i = 0; i < MAX_CHUNKS; i++) {
    addr[i] = malloc(CHUNK_SIZE);
    if (addr[i] == NULL) {
      break;
    }
    printf(".");
    fflush(stdout);
  }
  printf("\n");
  if (i == MAX_CHUNKS) {
    printf("%d chunks of %d bytes each allocated\n",
           i, CHUNK_SIZE);
  } else {
    printf("allocation failed at chunk %d of %d bytes\n",
           i, CHUNK_SIZE);
  }
  return 0;
}
