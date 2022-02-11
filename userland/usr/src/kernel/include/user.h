/*
 * user.h -- the user structure
 */

#ifndef _USER_H_
#define _USER_H_

#define U_PAGES	1				/* size of u area in pages */
#define U_SIZE	(U_PAGES * PAGE_SIZE)		/* size of u area in bytes */
#define U_BASE	(0xC0000000 - U_SIZE)		/* begin of u area */
#define u	(* (struct user *) U_BASE)	/* accessible as a struct */

/*
 * The user structure.
 * One allocated per process.
 * Contains all per process data
 * that doesn't need to be referenced
 * while the process is swapped.
 * The user block is U_SIZE bytes
 * long; resides at virtual kernel
 * loc U_BASE; contains the system
 * stack per user; is cross referenced
 * with the proc structure for the
 * same process.
 */

#define EXEC_MAGIC	0x8F0B45C0	/* identifies executables */
#define	EXCLOSE		01

struct	user
{
	label_t	u_rsav;			/* save info when exchanging stacks */
	int	u_fper;			/* FP error register */
	int	u_fpsaved;		/* FP regs saved for this proc */
	struct fps {
		int	u_fpsr;		/* FP status register */
		double	u_fpregs[6];	/* FP registers */
	} u_fps;
	char	u_segflg;		/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	short	u_uid;			/* effective user id */
	short	u_gid;			/* effective group id */
	short	u_ruid;			/* real user id */
	short	u_rgid;			/* real group id */
	struct proc *u_procp;		/* pointer to proc structure */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	r_val1;
			int	r_val2;
		} r_val;
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;			/* base address for IO */
	unsigned int u_count;		/* bytes remaining for IO */
	off_t	u_offset;		/* offset in file for IO */
	struct inode *u_cdir;		/* pointer to inode of current directory */
	struct inode *u_rdir;		/* root directory of current process */
	char	u_dbuf[DIRSIZ];		/* current pathname component */
	caddr_t	u_dirp;			/* pathname pointer */
	struct direct u_dent;		/* current directory entry */
	struct inode *u_pdir;		/* inode of parent directory of dirp */
	int	u_uisa[16];		/* prototype of segmentation addresses */
	int	u_uisd[16];		/* prototype of segmentation descriptors */
	struct file *u_ofile[NOFILE];	/* pointers to file structures of open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	int	u_arg[5];		/* arguments to current system call */
	unsigned u_tsize;		/* text size (clicks) */
	unsigned u_dsize;		/* data size (clicks) */
	unsigned u_ssize;		/* stack size (clicks) */
	label_t	u_qsav;			/* label variable for quits and interrupts */
	label_t	u_ssav;			/* label variable for swapping */
	int	u_signal[NSIG];		/* disposition of signals */
	int	u_sigret[NSIG];		/* signal handler return adresses */
	time_t	u_utime;		/* this process user time */
	time_t	u_stime;		/* this process system time */
	time_t	u_cutime;		/* sum of childs' utimes */
	time_t	u_cstime;		/* sum of childs' stimes */
	int	*u_ar0;			/* address of users saved R0 */
	struct prof {			/* profile arguments */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} u_prof;
	char	u_intflg;		/* catch intr from sys */
	char	u_sep;			/* flag for I and D separation */
	struct tty *u_ttyp;		/* controlling tty pointer */
	dev_t	u_ttyd;			/* controlling tty dev */
	union {
	    struct {			/* header of executable file */
		unsigned int ux_magic;	/* must be EXEC_MAGIC */
		unsigned int ux_osegs;	/* offset of segment table in file */
		unsigned int ux_nsegs;	/* number of segment table entries */
		unsigned int ux_osyms;	/* offset of symbol table in file */
		unsigned int ux_nsyms;	/* number of symbol table entries */
		unsigned int ux_orels;	/* offset of relocation table in file */
		unsigned int ux_nrels;	/* number of relocation table entries */
		unsigned int ux_odata;	/* offset of segment data in file */
		unsigned int ux_sdata;	/* size of segment data in file */
		unsigned int ux_ostrs;	/* offset of string space in file */
		unsigned int ux_sstrs;	/* size of string space in file */
		unsigned int ux_entry;	/* entry point (if executable) */
	    } ux_exhdr;
	    struct {			/* segment table of executable file */
		unsigned int ux_name;	/* offset in string space */
		unsigned int ux_offs;	/* offset in segment data */
		unsigned int ux_addr;	/* virtual start address */
		unsigned int ux_size;	/* size of segment in bytes */
		unsigned int ux_attr;	/* segment attributes */
	    } ux_segtbl[3];
	} u_exdata;
	char	u_comm[DIRSIZ];
	time_t	u_start;
	char	u_acflag;
	short	u_fpflag;		/* unused now, will be later */
	short	u_cmask;		/* mask for file creation */
	/***************************/
	int u_eco32_nt;
	int u_eco32_nd;
	int u_eco32_ns;
	Bool u_eco32_xrw;
	/***************************/
	int	u_stack[1];
					/* kernel stack per user
					 * extends from u + U_SIZE
					 * backward - not to reach here
					 */
};

/* u_error codes */
#define	EPERM	1
#define	ENOENT	2
#define	ESRCH	3
#define	EINTR	4
#define	EIO	5
#define	ENXIO	6
#define	E2BIG	7
#define	ENOEXEC	8
#define	EBADF	9
#define	ECHILD	10
#define	EAGAIN	11
#define	ENOMEM	12
#define	EACCES	13
#define	EFAULT	14
#define	ENOTBLK	15
#define	EBUSY	16
#define	EEXIST	17
#define	EXDEV	18
#define	ENODEV	19
#define	ENOTDIR	20
#define	EISDIR	21
#define	EINVAL	22
#define	ENFILE	23
#define	EMFILE	24
#define	ENOTTY	25
#define	ETXTBSY	26
#define	EFBIG	27
#define	ENOSPC	28
#define	ESPIPE	29
#define	EROFS	30
#define	EMLINK	31
#define	EPIPE	32
#define	EDOM	33
#define	ERANGE	34

#endif /* _USER_H_ */
