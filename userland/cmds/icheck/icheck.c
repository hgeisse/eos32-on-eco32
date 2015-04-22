#define	NI	16		/* number of inode blocks done at once */
#define	NB	10
#define	BITS	8

#include <sys/param.h>
#include <sys/blk.h>
#include <sys/off.h>
#include <sys/tim.h>
#include <sys/ino.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct	filsys	sblock;
struct	dinode	itab[INOPB*NI];
daddr_t	iaddr[NADDR];
daddr_t	blist[NB];
char	*bmap;

int	sflg;
int	mflg;
int	dflg;
int	fi;
ino_t	ino;

ino_t	nrfile;
ino_t	ndfile;
ino_t	nbfile;
ino_t	ncfile;

daddr_t	ndirect;
daddr_t	nindir;
daddr_t	niindir;
daddr_t	niiindir;
daddr_t	nfree;
daddr_t	ndup;

int	nerror;

void check(char *file);
void l4tol(long* dst, long *src, int cnt);
void pass1(struct dinode *ip);
int chk(daddr_t bno, char *s);
int duped(daddr_t bno);
daddr_t alloc(void);
void bfree(daddr_t bno);
void bread(daddr_t bno, char *buf, int cnt);
void bwrite(daddr_t bno, char *buf);
void makefree(void);

int main(int argc, char *argv[])
{
	register i;
	long n;

	blist[0] = -1;
	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {
		case 'd':
			dflg++;
			continue;


		case 'm':
			mflg++;
			continue;

		case 's':
			sflg++;
			continue;

		case 'b':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				blist[i] = n;
				argv++;
				argc--;
			}
			blist[i] = -1;
			continue;

		default:
			printf("Bad flag\n");
		}
		check(*argv);
	}
	return(nerror);
}

void check(char *file)
{
	register i, j;
	ino_t mino;
	daddr_t d;
	long n;

	fi = open(file, sflg?2:0);
	if (fi < 0) {
		printf("cannot open %s\n", file);
		nerror |= 04;
		return;
	}
	printf("%s:\n", file);
	nrfile = 0;
	ndfile = 0;
	ncfile = 0;
	nbfile = 0;

	ndirect = 0;
	nindir = 0;
	niindir = 0;
	niiindir = 0;

	ndup = 0;
	sync();
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	mino = sblock.s_isize * INOPB;
	ino = 0;
	n = (sblock.s_fsize - (sblock.s_isize+2) + BITS-1) / BITS;
	if (n != (unsigned)n) {
		printf("Check fsize and isize: %ld, %u\n",
		   sblock.s_fsize, sblock.s_isize);
	}
	bmap = malloc((unsigned)n);
	if (bmap==NULL) {
		printf("Not enough core; duplicates unchecked\n");
		dflg++;
		sflg = 0;
	}
	if(!dflg)
	for(i=0; i<(unsigned)n; i++)
		bmap[i] = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, sizeof(itab));
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			pass1(&itab[j]);
			ino++;
		}
	}
	ino = 0;
	sync();
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	if (sflg) {
		makefree();
		close(fi);
		if (bmap)
			free(bmap);
		return;
	}
	nfree = 0;
	while(n = alloc()) {
		if (chk(n, "free"))
			break;
		nfree++;
	}
	close(fi);
	if (bmap)
		free(bmap);

	i = nrfile + ndfile + ncfile + nbfile;
	printf("files %6u (r=%u, d=%u, b=%u, c=%u)\n",
		i, nrfile, ndfile, nbfile, ncfile);
	n = ndirect + nindir + niindir + niindir;
	printf("used %7ld (i=%ld, ii=%ld, iii=%ld, d=%ld)\n",
		n, nindir, niindir, niiindir, ndirect);
	printf("free %7ld\n", nfree);
	if(!dflg) {
		n = 0;
		for(d=sblock.s_isize+2; d<sblock.s_fsize; d++)
			if(!duped(d)) {
				if(mflg)
					printf("%ld missing\n", d);
				n++;
			}
		printf("missing%5ld\n", n);
	}
}

void l4tol(long* dst, long *src, int cnt) {
  while (cnt--) {
    *dst++ = *src++;
  }
}

void pass1(struct dinode *ip)
{
	daddr_t ind1[NINDIR];
	daddr_t ind2[NINDIR];
	daddr_t ind3[NINDIR];
	register i, j;
	int k, l;

	i = ip->di_mode & IFMT;
	if(i == 0) {
		sblock.s_tinode++;
		return;
	}
	if(i == IFCHR) {
		ncfile++;
		return;
	}
	if(i == IFBLK) {
		nbfile++;
		return;
	}
	if(i == IFDIR)
		ndfile++; else
	if(i == IFREG)
		nrfile++;
	else {
		printf("bad mode %u\n", ino);
		return;
	}
	l4tol(iaddr, ip->di_addr, NADDR);
	for(i=0; i<NADDR; i++) {
		if(iaddr[i] == 0)
			continue;
		if(i < NADDR-2) {
			ndirect++;
			chk(iaddr[i], "data (small)");
			continue;
		}
		nindir++;
		if (chk(iaddr[i], "1st indirect"))
				continue;
		bread(iaddr[i], (char *)ind1, BSIZE);
		for(j=0; j<NINDIR; j++) {
			if(ind1[j] == 0)
				continue;
			if(i == NADDR-2) {
				ndirect++;
				chk(ind1[j], "data (large)");
				continue;
			}
			niindir++;
			if(chk(ind1[j], "2nd indirect"))
				continue;
			bread(ind1[j], (char *)ind2, BSIZE);
			for(k=0; k<NINDIR; k++) {
				if(ind2[k] == 0)
					continue;
				if(i == NADDR-1) {
					ndirect++;
					chk(ind2[k], "data (huge)");
					continue;
				}
				niiindir++;
				if(chk(ind2[k], "3rd indirect"))
					continue;
				bread(ind2[k], (char *)ind3, BSIZE);
				for(l=0; l<NINDIR; l++)
					if(ind3[l]) {
						ndirect++;
						chk(ind3[l], "data (garg)");
					}
			}
		}
	}
}

int chk(daddr_t bno, char *s)
{
	register n;

	if (bno<sblock.s_isize+2 || bno>=sblock.s_fsize) {
		printf("%ld bad; inode=%u, class=%s\n", bno, ino, s);
		return(1);
	}
	if(duped(bno)) {
		printf("%ld dup; inode=%u, class=%s\n", bno, ino, s);
		ndup++;
	}
	for (n=0; blist[n] != -1; n++)
		if (bno == blist[n])
			printf("%ld arg; inode=%u, class=%s\n", bno, ino, s);
	return(0);
}

int duped(daddr_t bno)
{
	daddr_t d;
	register m, n;

	if(dflg)
		return(0);
	d = bno - (sblock.s_isize+2);
	m = 1 << (d%BITS);
	n = (d/BITS);
	if(bmap[n] & m)
		return(1);
	bmap[n] |= m;
	return(0);
}

daddr_t alloc(void)
{
	int i;
	daddr_t bno;
	union {
		char	data[BSIZE];
		struct	fblk fb;
	} buf;

	sblock.s_tfree--;
	if (sblock.s_nfree<=0)
		return(0);
	if (sblock.s_nfree>NICFREE) {
		printf("Bad free list, s.b. count = %d\n", sblock.s_nfree);
		return(0);
	}
	bno = sblock.s_free[--sblock.s_nfree];
	sblock.s_free[sblock.s_nfree] = (daddr_t)0;
	if(bno == 0)
		return(bno);
	if(sblock.s_nfree <= 0) {
		bread(bno, buf.data, BSIZE);
		sblock.s_nfree = buf.fb.df_nfree;
		if (sblock.s_nfree<0 || sblock.s_nfree>NICFREE) {
			printf("Bad free list, entry count of block %ld = %d\n",
				bno, sblock.s_nfree);
			sblock.s_nfree = 0;
			return(0);
		}
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.fb.df_free[i];
	}
	return(bno);
}

void bfree(daddr_t bno)
{
	union {
		char	data[BSIZE];
		struct	fblk fb;
	} buf;
	int i;

	sblock.s_tfree++;
	if(sblock.s_nfree >= NICFREE) {
		for(i=0; i<BSIZE; i++)
			buf.data[i] = 0;
		buf.fb.df_nfree = sblock.s_nfree;
		for(i=0; i<NICFREE; i++)
			buf.fb.df_free[i] = sblock.s_free[i];
		bwrite(bno, buf.data);
		sblock.s_nfree = 0;
	}
	sblock.s_free[sblock.s_nfree] = bno;
	sblock.s_nfree++;
}

void bread(daddr_t bno, char *buf, int cnt)
{
	register i;

	lseek(fi, bno*BSIZE, 0);
	if (read(fi, buf, cnt) != cnt) {
		printf("read error %ld\n", bno);
		if (sflg) {
			printf("No update\n");
			sflg = 0;
		}
		for(i=0; i<BSIZE; i++)
			buf[i] = 0;
	}
}

void bwrite(daddr_t bno, char *buf)
{

	lseek(fi, bno*BSIZE, 0);
	if (write(fi, buf, BSIZE) != BSIZE)
		printf("write error %ld\n", bno);
}

void makefree(void)
{
	daddr_t d;

	sblock.s_tfree = 0;
	sblock.s_tinode = 0;
	sblock.s_ninode = 0;
	sblock.s_nfree = 0;
	time(&sblock.s_time);
	sblock.s_flock = 0;
	sblock.s_ilock = 0;
	sblock.s_fmod = 0;
	sblock.s_ronly = 0;

	bfree((daddr_t)0);
	for (d = sblock.s_fsize - 1; d >= 2 + sblock.s_isize; d--) {
	  if (!duped(d)) {
	    bfree(d);
	  }
	}
	bwrite((daddr_t)1, (char *)&sblock);
	sync();
	return;
}
