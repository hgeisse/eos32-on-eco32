/*
 * ureg.c -- memory management mechanics
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/proc.h"
#include "../include/text.h"
#include "../include/seg.h"

#include "ureg.h"
#include "start.h"
#include "klib.h"
#include "malloc.h"
#include "xtest.h"


Bool debugPagingParams = FALSE;
Bool debugPageTableMiss = FALSE;
Bool debugEstabur = FALSE;
Bool debugSureg = FALSE;
Bool debugCheckAddrs = FALSE;

static int maxMemory;				/* max memory per process */

static int pageDirectory[PAGE_TABLE_PAGES];	/* current page directory */


/**************************************************************/


/*
 * Show the current page table.
 */
static void showPageTable(void) {
  int i;
  int f;
  unsigned int *p;
  int j;
  unsigned int entry;

  printf("page table:\n");
  for (i = 0; i < PAGE_TABLE_PAGES; i++) {
    f = pageDirectory[i];
    if (f != 0) {
      p = (unsigned int *) FRAME2ADDR(f);
      for (j = 0; j < ENTRIES_PER_PAGE; j++) {
        entry = p[j];
        if (entry != 0) {
          printf("0x%X --> 0x%X (%s, %s)\n",
                 i * ENTRIES_PER_PAGE + j,
                 entry >> PAGE_SHIFT,
                 entry & TLB_WRITE ? "RW" : "RO",
                 entry & TLB_VALID ? "VALID" : "INVALID");
        }
      }
    }
  }
  printf("\n");
}


/*
 * Flush the TLB and free the current page table.
 */
static void freePageTable(void) {
  int i;
  int f;

  if (debugSureg) {
    printf("flush TLB and free page table\n");
  }
  flushTLB();
  for (i = 0; i < PAGE_TABLE_PAGES; i++) {
    f = pageDirectory[i];
    if (f != 0) {
      mfree(coremap, 1, f);
      pageDirectory[i] = 0;
    }
  }
}


/*
 * Install a single page-to-frame mapping in the current page table.
 */
static void mapPageToFrame(int page, int frame, Bool write, Bool valid) {
  int i;
  int f;
  unsigned int *p;
  int j;

  if (debugSureg) {
    printf("map page to frame: 0x%X --> 0x%X (%s, %s)\n",
           page, frame,
           write ? "RW" : "RO",
           valid ? "VALID" : "INVALID");
  }
  i = page >> 10;
  f = pageDirectory[i];
  if (f == 0) {
    f = malloc(coremap, 1);
    if (f == -1) {
      panic("no memory left for page table");
    }
    pageDirectory[i] = f;
    p = (unsigned int *) FRAME2ADDR(f);
    for (j = 0; j < ENTRIES_PER_PAGE; j++) {
      p[j] = 0;
    }
  } else {
    p = (unsigned int *) FRAME2ADDR(f);
  }
  p[page & 0x03FF] = (frame << PAGE_SHIFT) |
                     (write ? TLB_WRITE : 0) |
                     (valid ? TLB_VALID : 0);
}


/*
 * This ISR handles the case of a missing page table translation.
 */
static void missingPageTableTranslation(int irq, InterruptContext *icp) {
  static int tlbIndex = 0;
  unsigned int addr;
  int page;
  int i;
  int f;

  addr = icp->ic_reg[27];
  page = (addr - PAGE_TABLE_BASE) >> 2;
  icp->ic_tlbHi = page << PAGE_SHIFT;
  i = page >> 10;
  f = pageDirectory[i];
  if (f == 0) {
    panic("no page directory entry for page table page");
  }
  do {
    tlbIndex += 7;
    if (tlbIndex >= TLB_TOTAL) {
      tlbIndex -= TLB_TOTAL;
    }
  } while (tlbIndex < TLB_FIXED);
  if (debugPageTableMiss) {
    printf("#### missing page table translation\n");
    printf("#### failing addr = 0x%X\n", addr);
    printf("#### failing page = 0x%X\n", page);
    printf("#### page directory index = 0x%X\n", i);
    printf("#### frame = 0x%X\n", f);
    printf("#### TLB[%d]: 0x%X --> 0x%X (%s, %s)\n",
           tlbIndex, addr & PAGE_NUMBER, f, "RO", "VALID");
    printf("\n");
  }
  setTLB(tlbIndex, addr & PAGE_NUMBER, (f << PAGE_SHIFT) | TLB_VALID);
}


/*
 * Initialize paging.
 */
void initPaging(int maxMem) {
  int i;

  maxMemory = maxMem;
  for (i = 0; i < PAGE_TABLE_PAGES; i++) {
    pageDirectory[i] = 0;
  }
  setISR(EXC_TLB_MISS, missingPageTableTranslation);
  if (debugPagingParams) {
    printf("==============  Paging Parameters  ==============\n");
    printf("  PAGE_SIZE        = 0x%X = %d\n",
           PAGE_SIZE, PAGE_SIZE);
    printf("  NUM_USER_PAGES   = 0x%X = %d\n",
           NUM_USER_PAGES, NUM_USER_PAGES);
    printf("  BYTES_PER_ENTRY  = 0x%X = %d\n",
           BYTES_PER_ENTRY, BYTES_PER_ENTRY);
    printf("  ENTRIES_PER_PAGE = 0x%X = %d\n",
           ENTRIES_PER_PAGE, ENTRIES_PER_PAGE);
    printf("  PAGE_TABLE_SIZE  = 0x%X = %d\n",
           PAGE_TABLE_SIZE, PAGE_TABLE_SIZE);
    printf("  PAGE_TABLE_PAGES = 0x%X = %d\n",
           PAGE_TABLE_PAGES, PAGE_TABLE_PAGES);
    printf("  PAGE_TABLE_BASE  = 0x%X\n",
           PAGE_TABLE_BASE);
    printf("=================================================\n");
    printf("\n");
  }
}


/**************************************************************/


/*
 * The original comment was:
 * "Set up software prototype segmentation
 * registers to implement the 3 pseudo
 * text, data, stack segment sizes passed
 * as arguments."
 *
 * We have a page table instead of segmentation
 * registers. So in principle, we should compute
 * the contents of the page table here. It must
 * be stored in the u area, however, and can get
 * quite big. Additionally, it has a very regular
 * structure (because we don't have demand paging).
 * So we only store the parameters here and use
 * them to compute the page table later.
 *
 * In the PDP11 implementation there was
 * a flag 'sep' given as argument, that
 * specified if the text and data+stack
 * segments were to be separated. This
 * is never the case on our ECO32, thus
 * we do not have this flag.
 *
 * The last argument determines whether the
 * text segment is read-write or read-only.
 *
 * Return TRUE in case of error (i.e., not
 * enough memory), else return FALSE.
 */
Bool estabur(int nt, int nd, int ns, Bool xrw) {
  if (debugEstabur) {
    printf("-----  pid = %d: estabur nt = %d (%s), nd = %d, ns = %d  -----\n",
           u.u_procp->p_pid, nt, xrw ? "RW" : "RO", nd, ns);
  }
  if (nt + nd + ns > NUM_USER_PAGES) {
    /* more pages requested than architecturally possible */
    u.u_error = ENOMEM;
    return TRUE;
  }
  if (nt + nd + ns + U_PAGES > maxMemory) {
    /* more memory requested than allowed per process */
    u.u_error = ENOMEM;
    return TRUE;
  }
  u.u_eco32_nt = nt;
  u.u_eco32_nd = nd;
  u.u_eco32_ns = ns;
  u.u_eco32_xrw = xrw;
  sureg();
  return FALSE;
}


/*
 * The original comment was:
 * "Load the user hardware segmentation
 * registers from the software prototype.
 * The software registers must have been
 * setup prior by estabur."
 *
 * In our implementation, estabur only stores
 * its parameters in the u area. Therefore
 * we compute the page table here.
 */
void sureg(void) {
  int nt, nd, ns;
  Bool xrw;
  struct text *tp;
  int taddr, daddr;
  int pn;
  int fn;

  nt = u.u_eco32_nt;
  nd = u.u_eco32_nd;
  ns = u.u_eco32_ns;
  xrw = u.u_eco32_xrw;
  if (debugSureg) {
    printf("-----  pid = %d: sureg  -----\n",
           u.u_procp->p_pid);
    printf("base = 0x%X, size = 0x%X\n",
           u.u_procp->p_addr, u.u_procp->p_size);
    printf("nt = %d (%s), nd = %d, ns = %d\n",
           nt, xrw ? "RW" : "RO", nd, ns);
  }
  daddr = u.u_procp->p_addr;
  tp = u.u_procp->p_textp;
  if (tp == NULL) {
    taddr = daddr;
  } else {
    taddr = tp->x_caddr;
  }
  if (debugSureg) {
    printf("taddr = 0x%X, daddr = 0x%X\n",
           taddr, daddr);
  }
  freePageTable();
  /* text */
  pn = 0;
  fn = taddr;
  while (nt > 0) {
    mapPageToFrame(pn, fn, xrw, TRUE);
    pn++;
    fn++;
    nt--;
  }
  /* data */
  fn = daddr + U_PAGES;
  while (nd > 0) {
    mapPageToFrame(pn, fn, TRUE, TRUE);
    pn++;
    fn++;
    nd--;
  }
  /* stack */
  pn = 0x80000000 >> PAGE_SHIFT;
  fn += ns;
  while (ns > 0) {
    pn--;
    fn--;
    mapPageToFrame(pn, fn, TRUE, TRUE);
    ns--;
  }
#if USER_EXC_TST == 16
  mapPageToFrame(0x40000, 0x30600, TRUE, TRUE);
#endif
#if USER_EXC_TST == 22
  mapPageToFrame(0x40000, 0x00000, FALSE, TRUE);
#endif
#if USER_EXC_TST == 23
  mapPageToFrame(0x40000, 0x00000, FALSE, FALSE);
#endif
  if (debugSureg) {
    showPageTable();
  }
}


/**************************************************************/


/*
 * Check a range of user addresses for accessability.
 * If 'wrt' is set, the addresses must be writeable.
 * Return FALSE if ok, TRUE on error.
 */
Bool checkAddrs(caddr_t addr, int count, Bool wrt) {
  /* !!!!! */
  if (debugCheckAddrs) {
    printf("-----  checkAddrs base = 0x%X, count = 0x%X, %s  -----\n",
           addr, count, wrt ? "RW" : "RO");
  }
  if (debugCheckAddrs) {
    printf("       checkAddrs = OK\n");
  }
  return FALSE;
}
