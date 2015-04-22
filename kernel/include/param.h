/*
 * param.h -- tunable variables
 */

#ifndef _PARAM_H_
#define _PARAM_H_

/*
 * Define these macros to direct debugging output.
 */
#define OUTPUT_TO_LOGFILE	FALSE	/* may only be TRUE if run on sim */
#define OUTPUT_TO_CONSOLE	TRUE
#define OUTPUT_TO_TERMINAL	FALSE

#define K	1024
#define M	(K * K)

#define MEM_MAX	(128 * K)	/* maximum total memory size (frames) */
#define MEM_USE	(1 * K)		/* restricted memory usage (frames) */
//#define MEM_USE	(512)		/* restricted memory usage (frames) */
#define MAXMEM	512		/* max core per process (frames) */

#define	NBUF	29		/* size of buffer cache */
#define	NINODE	200		/* number of in core inodes */
#define	NFILE	175		/* number of in core file structures */
#define	NMOUNT	8		/* number of mountable file systems */
#define	MAXUPRC	25		/* max processes per user */
#define	STKSIZE	2000		/* initial user stack size (bytes) */
#define	STKMARG	0		/* additional stack margin (pages) */
#define	NOFILE	20		/* max open files per process */
#define	CANBSIZ	256		/* max size of typewriter line */
#define	CMAPSIZ	50		/* size of core allocation area */
#define	SMAPSIZ	50		/* size of swap allocation area */
#define	NCALL	20		/* max simultaneous time callouts */
#define	NPROC	150		/* max number of processes */
#define	NTEXT	40		/* max number of pure texts */
#define	NCLIST	100		/* max total clist size */
#define	HZ	50		/* Ticks/second of the clock */
#define	TIMEZONE (-1*60)	/* Minutes westward from Greenwich */
#define	DSTFLAG	1		/* Daylight Saving Time applies here */
#define	NCARGS	15000		/* max # characters in exec arglist */

/*
 * priorities
 * probably should not be
 * altered too much
 */

#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	40
#define	PUSER	50

/*
 * signals
 * dont change
 */

#define	NSIG	17
/*
 * No more than 16 signals (1-16) because they are
 * stored in bits in a word.
 */
#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt (rubout) */
#define	SIGQUIT	3	/* quit (FS) */
#define	SIGINS	4	/* illegal instruction */
#define	SIGTRC	5	/* trace or breakpoint */
#define	SIGIOT	6	/* iot */
#define	SIGEMT	7	/* emt */
#define	SIGFPT	8	/* floating exception */
#define	SIGKIL	9	/* kill, uncatchable termination */
#define	SIGBUS	10	/* bus error */
#define	SIGSEG	11	/* segmentation violation */
#define	SIGSYS	12	/* bad system call */
#define	SIGPIPE	13	/* end of pipe */
#define	SIGCLK	14	/* alarm clock */
#define	SIGTRM	15	/* Catchable termination */

/*
 * fundamental constants of the implementation
 * which cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	NMASK	(NINDIR - 1)	/* NINDIR-1 */
#define	NSHIFT	10		/* LOG2(NINDIR) */
#define	NULL	((void *) 0)
#define	CMASK	0		/* default mask for file creation */
#define	NODEV	((dev_t) -1)
#define	ROOTINO	((ino_t) 1)	/* inode number of root directory */
#define	SUPERB	((daddr_t) 1)	/* block number of the super block */
#define	INFSIZE	138		/* size of per-proc info for users */
#define	CBSIZE	28		/* number of chars in a clist block */
#define	CROUND	0x1F		/* clist rounding: sizeof(int *) + CBSIZE - 1*/

/*
 * Some macros for units conversion
 */

/* paging constants */
#define PAGE_SHIFT	12			/* log2 of page size */
#define PAGE_SIZE	(1U << PAGE_SHIFT)	/* page size in bytes */
#define PAGE_OFFSET	(PAGE_SIZE - 1)		/* page offset mask */
#define PAGE_NUMBER	(~PAGE_OFFSET)		/* page number mask */

/* address conversions */
#define PHYS2DIRECT(p)	((caddr_t) (0xC0000000 | (p)))
#define DIRECT2PHYS(a)	(((unsigned int) (a)) & ~0xC0000000)
#define FRAME2ADDR(f)	PHYS2DIRECT((f) << PAGE_SHIFT)
#define ADDR2FRAME(a)	(DIRECT2PHYS(a) >> PAGE_SHIFT)

/* size conversions */
#define PAGES2BYTES(n)	((n) << PAGE_SHIFT)
#define BYTES2PAGES(n)	((((unsigned) (n)) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)
#define PAGES2BLOCKS(n)	(n)

typedef struct { int r[1]; } *	physadr;
typedef char *		caddr_t;
typedef int		label_t[10];	/* regs 16-23, 29, 31 */

/* truth values */
#define FALSE		0
#define TRUE		1
typedef int		Bool;

#endif /* _PARAM_H_ */
