/*
 * systm.h -- !!!!!
 */

#ifndef _SYSTM_H_
#define _SYSTM_H_

/*
 * Random set of variables
 * used by more than one
 * routine.
 */
extern  char	canonb[CANBSIZ];/* buffer for erase and kill (#@) */
extern  struct inode *rootdir;	/* pointer to inode of root directory */
extern  struct proc *runq;	/* head of linked list of running processes */
extern  int	lbolt;		/* time of day in 60th not in time */

/*
 * Nblkdev is the number of entries
 * (rows) in the block switch. It is
 * set in binit/bio.c by making
 * a pass over the switch.
 * Used in bounds checking on major
 * device numbers.
 */
extern  int	nblkdev;

/*
 * Number of character switch entries.
 * Set by cinit/tty.c
 */
extern  int	nchrdev;

extern  int	mpid;		/* generic for unique process id's */
extern  char	runin;		/* scheduling flag */
extern  char	runout;		/* scheduling flag */
extern  char	runrun;		/* scheduling flag */
extern  char	curpri;		/* more scheduling */
extern  int	updlock;	/* lock for sync */
extern  daddr_t	rablock;	/* block to be read ahead */
extern	char	regloc[];	/* locs. of saved user registers (trap.c) */
extern  dev_t	rootdev;	/* device of the root */
extern  dev_t	swapdev;	/* swapping device */
extern  dev_t	pipedev;	/* pipe device */

/*
 * Structure of the system-entry table
 */
extern struct sysent {
  char	sy_narg;		/* total number of arguments */
  void	(*sy_call)(void);	/* handler */
} sysent[64];

#endif /* _SYSTM_H_ */
