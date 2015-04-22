/*
 * ncheck -- obtain file names from reading filesystem
 */

#define	NI	16		/* number of inode blocks done at once */
#define	NB	100
#define	HSIZE	2503
#define	NDIR	(BSIZE/sizeof(struct direct))

#include <sys/param.h>
#include <sys/blk.h>
#include <sys/off.h>
#include <sys/tim.h>
#include <sys/ino.h>
#include <sys/filsys.h>
#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>

struct	filsys	sblock;
struct	dinode	itab[INOPB*NI];
daddr_t	iaddr[NADDR];
ino_t	ilist[NB];
struct	htab
{
	ino_t	h_ino;
	ino_t	h_pino;
	char	h_name[DIRSIZ];
} htab[HSIZE];

int	aflg;
int	sflg;
int	fi;
ino_t	ino;
int	nhent;
int	nxfile;

int	nerror;

void check(char *file);
void l4tol(long* dst, long *src, int cnt);
void pass1(struct dinode *ip);
void pass2(struct dinode *ip);
void pass3(struct dinode *ip);
int dotname(struct direct *dp);
void pname(ino_t i, int lev);
struct htab *lookup(ino_t i, int ef);
void bread(daddr_t bno, char *buf, int cnt);
daddr_t bmap(int i);

int main(int argc, char *argv[])
{
	int i;
	long n;

	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {

		case 'a':
			aflg++;
			continue;

		case 'i':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				ilist[i] = n;
				nxfile = i;
				argv++;
				argc--;
			}
			continue;

		case 's':
			sflg++;
			continue;

		default:
			fprintf(stderr, "ncheck: bad flag %c\n", (*argv)[1]);
			nerror++;
		}
		check(*argv);
	}
	return(nerror);
}

void check(char *file)
{
	int i, j;
	ino_t mino;

	fi = open(file, 0);
	if(fi < 0) {
		fprintf(stderr, "ncheck: cannot open %s\n", file);
		nerror++;
		return;
	}
	nhent = 0;
	printf("%s:\n", file);
	sync();
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	mino = sblock.s_isize * INOPB;
	ino = 0;
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
	ilist[nxfile+1] = 0;
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, sizeof(itab));
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			pass2(&itab[j]);
			ino++;
		}
	}
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, sizeof(itab));
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			pass3(&itab[j]);
			ino++;
		}
	}
}

void l4tol(long* dst, long *src, int cnt) {
  while (cnt--) {
    *dst++ = *src++;
  }
}

void pass1(struct dinode *ip)
{
	if((ip->di_mode & IFMT) != IFDIR) {
		if (sflg==0 || nxfile>=NB)
			return;
		if ((ip->di_mode&IFMT)==IFBLK ||
		    (ip->di_mode&IFMT)==IFCHR ||
		    (ip->di_mode&(ISUID|ISGID)))
			ilist[nxfile++] = ino;
			return;
	}
	lookup(ino, 1);
}

void pass2(struct dinode *ip)
{
	struct direct dbuf[NDIR];
	long doff;
	struct direct *dp;
	register i, j;
	int k;
	struct htab *hp;
	daddr_t d;
	ino_t kno;

	if((ip->di_mode&IFMT) != IFDIR)
		return;
	l4tol(iaddr, ip->di_addr, NADDR);
	doff = 0;
	for(i=0;; i++) {
		if(doff >= ip->di_size)
			break;
		d = bmap(i);
		if(d == 0)
			break;
		bread(d, (char *)dbuf, sizeof(dbuf));
		for(j=0; j<NDIR; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = dbuf+j;
			kno = dp->d_ino;
			if(kno == 0)
				continue;
			hp = lookup(kno, 0);
			if(hp == 0)
				continue;
			if(dotname(dp))
				continue;
			hp->h_pino = ino;
			for(k=0; k<DIRSIZ; k++)
				hp->h_name[k] = dp->d_name[k];
		}
	}
}

void pass3(struct dinode *ip)
{
	struct direct dbuf[NDIR];
	long doff;
	struct direct *dp;
	register i, j;
	int k;
	daddr_t d;
	ino_t kno;

	if((ip->di_mode&IFMT) != IFDIR)
		return;
	l4tol(iaddr, ip->di_addr, NADDR);
	doff = 0;
	for(i=0;; i++) {
		if(doff >= ip->di_size)
			break;
		d = bmap(i);
		if(d == 0)
			break;
		bread(d, (char *)dbuf, sizeof(dbuf));
		for(j=0; j<NDIR; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = dbuf+j;
			kno = dp->d_ino;
			if(kno == 0)
				continue;
			if(aflg==0 && dotname(dp))
				continue;
			if(ilist[0] == 0)
				goto pr;
			for(k=0; ilist[k] != 0; k++)
				if(ilist[k] == kno)
					goto pr;
			continue;
		pr:
			printf("%u	", kno);
			pname(ino, 0);
			printf("/%.14s", dp->d_name);
			if (lookup(kno, 0))
				printf("/.");
			printf("\n");
		}
	}
}

int dotname(struct direct *dp)
{

	if (dp->d_name[0]=='.')
		if (dp->d_name[1]==0 ||
		    (dp->d_name[1]=='.' && dp->d_name[2]==0))
			return(1);
	return(0);
}

void pname(ino_t i, int lev)
{
	register struct htab *hp;

	if (i==ROOTINO)
		return;
	if ((hp = lookup(i, 0)) == 0) {
		printf("???");
		return;
	}
	if (lev > 10) {
		printf("...");
		return;
	}
	pname(hp->h_pino, ++lev);
	printf("/%.14s", hp->h_name);
}

struct htab *lookup(ino_t i, int ef)
{
	register struct htab *hp;

	for (hp = &htab[i%HSIZE]; hp->h_ino;) {
		if (hp->h_ino==i)
			return(hp);
		if (++hp >= &htab[HSIZE])
			hp = htab;
	}
	if (ef==0)
		return(0);
	if (++nhent >= HSIZE) {
		fprintf(stderr, "ncheck: out of core-- increase HSIZE\n");
		exit(1);
	}
	hp->h_ino = i;
	return(hp);
}

void bread(daddr_t bno, char *buf, int cnt)
{
	register i;

	lseek(fi, bno*BSIZE, 0);
	if (read(fi, buf, cnt) != cnt) {
		fprintf(stderr, "ncheck: read error %ld\n", bno);
		for(i=0; i<BSIZE; i++)
			buf[i] = 0;
	}
}

daddr_t bmap(int i)
{
	daddr_t ibuf[NINDIR];

	if(i < NADDR-2)
		return(iaddr[i]);
	i -= NADDR-2;
	if(i > NINDIR) {
		fprintf(stderr, "ncheck: %u - huge directory\n", ino);
		return((daddr_t)0);
	}
	bread(iaddr[NADDR-2], (char *)ibuf, sizeof(ibuf));
	return(ibuf[i]);
}
