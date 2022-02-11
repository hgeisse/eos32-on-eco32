/*
 * malloc.h -- mapped allocation
 */

#ifndef _MALLOC_H_
#define _MALLOC_H_

extern Bool debugMalloc;

extern struct map coremap[CMAPSIZ];	/* used to manage core space */
extern struct map swapmap[SMAPSIZ];	/* used to manage swap space */

int malloc(struct map *mp, int size);
void mfree(struct map *mp, int size, int addr);

#endif /* _MALLOC_H_ */
