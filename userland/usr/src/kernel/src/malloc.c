/*
 * malloc.c -- mapped allocation
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/systm.h"
#include "../include/map.h"

#include "malloc.h"
#include "slp.h"
#include "klib.h"

Bool debugMalloc = FALSE;

struct map coremap[CMAPSIZ];	/* used to manage core space */
struct map swapmap[SMAPSIZ];	/* used to manage swap space */

/*
 * Return the name of a map.
 */
static char *mapName(struct map *mp) {
  if (mp == coremap) {
    return "core";
  }
  if (mp == swapmap) {
    return "swap";
  }
  return "unknown";
}

/*
 * Show the contents of a map.
 */
static void mapShow(struct map *mp) {
  printf("%s map:", mapName(mp));
  if (mp->m_size == 0) {
    printf(" empty\n");
  } else {
    printf("\n");
    while (mp->m_size != 0) {
      printf("size = 0x%X = %d, addr = 0x%X = %d\n",
             mp->m_size, mp->m_size, mp->m_addr, mp->m_addr);
      mp++;
    }
  }
  printf("\n");
}

/*
 * Allocate 'size' units from the given
 * map. Return the base of the allocated
 * space (in units of the map), or -1 if
 * there is none.
 * In a map, the addresses are increasing
 * and the list is terminated by a 0 size.
 * Algorithm is first-fit.
 */
int malloc(struct map *mp, int size) {
  struct map *bp;
  int addr;

  if (debugMalloc) {
    printf("-----  malloc (map = %s, size = 0x%X)  -----\n",
           mapName(mp), size);
  }
  if (size <= 0) {
    panic("malloc got illegal size");
  }
  for (bp = mp; bp->m_size != 0; bp++) {
    if (bp->m_size >= size) {
      /* area with at least 'size' units found */
      addr = bp->m_addr;
      bp->m_addr += size;
      bp->m_size -= size;
      if (bp->m_size == 0) {
        /* space exactly used up, delete entry */
        do {
          bp++;
          (bp-1)->m_addr = bp->m_addr;
          (bp-1)->m_size = bp->m_size;
        } while (bp->m_size != 0);
      }
      if (debugMalloc) {
        printf("       malloc = 0x%X\n", addr);
        mapShow(mp);
      }
      return addr;
    }
  }
  /* no area of at least 'size' units found */
  addr = -1;
  if (debugMalloc) {
    printf("       malloc = 0x%X\n", addr);
    mapShow(mp);
  }
  return addr;
}

/*
 * Free the previously allocated space at
 * 'addr' of 'size' units into the specified
 * map. Sort addr into map and combine on
 * one or both ends if possible.
 */
void mfree(struct map *mp, int size, int addr) {
  struct map *bp;
  int a;
  int s;

  if (debugMalloc) {
    printf("-----  mfree (map = %s, size = 0x%X, addr = 0x%X)  -----\n",
           mapName(mp), size, addr);
  }
  if (size < 0 || addr < 0) {
    panic("mfree got illegal size or address");
  }
  if (size == 0) {
    if (debugMalloc) {
      mapShow(mp);
    }
    return;
  }
  if (mp == coremap && runin) {
    /* wake swapper when freeing core */
    runin = 0;
    wakeup((caddr_t)&runin);
  }
  /* find correct place to insert area in front of */
  for (bp = mp; bp->m_size != 0 && bp->m_addr <= addr; bp++) ;
  /* check lower end */
  if (bp != mp && (bp-1)->m_addr + (bp-1)->m_size == addr) {
    /* combine on lower end */
    (bp-1)->m_size += size;
    /* check upper end */
    if (bp->m_size != 0 && bp->m_addr == addr + size) {
      /* combine on upper end too, delete entry */
      (bp-1)->m_size += bp->m_size;
      do {
        bp++;
        (bp-1)->m_addr = bp->m_addr;
        (bp-1)->m_size = bp->m_size;
      } while (bp->m_size != 0);
    }
  } else {
    /* cannot combine on lower end, check upper end */
    if (bp->m_size != 0 && bp->m_addr == addr + size) {
      /* combine on upper end */
      bp->m_addr -= size;
      bp->m_size += size;
    } else {
      /* cannot combine on either end, construct new entry */
      do {
        a = bp->m_addr;
        s = bp->m_size;
        bp->m_addr = addr;
        bp->m_size = size;
        addr = a;
        size = s;
        bp++;
      } while (size != 0);
      bp->m_size = 0;
    }
  }
  if (debugMalloc) {
    mapShow(mp);
  }
}
