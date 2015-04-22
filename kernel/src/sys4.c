/*
 * sys4.c -- system calls, part 4
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
#include "../include/reg.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/proc.h"

#include "sys4.h"
#include "start.h"
#include "klib.h"
#include "txt.h"
#include "slp.h"
#include "sig.h"
#include "pipe.h"
#include "nami.h"
#include "rdwri.h"
#include "fio.h"
#include "iget.h"
#include "alloc.h"
#include "machdep.h"
#include "clock.h"

/*
 * Everything in this file is a routine implementing a system call.
 */

/*
 * return the current time (old-style entry)
 */
void gtime(void)
{
	u.u_r.r_time = time;
}

/*
 * New time entry-- return TOD with milliseconds, timezone,
 * DST flag
 */
void ftime(void)
{
	register struct a {
		struct	timeb	*tp;
	} *uap;
	struct timeb t;
	register unsigned ms;

	uap = (struct a *)u.u_ap;
	spl7();
	t.time = time;
	ms = lbolt;
	spl0();
	if (ms > HZ) {
		ms -= HZ;
		t.time++;
	}
	t.millitm = (1000*ms)/HZ;
	t.timezone = TIMEZONE;
	t.dstflag = DSTFLAG;
	if (copyOut((caddr_t) &t, (caddr_t) uap->tp, sizeof(t)))
		u.u_error = EFAULT;
}

/*
 * Set the time
 */
void stime(void)
{
	register struct a {
		time_t	time;
	} *uap;

	uap = (struct a *)u.u_ap;
	if(suser())
		time = uap->time;
}

void setuid(void)
{
	register uid;
	register struct a {
		int	uid;
	} *uap;

	uap = (struct a *)u.u_ap;
	uid = uap->uid;
	if(u.u_ruid == uid || suser()) {
		u.u_uid = uid;
		u.u_procp->p_uid = uid;
		u.u_ruid = uid;
	}
}

void getuid(void)
{

	u.u_r.r_val.r_val1 = u.u_ruid;
	u.u_r.r_val.r_val2 = u.u_uid;
}

void setgid(void)
{
	register gid;
	register struct a {
		int	gid;
	} *uap;

	uap = (struct a *)u.u_ap;
	gid = uap->gid;
	if(u.u_rgid == gid || suser()) {
		u.u_gid = gid;
		u.u_rgid = gid;
	}
}

void getgid(void)
{

	u.u_r.r_val.r_val1 = u.u_rgid;
	u.u_r.r_val.r_val2 = u.u_gid;
}

void getpid(void)
{
	u.u_r.r_val.r_val1 = u.u_procp->p_pid;
	u.u_r.r_val.r_val2 = u.u_procp->p_ppid;
}

void sync(void)
{

	update();
}

void nice(void)
{
	register n;
	register struct a {
		int	niceness;
	} *uap;

	uap = (struct a *)u.u_ap;
	n = uap->niceness;
	if(n < 0 && !suser())
		n = 0;
	n += u.u_procp->p_nice;
	if(n >= 2*NZERO)
		n = 2*NZERO -1;
	if(n < 0)
		n = 0;
	u.u_procp->p_nice = n;
}

/*
 * Unlink system call.
 * Hard to avoid races here, especially
 * in unlinking directories.
 */
void unlink(void)
{
	register struct inode *ip, *pp;
	struct a {
		char	*fname;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->fname;
	pp = namei(uchar, 2);
	if(pp == NULL)
		return;
	/*
	 * Check for unlink(".")
	 * to avoid hanging on the iget
	 */
	if (pp->i_number == u.u_dent.d_ino) {
		ip = pp;
		ip->i_count++;
	} else
		ip = iget(pp->i_dev, u.u_dent.d_ino);
	if (ip == NULL)
		goto out1;
	if ((ip->i_mode & IFMT) == IFDIR && !suser())
		goto out;
	/*
	 * Don't unlink a mounted file.
	 */
	if (ip->i_dev != pp->i_dev) {
		u.u_error = EBUSY;
		goto out;
	}
	if (ip->i_flag&ITEXT)
		xrele(ip);	/* try once to free text */
	if (ip->i_flag&ITEXT && ip->i_nlink==1) {
		u.u_error = ETXTBSY;
		goto out;
	}
	u.u_offset -= sizeof(struct direct);
	u.u_base = (caddr_t)&u.u_dent;
	u.u_count = sizeof(struct direct);
	u.u_dent.d_ino = 0;
	writei(pp);
	ip->i_nlink--;
	ip->i_flag |= ICHG;

out:
	iput(ip);
out1:
	iput(pp);
}

void chdir(void)
{
	chdirec(&u.u_cdir);
}

void chroot(void)
{
	if (suser())
		chdirec(&u.u_rdir);
}

void chdirec(struct inode **ipp)
{
	register struct inode *ip;
	struct a {
		char	*fname;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->fname;
	ip = namei(uchar, 0);
	if (ip == NULL)
		return;
	if ((ip->i_mode & IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto bad;
	}
	if(access(ip, IEXEC))
		goto bad;
	prele(ip);
	if (*ipp) {
		plock(*ipp);
		iput(*ipp);
	}
	*ipp = ip;
	return;

bad:
	iput(ip);
}

void chmod(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	if ((ip = owner(uap->fname)) == NULL)
		return;
	ip->i_mode &= ~07777;
	if (u.u_uid)
		uap->fmode &= ~ISVTX;
	ip->i_mode |= uap->fmode&07777;
	ip->i_flag |= ICHG;
	if (ip->i_flag&ITEXT && (ip->i_mode&ISVTX)==0)
		xrele(ip);
	iput(ip);
}

void chown(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	uid;
		int	gid;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (!suser() || (ip = owner(uap->fname)) == NULL)
		return;
	ip->i_uid = uap->uid;
	ip->i_gid = uap->gid;
	ip->i_flag |= ICHG;
	iput(ip);
}

void ssig(void)
{
	register a;
	struct a {
		int	signo;
		int	fun;
		int	ret;
	} *uap;

	uap = (struct a *)u.u_ap;
	a = uap->signo;
	if (a <= 0 || a >= NSIG || a == SIGKIL) {
		u.u_error = EINVAL;
		return;
	}
	u.u_r.r_val.r_val1 = u.u_signal[a];
	u.u_signal[a] = uap->fun;
	u.u_sigret[a] = uap->ret;
	u.u_procp->p_sig &= ~(1<<(a-1));
}

void kill(void)
{
	register struct proc *p, *q;
	register a;
	register struct a {
		int	pid;
		int	signo;
	} *uap;
	int f, priv;

	uap = (struct a *)u.u_ap;
	f = 0;
	a = uap->pid;
	priv = 0;
	if (a==-1 && u.u_uid==0) {
		priv++;
		a = 0;
	}
	q = u.u_procp;
	for(p = &proc[0]; p < &proc[NPROC]; p++) {
		if(p->p_stat == 0)
			continue;
		if(a != 0 && p->p_pid != a)
			continue;
		if(a==0 && ((p->p_pgrp!=q->p_pgrp&&priv==0) || p<=&proc[1]))
			continue;
		if(u.u_uid != 0 && u.u_uid != p->p_uid)
			continue;
		f++;
		psignal(p, uap->signo);
	}
	if(f == 0)
		u.u_error = ESRCH;
}

void times(void)
{
	register struct a {
		time_t	(*times)[4];
	} *uap;

	uap = (struct a *)u.u_ap;
	if (copyOut((caddr_t) &u.u_utime,
	            (caddr_t) uap->times,
	            sizeof(*uap->times)))
		u.u_error = EFAULT;
}

void profil(void)
{
	register struct a {
		short	*bufbase;
		unsigned bufsize;
		unsigned pcoffset;
		unsigned pcscale;
	} *uap;

	/* !!!!! */
	if (u.u_error == u.u_error) {
	  u.u_error = EINVAL;
	  return;
	}
	/* !!!!! */
	uap = (struct a *)u.u_ap;
	u.u_prof.pr_base = uap->bufbase;
	u.u_prof.pr_size = uap->bufsize;
	u.u_prof.pr_off = uap->pcoffset;
	u.u_prof.pr_scale = uap->pcscale;
}

/*
 * alarm clock signal
 */
void alarm(void)
{
	register struct proc *p;
	register c;
	register struct a {
		int	deltat;
	} *uap;

	uap = (struct a *)u.u_ap;
	p = u.u_procp;
	c = p->p_clktim;
	p->p_clktim = uap->deltat;
	u.u_r.r_val.r_val1 = c;
}

/*
 * indefinite wait.
 * no one should wakeup(&u)
 */
void pause(void)
{

	for(;;)
		sleep((caddr_t)&u, PSLEP);
}

/*
 * mode mask for creation of files
 */
void umask(void)
{
	register struct a {
		int	mask;
	} *uap;
	register t;

	uap = (struct a *)u.u_ap;
	t = u.u_cmask;
	u.u_cmask = uap->mask & 0777;
	u.u_r.r_val.r_val1 = t;
}

/*
 * Set IUPD and IACC times on file.
 * Can't set ICHG.
 */
void utime(void)
{
	register struct a {
		char	*fname;
		time_t	*tptr;
	} *uap;
	register struct inode *ip;
	time_t tv[2];

	uap = (struct a *)u.u_ap;
	if ((ip = owner(uap->fname)) == NULL)
		return;
	if (copyIn((caddr_t)uap->tptr, (caddr_t)tv, sizeof(tv))) {
		u.u_error = EFAULT;
		return;
	}
	ip->i_flag |= IACC|IUPD|ICHG;
	iupdat(ip, &tv[0], &tv[1]);
	iput(ip);
}
