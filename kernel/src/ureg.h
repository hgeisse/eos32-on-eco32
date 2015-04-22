/*
 * ureg.h -- memory management mechanics
 */


#ifndef _UREG_H_
#define _UREG_H_


#define NUM_USER_PAGES		(0x80000000 / PAGE_SIZE)
#define BYTES_PER_ENTRY		4
#define ENTRIES_PER_PAGE	(PAGE_SIZE / BYTES_PER_ENTRY)
#define PAGE_TABLE_SIZE		(NUM_USER_PAGES * BYTES_PER_ENTRY)
#define PAGE_TABLE_PAGES	(PAGE_TABLE_SIZE / PAGE_SIZE)
#define PAGE_TABLE_BASE		0x80000000

#define EXC_TLB_MISS	21	/* TLB miss exception */

#define TLB_FIXED	4	/* number of fixed TLB entries */
#define TLB_TOTAL	32	/* total number of TLB entries */

#define TLB_VALID	(1 << 0)
#define TLB_WRITE	(1 << 1)


extern Bool debugPagingParams;
extern Bool debugPageTableMiss;
extern Bool debugEstabur;
extern Bool debugSureg;
extern Bool debugCheckAddrs;


void initPaging(int maxMem);
Bool estabur(int nt, int nd, int ns, Bool xrw);
void sureg(void);

Bool checkAddrs(caddr_t addr, int count, Bool wrt);


#endif /* _UREG_H_ */
