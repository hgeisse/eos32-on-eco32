/*
 * fblk.h -- structure of a free block
 */


#ifndef _FBLK_H_
#define _FBLK_H_


#define DF_PADSIZE	(BSIZE - sizeof(int) - NICFREE * sizeof(daddr_t))


struct fblk {
  int    	df_nfree;		/* number of valid block addresses */
  daddr_t	df_free[NICFREE];	/* addresses of free blocks */
  char		df_pad[DF_PADSIZE];	/* pad to block size */
};


#endif /* _FBLK_H_ */
