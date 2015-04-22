/*
 * filsys.h -- structure of the super-block
 */


#ifndef _FILSYS_H_
#define _FILSYS_H_


#define SUPER_MAGIC	0x44FCB67D	/* magic number for super block */

#define	NICINOD		500		/* number of superblock inodes */
#define	NICFREE		500		/* number of superblock free blocks */

#define S_PADSIZE	(BSIZE - sizeof(unsigned int) \
			       - 3 * sizeof(daddr_t) - sizeof(ino_t) \
			       - sizeof(int) - NICINOD * sizeof(ino_t) \
			       - sizeof(int) - NICFREE * sizeof(daddr_t) \
			       - sizeof(time_t) - 4 * sizeof(char))


struct filsys {
  unsigned int	s_magic;		/* must be SUPER_MAGIC */
  daddr_t	s_fsize;		/* size of file system in blocks */
  daddr_t	s_isize;		/* size of inode list in blocks */
  daddr_t	s_tfree;		/* total free blocks - NOT USED */
  ino_t		s_tinode;		/* total free inodes - NOT USED */
  int		s_ninode;		/* number of i-nodes in s_inode */
  ino_t		s_inode[NICINOD];	/* free i-node list */
  int		s_nfree;		/* number of addresses in s_free */
  daddr_t	s_free[NICFREE];	/* free block list */
  time_t	s_time;			/* last super block update */
  char		s_flock;		/* lock for free list manipulation */
  char		s_ilock;		/* lock for i-list manipulation */
  char		s_fmod;			/* super block modified flag */
  char		s_ronly;		/* mounted read-only flag */
  char		s_pad[S_PADSIZE];	/* pad to block size */
};


#endif /* _FILSYS_H_ */
