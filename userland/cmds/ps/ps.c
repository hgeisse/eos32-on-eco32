/*
 *	ps - process status
 *	examine and print certain things about processes
 */

#include <sys/param.h>
#include <sys/dev.h>
#include <sys/off.h>
#include <sys/blk.h>
#include <sys/tim.h>
#include <sys/proc.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <eos32.h>
#include <core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nlist {
  char *name;
  int objdes;
  unsigned int addr;
  unsigned int size;
};

struct nlist nl[] = {
	{ "proc" ,   0, 0, 0 },
	{ "swapdev", 1, 0, 0 },
};

struct	proc mproc;

struct	user usr;
int	chkpid;
int	retcode=1;
int	lflg;
int	vflg;
int	kflg;
int	xflg;
char	*tptr;
int	aflg;
int	mem;
int	swmem;
int	swap;

int	ndev;
struct devl {
	char	dname[DIRSIZ];
	dev_t	dev;
} devl[256];

char	*coref;

void getInfo(struct nlist nlp[], int n);
void getdev(void);
unsigned long round(unsigned long a, unsigned long b);
int prcom(int puid);
char *gettty(void);
char *getptr(char **adr);
int getbyte(char *adr);
int within(char *adr, unsigned long lbd, unsigned long ubd);

int main(int argc, char **argv)
{
	int i;
	char *ap;
	int uid, puid;

	if (argc>1) {
		ap = argv[1];
		while (*ap) switch (*ap++) {

		case 'v':
			vflg++;
			break;

		case 'a':
			aflg++;
			break;

		case 't':
			if(*ap)
				tptr = ap;
			aflg++;
			if (*tptr == '?')
				xflg++;
			goto bbreak;

		case 'x':
			xflg++;
			break;

		case '-':
			break;

		case 'l':
			lflg++;
			break;

		case 'k':
			kflg++;
			break;

		default:
			chkpid = atoi(ap-1);
			goto bbreak;
			break;
		}
	}

bbreak:
	if(chdir("/dev") < 0) {
		fprintf(stderr, "Can't change to /dev\n");
		exit(1);
	}
	getInfo(nl, sizeof(nl)/sizeof(nl[0]));
	coref = "/dev/mem";
	if(kflg)
		coref = "/usr/sys/core";
	if ((mem = open(coref, 0)) < 0) {
		fprintf(stderr, "No mem\n");
		exit(1);
	}
	swmem = open(coref, 0);
	/*
	 * read mem to find swap dev.
	 */
	lseek(mem, (long)nl[1].addr, 0);
	read(mem, (char *)&nl[1].addr, sizeof(nl[1].addr));
	/*
	 * Locate proc table
	 */
	lseek(mem, (long)nl[0].addr, 0);
	getdev();
	uid = getuid();
	if (lflg)
	printf(" F S UID   PID  PPID CPU PRI NICE  ADDR  SZ  WCHAN TTY TIME CMD\n"); else
		if (chkpid==0) printf("   PID TTY TIME CMD\n");
	for (i=0; i<NPROC; i++) {
		read(mem, (char *)&mproc, sizeof mproc);
		if (mproc.p_stat==0)
			continue;
		if (mproc.p_pgrp==0 && xflg==0 && mproc.p_uid==0)
			continue;
		puid = mproc.p_uid;
		if ((uid != puid && aflg==0) ||
		    (chkpid!=0 && chkpid!=mproc.p_pid))
			continue;
		if(prcom(puid)) {
			printf("\n");
			retcode=0;
		}
	}
	exit(retcode);
	return 0;
}

void getInfo(struct nlist nlp[], int n) {
  int i;

  for (i = 0; i < n; i++) {
    if (getadr(nlp[i].objdes, &nlp[i].addr, &nlp[i].size) < 0) {
      fprintf(stderr, "No info on %s\n", nlp[i].name);
      exit(1);
    }
    nlp[i].addr = DIRECT2PHYS(nlp[i].addr);
  }
}

#include <sys/stat.h>

void getdev(void)
{
	register FILE *df;
	struct stat sbuf;
	struct direct dbuf;

	if ((df = fopen("/dev", "r")) == NULL) {
		fprintf(stderr, "Can't open /dev\n");
		exit(1);
	}
	ndev = 0;
	while (fread((char *)&dbuf, sizeof(dbuf), 1, df) == 1) {
		if(dbuf.d_ino == 0)
			continue;
		if(stat(dbuf.d_name, &sbuf) < 0)
			continue;
		if ((sbuf.st_mode&S_IFMT) != S_IFCHR)
			continue;
		strcpy(devl[ndev].dname, dbuf.d_name);
		devl[ndev].dev = sbuf.st_rdev;
		ndev++;
	}
	fclose(df);
	if ((swap = open("/dev/swap", 0)) < 0) {
		fprintf(stderr, "Can't open /dev/swap\n");
		exit(1);
	}
}

unsigned long round(unsigned long a, unsigned long b)
{
	unsigned long w = ((a+b-1)/b)*b;

	return(w);
}

struct map {
	unsigned long	b1, e1; unsigned long f1;
	unsigned long	b2, e2; unsigned long f2;
};

struct map datmap;
int	file;

int prcom(int puid)
{
	char abuf[512];
	unsigned long addr;
	register int *ip;
	register char *cp, *cp1;
	long tm;
	int c, nbad;
	register char *tp;
	unsigned long txtsiz, datsiz, stksiz;
	int septxt;
	int lw=(lflg?35:80);
	char **ap;

	if (mproc.p_flag&SLOAD) {
		addr = PAGES2BYTES((unsigned long)mproc.p_addr);
		file = swmem;
	} else {
		addr = mproc.p_addr << BSHIFT;
		file = swap;
	}
	lseek(file, addr, 0);
	if (read(file, (char *)&usr, sizeof(usr)) != sizeof(usr))
		return(0);

	/* set up address maps for user pcs */
	txtsiz = PAGES2BYTES(usr.u_tsize);
	datsiz = PAGES2BYTES(usr.u_dsize);
	stksiz = PAGES2BYTES(usr.u_ssize);
	septxt = usr.u_sep;
	datmap.b1 = (septxt ? 0 : round(txtsiz,TXTRNDSIZ));
	datmap.e1 = datmap.b1+datsiz;
	datmap.f1 = U_SIZE+addr;
	datmap.b2 = stackbas(stksiz);
	datmap.e2 = stacktop(stksiz);
	datmap.f2 = U_SIZE+(datmap.e1-datmap.b1)+addr;

	tp = gettty();
	if (tptr && strncmp(tptr, tp, 2))
		return(0);
	if (lflg) {
		printf("%2o %c%4d", mproc.p_flag,
			"0SWRIZT"[mproc.p_stat], puid);
	}
	printf("%6u", mproc.p_pid);
	if (lflg) {
		printf("%6u%4d%4d%5d%6o%4d",
			mproc.p_ppid,
			mproc.p_cpu&0377,
			mproc.p_pri,
			mproc.p_nice,
			mproc.p_addr,
			mproc.p_size);
		if (mproc.p_wchan)
			printf("%7x", DIRECT2PHYS(mproc.p_wchan));
		else
			printf("       ");
	}
	printf(" %-2.2s", tp);
	if (mproc.p_stat==SZOMB) {
		printf("  <defunct>");
		return(1);
	}
	tm = (usr.u_utime + usr.u_stime + 30)/60;
	printf(" %2ld:", tm/60);
	tm %= 60;
	printf(tm<10?"0%ld":"%ld", tm);
	if (vflg && lflg==0) {	/* 0 == old tflg (print long times) */
		tm = (usr.u_cstime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
		tm = (usr.u_cutime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
	}
	if (mproc.p_pid == 0) {
		printf(" swapper");
		return(1);
	}
	addr += PAGES2BYTES((unsigned long)mproc.p_size) - 512;

	/* look for sh special */
	lseek(file, addr+512-sizeof(char **), 0);
	if (read(file, (char *)&ap, sizeof(char *)) != sizeof(char *))
		return(1);
	if (ap) {
		char b[82];
		char *bp = b;
		while((cp=getptr(ap++)) && cp && (bp<b+lw) ) {
			nbad = 0;
			while((c=getbyte(cp++)) && (bp<b+lw)) {
				if (c<' ' || c>'~') {
					if (nbad++>3)
						break;
					continue;
				}
				*bp++ = c;
			}
			*bp++ = ' ';
		}
		*bp++ = 0;
		printf(lflg?" %.30s":" %.60s", b);
		return(1);
	}

	lseek(file, addr, 0);
	if (read(file, abuf, sizeof(abuf)) != sizeof(abuf))
		return(1);
	for (ip = (int *)&abuf[512]-2; ip > (int *)abuf; ) {
		if (*--ip == -1 || *ip==0) {
			cp = (char *)(ip+1);
			if (*cp==0)
				cp++;
			nbad = 0;
			for (cp1 = cp; cp1 < &abuf[512]; cp1++) {
				c = *cp1&0177;
				if (c==0)
					*cp1 = ' ';
				else if (c < ' ' || c > 0176) {
					if (++nbad >= 5) {
						*cp1++ = ' ';
						break;
					}
					*cp1 = '?';
				} else if (c=='=') {
					*cp1 = 0;
					while (cp1>cp && *--cp1!=' ')
						*cp1 = 0;
					break;
				}
			}
			while (*--cp1==' ')
				*cp1 = 0;
			printf(lflg?" %.30s":" %.60s", cp);
			return(1);
		}
	}
	return(1);
}

char *gettty(void)
{
	register i;
	register char *p;

	if (usr.u_ttyp==0)
		return("?");
	for (i=0; i<ndev; i++) {
		if (devl[i].dev == usr.u_ttyd) {
			p = devl[i].dname;
			if (p[0]=='t' && p[1]=='t' && p[2]=='y')
				p += 3;
			return(p);
		}
	}
	return("?");
}

char *getptr(char **adr)
{
	char *ptr;
	register char *p, *pa;
	register i;

	ptr = 0;
	pa = (char *)adr;
	p = (char *)&ptr;
	for (i=0; i<sizeof(ptr); i++)
		*p++ = getbyte(pa++);
	return(ptr);
}

int getbyte(char *adr)
{
	register struct map *amap = &datmap;
	char b;
	unsigned long saddr;

	if(!within(adr, amap->b1, amap->e1)) {
		if(within(adr, amap->b2, amap->e2)) {
			saddr = (unsigned)adr + amap->f2 - amap->b2;
		} else
			return(0);
	} else
		saddr = (unsigned)adr + amap->f1 - amap->b1;
	if(lseek(file, saddr, 0)==-1
		   || read(file, &b, 1)<1) {
		return(0);
	}
	return((unsigned)b);
}

int within(char *adr, unsigned long lbd, unsigned long ubd)
{
	return((unsigned)adr>=lbd && (unsigned)adr<ubd);
}
