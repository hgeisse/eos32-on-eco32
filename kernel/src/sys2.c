/*
 * sys2.c -- system calls, part 2
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
#include "../include/file.h"
#include "../include/ino.h"
#include "../include/inode.h"

#include "sys2.h"
#include "nami.h"
#include "pipe.h"
#include "rdwri.h"
#include "fio.h"
#include "iget.h"

/*
 * read system call
 */
void read(void)
{
	rdwr(FREAD);
}

/*
 * write system call
 */
void write(void)
{
	rdwr(FWRITE);
}

/*
 * common code for read and write calls:
 * check permissions, set base, count, and offset,
 * and switch out to readi, writei, or pipe code.
 */
void rdwr(int mode)
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if((fp->f_flag&mode) == 0) {
		u.u_error = EBADF;
		return;
	}
	u.u_base = (caddr_t)uap->cbuf;
	u.u_count = uap->count;
	u.u_segflg = 0;
	if((fp->f_flag&FPIPE) != 0) {
		if(mode == FREAD)
			readp(fp);
		else
			writep(fp);
	} else {
		ip = fp->f_inode;
		u.u_offset = fp->f_offset;
		if ((ip->i_mode & IFMT) != IFCHR &&
		    (ip->i_mode & IFMT) != IFBLK) {
			plock(ip);
		}
		if (mode == FREAD) {
			readi(ip);
		} else {
			writei(ip);
		}
		if ((ip->i_mode & IFMT) != IFCHR &&
		    (ip->i_mode & IFMT) != IFBLK) {
			prele(ip);
		}
		fp->f_offset += uap->count-u.u_count;
	}
	u.u_r.r_val.r_val1 = uap->count-u.u_count;
}

/*
 * open system call
 */
void open(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	rwmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->fname;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	open1(ip, ++uap->rwmode, 0);
}

/*
 * creat system call
 */
void creat(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->fname;
	ip = namei(uchar, 1);
	if(ip == NULL) {
		if(u.u_error)
			return;
		ip = maknode(uap->fmode&07777&(~ISVTX));
		if (ip==NULL)
			return;
		open1(ip, FWRITE, 2);
	} else
		open1(ip, FWRITE, 1);
}

/*
 * common code for open and creat.
 * Check permissions, allocate an open file structure,
 * and call the device open routine if any.
 */
void open1(struct inode *ip, int mode, int trf)
{
	register struct file *fp;
	int i;

	if(trf != 2) {
		if(mode&FREAD)
			access(ip, IREAD);
		if(mode&FWRITE) {
			access(ip, IWRITE);
			if ((ip->i_mode & IFMT) == IFDIR)
				u.u_error = EISDIR;
		}
	}
	if(u.u_error)
		goto out;
	if(trf == 1)
		itrunc(ip);
	prele(ip);
	if ((fp = falloc()) == NULL)
		goto out;
	fp->f_flag = mode&(FREAD|FWRITE);
	fp->f_inode = ip;
	i = u.u_r.r_val.r_val1;
	openi(ip, mode&FWRITE);
	if(u.u_error == 0)
		return;
	u.u_ofile[i] = NULL;
	fp->f_count--;

out:
	iput(ip);
}

/*
 * close system call
 */
void close(void)
{
	register struct file *fp;
	register struct a {
		int	fdes;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	u.u_ofile[uap->fdes] = NULL;
	closef(fp);
}

/*
 * seek system call
 */
void seek(void)
{
	register struct file *fp;
	register struct a {
		int	fdes;
		off_t	off;
		int	sbase;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if(fp->f_flag & FPIPE) {
		u.u_error = ESPIPE;
		return;
	}
	if(uap->sbase == 1)
		uap->off += fp->f_offset;
	else if(uap->sbase == 2)
		uap->off += fp->f_inode->i_size;
	fp->f_offset = uap->off;
	u.u_r.r_off = uap->off;
}

/*
 * link system call
 */
void link(void)
{
	register struct inode *ip, *xp;
	register struct a {
		char	*target;
		char	*linkname;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_dirp = uap->target;
	ip = namei(uchar, 0);
	if (ip == NULL) {
		return;
	}
	if ((ip->i_mode & IFMT) == IFDIR && !suser()) {
		goto out;
	}
	/*
	 * Unlock to avoid possibly hanging the namei.
	 * Sadly, this means races. (Suppose someone
	 * deletes the file in the meantime?)
	 * Nor can it be locked again later
	 * because then there will be deadly
	 * embraces.
	 */
	prele(ip);
	u.u_dirp = (caddr_t)uap->linkname;
	xp = namei(uchar, 1);
	if(xp != NULL) {
		u.u_error = EEXIST;
		iput(xp);
		goto out;
	}
	if (u.u_error)
		goto out;
	if(u.u_pdir->i_dev != ip->i_dev) {
		iput(u.u_pdir);
		u.u_error = EXDEV;
		goto out;
	}
	wdir(ip);
	if (u.u_error==0) {
		ip->i_nlink++;
		ip->i_flag |= ICHG;
	}

out:
	iput(ip);
}

/*
 * mknod system call
 */
void mknod(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
		int	dev;
	} *uap;

	uap = (struct a *)u.u_ap;
	if(suser()) {
		u.u_dirp = uap->fname;
		ip = namei(uchar, 1);
		if(ip != NULL) {
			u.u_error = EEXIST;
			goto out;
		}
	}
	if(u.u_error)
		return;
	ip = maknode(uap->fmode);
	if (ip == NULL)
		return;
	ip->i_un.i_s2.i_rdev = (dev_t)uap->dev;

out:
	iput(ip);
}

/*
 * access system call
 */
void saccess(void)
{
	register svuid, svgid;
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	svuid = u.u_uid;
	svgid = u.u_gid;
	u.u_uid = u.u_ruid;
	u.u_gid = u.u_rgid;
	u.u_dirp = uap->fname;
	ip = namei(uchar, 0);
	if (ip != NULL) {
		if (uap->fmode&(IREAD>>6))
			access(ip, IREAD);
		if (uap->fmode&(IWRITE>>6))
			access(ip, IWRITE);
		if (uap->fmode&(IEXEC>>6))
			access(ip, IEXEC);
		iput(ip);
	}
	u.u_uid = svuid;
	u.u_gid = svgid;
}
