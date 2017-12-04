/*
 * sys1.c -- system calls, part 1
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/map.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/proc.h"
#include "../include/buf.h"
#include "../include/reg.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/seg.h"
#include "../include/acct.h"

#include "sys1.h"
#include "start.h"
#include "acc.h"
#include "klib.h"
#include "txt.h"
#include "slp.h"
#include "sig.h"
#include "nami.h"
#include "pipe.h"
#include "rdwri.h"
#include "fio.h"
#include "iget.h"
#include "subr.h"
#include "bio.h"
#include "malloc.h"
#include "ureg.h"
#include "clock.h"

/*
 * getadr system call
 */
void getadr(void) {
  struct a {
    int od;
  } *uap;

  uap = (struct a *) u.u_ap;
  switch (uap->od) {
    case 0:
      u.u_r.r_val.r_val1 = (unsigned) &proc[0];
      u.u_r.r_val.r_val2 = sizeof(proc);
      break;
    case 1:
      u.u_r.r_val.r_val1 = (unsigned) &rootdev;
      u.u_r.r_val.r_val2 = sizeof(rootdev);
      break;
    case 2:
      u.u_r.r_val.r_val1 = (unsigned) &swapdev;
      u.u_r.r_val.r_val2 = sizeof(swapdev);
      break;
    case 3:
      u.u_r.r_val.r_val1 = (unsigned) &pipedev;
      u.u_r.r_val.r_val2 = sizeof(pipedev);
      break;
    default:
      u.u_error = EINVAL;
      break;
  }
}

/*
 * exece system call
 */
void exece(void)
{
	int nc;
	char *cp;
	struct buf *bp;
	int na, ne, bno, ucp, ap, sp, c;
	struct inode *ip;
	struct a {
		char	*fname;
		char	**argp;
		char	**envp;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->fname;
	if ((ip = namei(uchar, 0)) == NULL)
		return;
	bno = 0;
	bp = 0;
	if (access(ip, IEXEC)) {
		goto bad;
	}
	if ((ip->i_mode & IFMT) != IFREG ||
	    (ip->i_mode & (IEXEC | (IEXEC >> 3) | (IEXEC >> 6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}
	/*
	 * Collect arguments on "file" in swap space.
	 */
	na = 0;
	ne = 0;
	nc = 0;
	if ((bno = malloc(swapmap,(NCARGS+BSIZE-1)/BSIZE)) == -1)
		panic("out of swap");
	if (uap->argp) for (;;) {
		ap = 0;
		if (uap->argp) {
			ap = fuword((caddr_t)uap->argp);
			uap->argp++;
		}
		if (ap==0 && uap->envp) {
			uap->argp = NULL;
			if ((ap = fuword((caddr_t)uap->envp)) == 0)
				break;
			uap->envp++;
			ne++;
		}
		if (ap==0)
			break;
		na++;
		if(ap == -1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if ((c = fubyte((caddr_t)ap++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			if ((nc&BMASK) == 0) {
				if (bp)
					bawrite(bp);
				bp = getblk(swapdev, bno+(nc>>BSHIFT));
				cp = bp->b_un.b_addr;
			}
			nc++;
			*cp++ = c;
		} while (c>0);
	}
	if (bp)
		bawrite(bp);
	bp = 0;
	nc = (nc + NBPW-1) & ~(NBPW-1);
	getxfile(ip, nc);
	if (u.u_error)
		goto bad;

	/*
	 * copy back arglist
	 */

	ucp = 0x80000000 - nc - NBPW;
	ap = ucp - na * NBPW - 3 * NBPW;
	sp = ap;
	suword((caddr_t)ap, na-ne);
	nc = 0;
	for (;;) {
		ap += NBPW;
		if (na==ne) {
			suword((caddr_t)ap, 0);
			ap += NBPW;
		}
		if (--na < 0)
			break;
		suword((caddr_t)ap, ucp);
		do {
			if ((nc&BMASK) == 0) {
				if (bp)
					brelse(bp);
				bp = bread(swapdev, bno+(nc>>BSHIFT));
				cp = bp->b_un.b_addr;
			}
			subyte((caddr_t)ucp++, (c = *cp++));
			nc++;
		} while(c&0377);
	}
	suword((caddr_t)ap, 0);
	while (ucp & 3) {
	  subyte((caddr_t)ucp++, 0);
	}
	suword((caddr_t)ucp, 0);
	setregs(sp);
bad:
	if (bp)
		brelse(bp);
	if(bno)
		mfree(swapmap, (NCARGS+BSIZE-1)/BSIZE, bno);
	iput(ip);
}

/*
 * Read in and set up memory for executed file.
 */
void getxfile(struct inode *ip, int nargc)
{
	unsigned osegs, odata, entry;
	unsigned ts, ds, ss;
	long lsize;
	int i;

	/*
	 * read in first few bytes of
	 * file for segment sizes
	 *
	 * ux_magic = EXEC_MAGIC
	 * text is always RO (but its size may be zero)
	 * data starts on the next page boundary after text
	 */

	/* read header */
	u.u_base = (caddr_t)&u.u_exdata.ux_exhdr;
	u.u_count = sizeof(u.u_exdata.ux_exhdr);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if (u.u_error)
		return;
	if (u.u_count != 0) {
		u.u_error = ENOEXEC;
		return;
	}
	if (u.u_exdata.ux_exhdr.ux_magic != EXEC_MAGIC) {
		u.u_error = ENOEXEC;
		return;
	}
	if (u.u_exdata.ux_exhdr.ux_nsegs != 3) {
		u.u_error = ENOEXEC;
		return;
	}
	osegs = u.u_exdata.ux_exhdr.ux_osegs;
	odata = u.u_exdata.ux_exhdr.ux_odata;
	entry = u.u_exdata.ux_exhdr.ux_entry;
	if (entry != 0) {
		/* should eventually be fixed */
		panic("executable not starting at 0");
	}
	/* read segment table */
	u.u_base = (caddr_t)&u.u_exdata.ux_segtbl[0];
	u.u_count = sizeof(u.u_exdata.ux_segtbl);
	u.u_offset = osegs;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if (u.u_error)
		return;
	if (u.u_count != 0) {
		u.u_error = ENOEXEC;
		return;
	}
	if (u.u_exdata.ux_segtbl[0].ux_size != 0 &&
	    (ip->i_flag & ITEXT) == 0 &&
	    ip->i_count != 1) {
		u.u_error = ETXTBSY;
		return;
	}
	/* convert segment offsets to file offsets */
	u.u_exdata.ux_segtbl[0].ux_offs += odata;
	u.u_exdata.ux_segtbl[1].ux_offs += odata;
	u.u_exdata.ux_segtbl[2].ux_offs += odata;

	/*
	 * find text and data sizes
	 * try them out for possible
	 * overflow of max sizes
	 */
	ts = BYTES2PAGES(u.u_exdata.ux_segtbl[0].ux_size);
	lsize = (long) u.u_exdata.ux_segtbl[1].ux_size +
	        (long) u.u_exdata.ux_segtbl[2].ux_size;
	if (lsize != (unsigned) lsize) {
		u.u_error = ENOMEM;
		return;
	}
	ds = BYTES2PAGES(lsize);
	ss = BYTES2PAGES(STKSIZE + nargc);
	/* HG: sep == 0 */
	if (estabur(ts, ds, ss, RO))
		return;

	/*
	 * allocate and clear core
	 * at this point, committed
	 * to the new image
	 */

	u.u_prof.pr_scale = 0;
	xfree();
	i = U_PAGES + ds + ss;
	expand(i);
	while(--i >= U_PAGES)
		clearFrame(u.u_procp->p_addr + i);
	xalloc(ip);

	/*
	 * read in data segment
	 */

	estabur(0, ds, 0, RO);
	u.u_base = 0;
	u.u_offset = u.u_exdata.ux_segtbl[1].ux_offs;
	u.u_count = u.u_exdata.ux_segtbl[1].ux_size;
	readi(ip);
	/*
	 * set SUID/SGID protections, if no tracing
	 */
	if ((u.u_procp->p_flag & STRC) == 0) {
		if (ip->i_mode & ISUID)
			if (u.u_uid != 0) {
				u.u_uid = ip->i_uid;
				u.u_procp->p_uid = ip->i_uid;
			}
		if (ip->i_mode & ISGID)
			u.u_gid = ip->i_gid;
	} else {
		psignal(u.u_procp, SIGTRC);
	}
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;
	u.u_sep = 0;
	/* HG: sep == 0 */
	estabur(ts, ds, ss, RO);
}

/*
 * Clear registers on exec
 */
void setregs(int sp)
{
	int *rp;
	char *cp;
	int i;

	for (rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++) {
		if ((*rp & 1) == 0) {
			*rp = 0;
		}
	}
	for (cp = &regloc[0]; cp < &regloc[32]; cp++) {
		u.u_ar0[*cp] = 0;
	}
	u.u_ar0[SP] = sp;
	u.u_ar0[PC] = 0;
	for (rp = (int *)&u.u_fps; rp < (int *)&u.u_fps.u_fpregs[6];) {
		*rp++ = 0;
	}
	for (i = 0; i < NOFILE; i++) {
		if (u.u_pofile[i] & EXCLOSE) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
			u.u_pofile[i] &= ~EXCLOSE;
		}
	}
	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t) u.u_dbuf, (caddr_t) u.u_comm, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
void rexit(void)
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Save u. area for parent to look at.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
void exit(int rv)
{
	register int i;
	register struct proc *p, *q;
	register struct file *f;

	p = u.u_procp;
	p->p_flag &= ~(STRC|SULOCK);
	p->p_clktim = 0;
	for(i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	for(i=0; i<NOFILE; i++) {
		f = u.u_ofile[i];
		u.u_ofile[i] = NULL;
		closef(f);
	}
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();
	acct();
	mfree(coremap, p->p_size, p->p_addr);
	p->p_stat = SZOMB;
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
	for(q = &proc[0]; q < &proc[NPROC]; q++)
		if(q->p_ppid == p->p_pid) {
			wakeup((caddr_t)&proc[1]);
			q->p_ppid = 1;
			if (q->p_stat==SSTOP)
				setrun(q);
		}
	for(q = &proc[0]; q < &proc[NPROC]; q++)
		if(p->p_ppid == q->p_pid) {
			wakeup((caddr_t)q);
			swtch();
			/* no return */
		}
	swtch();
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
void wait(void)
{
	register f;
	register struct proc *p;

	f = 0;

loop:
	for(p = &proc[0]; p < &proc[NPROC]; p++)
	if(p->p_ppid == u.u_procp->p_pid) {
		f++;
		if(p->p_stat == SZOMB) {
			u.u_r.r_val.r_val1 = p->p_pid;
			u.u_r.r_val.r_val2 = ((struct xproc *)p)->xp_xstat;
			u.u_cutime += ((struct xproc *)p)->xp_utime;
			u.u_cstime += ((struct xproc *)p)->xp_stime;
			p->p_pid = 0;
			p->p_ppid = 0;
			p->p_pgrp = 0;
			p->p_sig = 0;
			p->p_flag = 0;
			p->p_wchan = 0;
			p->p_stat = 0;
			return;
		}
		if(p->p_stat == SSTOP) {
			if((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_r.r_val.r_val1 = p->p_pid;
				u.u_r.r_val.r_val2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if(f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * fork system call.
 */
void fork(void)
{
	struct proc *p1, *p2;
	int a;

	/*
	 * Make sure there's enough swap space for max
	 * core image, thus reducing chances of running out
	 */
	if ((a = malloc(swapmap, PAGES2BLOCKS(MAXMEM))) == -1) {
		u.u_error = ENOMEM;
		return;
	}
	mfree(swapmap, PAGES2BLOCKS(MAXMEM), a);
	a = 0;
	p2 = NULL;
	for(p1 = &proc[0]; p1 < &proc[NPROC]; p1++) {
		if (p1->p_stat==0 && p2==NULL)
			p2 = p1;
		else {
			if (p1->p_uid==u.u_uid && p1->p_stat!=0)
				a++;
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[NPROC-1] || a>MAXUPRC))) {
		u.u_error = EAGAIN;
		return;
	}
	p1 = u.u_procp;
	if(newproc()) {
		u.u_r.r_val.r_val1 = 0;
		u.u_r.r_val.r_val2 = p1->p_pid;
		u.u_start = time;
		u.u_cstime = 0;
		u.u_stime = 0;
		u.u_cutime = 0;
		u.u_utime = 0;
		u.u_acflag = AFORK;
		return;
	}
	u.u_r.r_val.r_val1 = p2->p_pid;
	u.u_r.r_val.r_val2 = 0;
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
void sbreak(void)
{
	struct a {
		char	*nsiz;
	};
	register a, n, d;
	int i;

	/*
	 * set n to new data size
	 * set d to new-old
	 * set n to new total size
	 */

	n = BYTES2PAGES((int)((struct a *)u.u_ap)->nsiz);
	if(!u.u_sep)
		n -= u.u_tsize;
	if(n < 0)
		n = 0;
	d = n - u.u_dsize;
	n += U_PAGES+u.u_ssize;
	/* HG: u.u_sep == 0 */
	if(estabur(u.u_tsize, u.u_dsize+d, u.u_ssize, RO))
		return;
	u.u_dsize += d;
	if(d > 0)
		goto bigger;
	a = u.u_procp->p_addr + n - u.u_ssize;
	i = n;
	n = u.u_ssize;
	while(n--) {
		copyFrame(a-d, a);
		a++;
	}
	expand(i);
	return;

bigger:
	expand(n);
	a = u.u_procp->p_addr + n;
	n = u.u_ssize;
	while(n--) {
		a--;
		copyFrame(a-d, a);
	}
	while(d--)
		clearFrame(--a);
}
