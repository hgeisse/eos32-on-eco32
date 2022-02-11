/*
 * acc.c -- do process accounting
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/acct.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/proc.h"
#include "../include/seg.h"

#include "acc.h"
#include "start.h"
#include "pipe.h"
#include "rdwri.h"
#include "fio.h"
#include "iget.h"
#include "clock.h"
#include "nami.h"

/*
 * Perform process accounting functions.
 */
void sysacct(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (suser()) {
		if (uap->fname==NULL) {
			if (acctp) {
				plock(acctp);
				iput(acctp);
				acctp = NULL;
			}
			return;
		}
		if (acctp) {
			u.u_error = EBUSY;
			return;
		}
		u.u_dirp = uap->fname;
		ip = namei(uchar, 0);
		if (ip == NULL) {
			return;
		}
		if ((ip->i_mode & IFMT) != IFREG) {
			u.u_error = EACCES;
			iput(ip);
			return;
		}
		acctp = ip;
		prele(ip);
	}
}

/*
 * On exit, write a record on the accounting file.
 */
void acct(void)
{
	register i;
	register struct inode *ip;
	off_t siz;

	if ((ip=acctp)==NULL)
		return;
	plock(ip);
	for (i=0; i<sizeof(acctbuf.ac_comm); i++)
		acctbuf.ac_comm[i] = u.u_comm[i];
	acctbuf.ac_utime = compress(u.u_utime);
	acctbuf.ac_stime = compress(u.u_stime);
	acctbuf.ac_etime = compress(time - u.u_start);
	acctbuf.ac_btime = u.u_start;
	acctbuf.ac_uid = u.u_ruid;
	acctbuf.ac_gid = u.u_rgid;
	acctbuf.ac_mem = 0;
	acctbuf.ac_io = 0;
	acctbuf.ac_tty = u.u_ttyd;
	acctbuf.ac_flag = u.u_acflag;
	siz = ip->i_size;
	u.u_offset = siz;
	u.u_base = (caddr_t)&acctbuf;
	u.u_count = sizeof(acctbuf);
	u.u_segflg = 1;
	u.u_error = 0;
	writei(ip);
	if(u.u_error)
		ip->i_size = siz;
	prele(ip);
}

/*
 * Produce a pseudo-floating point representation
 * with 3 bits base-8 exponent, 13 bits fraction.
 */
int compress(time_t t)
{
	register exp = 0, round = 0;

	while (t >= 8192) {
		exp++;
		round = t&04;
		t >>= 3;
	}
	if (round) {
		t++;
		if (t >= 8192) {
			t >>= 3;
			exp++;
		}
	}
	return((exp<<13) + t);
}
