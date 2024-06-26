#include <eos32.h>
#include <sys/stat.h>
#include "tdef.h"
extern
#include "d.h"
extern
#include "v.h"
#ifdef NROFF
extern
#include "tw.h"
#endif
#include "s.h"
#include <setjmp.h>
jmp_buf sjbuf = {0};

/*
troff1.c

consume options, initialization, main loop,
input routines, escape function calling
*/

/* PROTOTYPES FOR MODERN C*/
void catch(int x);
void fpecatch(int x);
void kcatch(int x);
void init1(char a);
void init2(void);
void cvtime(void);
void mesg(int f);
void prstrfl(char *s);
void prstr(char *s);
int control(int a, int b);
int getrq(void);
void flushi(void);
int getach(void);
void getpn(char *a);
void setrpt(void);


/* GLOBAL VARIABLES */

extern struct s *frame, *stk, *nxf;
extern struct s *ejl, *litlev;
extern filep ip;
extern filep offset;
extern filep nextb;


extern int stdi;
extern int waitf;
extern int nofeed;
extern int quiet;
extern int ptid;
extern int ascii;
extern int npn;
extern int xflg;
extern int stop;
extern char ibuf[IBUFSZ];
extern char xbuf[IBUFSZ];
extern char *ibufp;
extern char *xbufp;
extern char *eibuf;
extern char *xeibuf;
extern int cbuf[NC];
extern int *cp;
extern int *vlist;
extern int nx;
extern int mflg;
extern int ch;
extern int pto;
extern int pfrom;
extern int cps;
extern int chbits;
extern int ibf;
extern int ttyod;
extern struct sgttyb ttys;
extern int iflg;
extern int init;
extern int rargc;
extern char **argp;
extern char trtab[256];
extern int lgf;
extern int copyf;
extern int eschar;
extern int ch0;
extern int cwidth;
extern int nlflg;
extern int *ap;
extern int donef;
extern int nflush;
extern int nchar;
extern int rchar;
extern int nfo;
extern int ifile;
extern int fc;
extern int padc;
extern int tabc;
extern int dotc;
extern int raw;
extern int tabtab[NTAB];
extern char nextf[];
extern int nfi;
#ifdef NROFF
extern char termtab[];
extern int tti;
#endif
extern int ifl[NSO];
extern int ifi;
extern int pendt;
extern int flss;
extern int fi;
extern int lg;
extern char ptname[];
extern int print;
extern int nonumb;
extern int pnlist[];
extern int *pnp;
extern int nb;
extern int trap;
extern int tflg;
extern int ejf;
extern int lit;
extern int cc;
extern int c2;
extern int spread;
extern int gflag;
extern int oline[];
extern int *olinep;
extern int dpn;
extern int noscale;
extern char *unlkp;
extern int pts;
extern int level;
extern int ttysave;
extern int tdelim;
extern int dotT;
extern int tabch, ldrch;
extern int eqflg;
extern no_out;
extern int hflg;
#ifndef NROFF
extern char codetab[];
extern int spbits;
#endif
extern int xxx;
int stopmesg;
filep ipl[NSO];
long offl[NSO];
long ioff;
char *ttyp;
extern struct contab {
	int rq;
	union {
		int (*f)();
		unsigned mx;
	}x;
}contab[NM];
int ms[] = {31,28,31,30,31,30,31,31,30,31,30,31};
#ifndef NROFF
int acctf;
#endif

main(int argc, char **argv) {
	char *p, *q;
	int i, j;

	signal(SIGHUP, catch);
	if(signal(SIGINT,catch) == SIG_IGN){
		signal(SIGHUP,SIG_IGN);
		signal(SIGINT,SIG_IGN);
		signal(SIGQUIT,SIG_IGN);
	}
	signal(SIGFPE,fpecatch);
	signal(SIGPIPE,catch);
	signal(SIGTERM,kcatch);
	init1(argv[0][0]);
options:
	while(--argc > 0 && (++argv)[0][0]=='-')
		switch(argv[0][1]){

		case 0:
			goto start;
		case 'i':
			stdi++;
			continue;
		case 'q':
			quiet++;
			if(gtty(0, &ttys) >= 0)
				ttysave = ttys.sg_flags;
			continue;
		case 'n':
			npn = cnum(&argv[0][2]);
			continue;
		case 'p':
			xflg = 0;
			cps = cnum(&argv[0][2]);
			continue;
		case 'S':
			stopmesg++;
			continue;
		case 's':
			if(!(stop = cnum(&argv[0][2])))stop++;
			continue;
		case 'r':
			vlist[findr(argv[0][2])] = cnum(&argv[0][3]);
			continue;
		case 'm':
			p = &nextf[nfi];
			q = &argv[0][2];
			while((*p++ = *q++) != 0);
			mflg++;
			continue;
		case 'o':
			getpn(&argv[0][2]);
			continue;
#ifdef NROFF
		case 'h':
			hflg++;
			continue;
		case 'z':
			no_out++;
			continue;
		case 'e':
			eqflg++;
			continue;
		case 'T':
			p = &termtab[tti];
			q = &argv[0][2];
			if(!((*q) & 0177))continue;
			while((*p++ = *q++) != 0);
			dotT++;
			continue;
#endif
#ifndef NROFF
		case 'z':
			no_out++;
		case 'a':
			ascii = 1;
			nofeed++;
		case 't':
			ptid = 1;
			continue;
		case 'w':
			waitf = 1;
			continue;
		case 'f':
			nofeed++;
			continue;
		case 'x':
			xflg = 0;
			continue;
		case 'b':
			if(open(ptname,1) < 0)prstr("Busy.\n");
			else prstr("Available.\n");
			done3(0);
		case 'g':
			stop = ptid = gflag = 1;
			dpn = 0;
			continue;
#endif
		default:
			pto = cnum(&argv[0][1]);
			continue;
		}

	if(argv[0][0] == '+'){
		pfrom = cnum(&argv[0][1]);
		print = 0;
		if(argc > 0)goto options;
	}

start:
	argp = argv;
	rargc = argc;
	init2();
	setjmp(sjbuf);
loop:
	copyf = lgf = nb = nflush = nlflg = 0;
	if(ip && (rbf0(ip)==0) && ejf && (frame->pframe <= ejl)){
		nflush++;
		trap = 0;
		eject((struct s *)0);
		goto loop;
	}
	i = getch();
	if(pendt)goto lt;
	if(lit && (frame <= litlev)){
		lit--;
		goto lt;
	}
	if((j = (i & CMASK)) == XPAR){
		copyf++;
		tflg++;
		for(;(i & CMASK) != '\n';)pchar(i = getch());
		tflg = 0;
		copyf--;
		goto loop;
	}
	if((j == cc) || (j == c2)){
		if(j == c2)nb++;
		copyf++;
		while(((j=((i=getch()) & CMASK)) == ' ') ||
			(j == '\t'));
		ch = i;
		copyf--;
		control(getrq(),1);
		flushi();
		goto loop;
	}
lt:
	ch = i;
	text();
	goto loop;
}

void catch(int x) {
	/*
		prstr("Interrupt\n");
	*/
	done3(01);
}

void fpecatch(int x) {
	prstrfl("Floating Exception.\n");
	signal(SIGFPE,fpecatch);
}

void kcatch(int x) {
	signal(SIGTERM,SIG_IGN);
	done3(01);
}

#ifndef NROFF
void acctg(void) {
	static char *acct_file = "/usr/adm/tracct";
	acctf = open(acct_file,1);
	setuid(getuid());
}
#endif
void init1(char a) {
	register char *p;
	char *mktemp();
	register i;

#ifndef NROFF
	acctg();/*open troff actg file while mode 4755*/
#endif
	p = mktemp("/tmp/taXXXXX");
	if(a == 'a')p = &p[5];
	if((close(creat(p, 0600))) < 0){
		prstr("Cannot create temp file.\n");
		exit(-1);
	}
	ibf = open(p, 2);
	for(i=256; --i;)trtab[i]=i;
	trtab[UNPAD] = ' ';
	mchbits();
	if(a != 'a')unlkp = p;
}

void init2(void) {
	register i,j;
	extern int block;
	extern char *setbrk();
	extern char *ttyname();

	ttyod = 2;
	if(((ttyp=ttyname(j=0)) != (char *)0) ||
	   ((ttyp=ttyname(j=1)) != (char *)0) ||
	   ((ttyp=ttyname(j=2)) != (char *)0)
	  );else ttyp = "notty";
	iflg = j;
	if(ascii)mesg(0);

	if((!ptid) && (!waitf)){
		if((ptid = open(ptname,1)) < 0){
			prstr("Typesetter busy.\n");
			done3(-2);
		}
	}
	ptinit();
	for(i=NEV; i--;)write(ibf, (char *)&block, EVS*sizeof(int));
	olinep = oline;
	ibufp = eibuf = ibuf;
	v.hp = init = 0;
	ioff = 0;
	v.nl = -1;
	cvtime();
	frame = stk = (struct s *)setbrk(DELTA);
	dip = &d[0];
	nxf = frame + 1;
	nx = mflg;
}

void cvtime(void) {
	long tt;
	register i;

	time(&tt);
	tt -= 3600*ZONE;	/*5hrs for EST*/
	v.dy = (tt/86400L) + 1;
	v.dw = (v.dy + 3)%7 + 1;
	for(v.yr=70;; v.yr++){
		if((v.yr)%4)ms[1]=28;else ms[1]=29;
		for(i=0;i<12;){
			if(v.dy<=ms[i]){
				v.mo = i+1;
				return;
			}
			v.dy -= ms[i++];
		}
	}
}

int cnum(char *a) {
	int i;

	ibufp = a;
	eibuf = MAXPTR;
	i = atoi();
	ch = 0;
	return i;
}

void mesg(int f) {
	static int mode;

	if(!f){
		stat(ttyp, (struct stat *)(cbuf));
		mode = ((struct stat *)(cbuf))->st_mode;
		chmod(ttyp,mode & ~022);
	}else{
		chmod(ttyp,mode);
	}
}

void prstrfl(char *s) {
	flusho();
	prstr(s);
}


void prstr(char *s) {
	int i;
	char *j;

	j = s;
	for(i=0;*s;i++)s++;
	write(ttyod,j,i);
}


int control(int a, int b) {
	int i, j;
	extern filep boff();

	i = a;
	if((i == 0) || ((j = findmn(i)) == -1))return(0);
	if(contab[j].rq & MMASK){
		nxf->nargs = 0;
		if(b)collect();
		flushi();
		return(pushi(((filep)contab[j].x.mx)<<BLKBITS));
	}else{
		if(!b)return(0);
		return((*contab[j].x.f)(0));
	}
}

int getrq(void) {
	int i, j;

	if(((i=getach()) == 0) ||
	   ((j=getach()) == 0))goto rtn;
	i = PAIR(i,j);
rtn:
	return(i);
}

int getch(void) {
	int i, j, k;

	level++;
g0:
	if(ch){
		if(((i = ch) & CMASK) == '\n')nlflg++;
		ch = 0;
		level--;
		return(i);
	}

	if(nlflg){
		level--;
		return('\n');
	}

	if((k = (i = getch0()) & CMASK) != ESC){
		if(i & MOT)goto g2;
		if(k == FLSS){
			copyf++; raw++;
			i = getch0();
			if(!fi)flss = i;
			copyf--; raw--;
			goto g0;
		}
		if(k == RPT){
			setrpt();
			goto g0;
		}
		if(!copyf){
			if((k == 'f') && lg && !lgf){
				i = getlg(i);
				goto g2;
			}
			if((k == fc) || (k == tabch) || (k == ldrch)){
				if((i=setfield(k)) == 0)goto g0; else goto g2;
			}
			if(k == 010){
				i = makem(-width(' ' | chbits));
				goto g2;
			}
		}
		goto g2;
	}
	k = (j = getch0()) & CMASK;
	if(j & MOT){
		i = j;
		goto g2;
	}
/*
	if(k == tdelim){
		i = TDELIM;
		tdelim = IMP;
		goto g2;
	}
*/
	switch(k){

		case '\n':	/*concealed newline*/
			goto g0;
		case 'n':	/*number register*/
			setn();
			goto g0;
		case '*':	/*string indicator*/
			setstr();
			goto g0;
		case '$':	/*argument indicator*/
			seta();
			goto g0;
		case '{':	/*LEFT*/
			i = LEFT;
			goto gx;
		case '}':	/*RIGHT*/
			i = RIGHT;
			goto gx;
		case '"':	/*comment*/
			while(((i=getch0()) & CMASK ) != '\n');
			goto g2;
		case ESC:	/*double backslash*/
			i = eschar;
			goto gx;
		case 'e':	/*printable version of current eschar*/
			i = PRESC;
			goto gx;
		case ' ':	/*unpaddable space*/
			i = UNPAD;
			goto gx;
		case '|':	/*narrow space*/
			i = NARSP;
			goto gx;
		case '^':	/*half of narrow space*/
			i = HNSP;
			goto gx;
		case '\'':	/*\(aa*/
			i = 0222;
			goto gx;
		case '`':	/*\(ga*/
			i = 0223;
			goto gx;
		case '_':	/*\(ul*/
			i = 0224;
			goto gx;
		case '-':	/*current font minus*/
			i = 0210;
			goto gx;
		case '&':	/*filler*/
			i = FILLER;
			goto gx;
		case 'c':	/*to be continued*/
			i = CONT;
			goto gx;
		case ':':	/*lem's char*/
			i = COLON;
			goto gx;
		case '!':	/*transparent indicator*/
			i = XPAR;
			goto gx;
		case 't':	/*tab*/
			i = '\t';
			goto g2;
		case 'a':	/*leader (SOH)*/
			i = LEADER;
			goto g2;
		case '%':	/*ohc*/
			i = OHC;
			goto g2;
		case '.':	/*.*/
			i = '.';
		gx:
			i = (j & ~CMASK) | i;
			goto g2;
	}
	if(!copyf)
		switch(k){

			case 'p':	/*spread*/
				spread++;
				goto g0;
			case '(':	/*special char name*/
				if((i=setch()) == 0)goto g0;
				break;
			case 's':	/*size indicator*/
				setps();
				goto g0;
			case 'f':	/*font indicator*/
				setfont(0);
				goto g0;
			case 'w':	/*width function*/
				setwd();
				goto g0;
			case 'v':	/*vert mot*/
				if(i = vmot())break;
				goto g0;
			case 'h': 	/*horiz mot*/
				if(i = hmot())break;
				goto g0;
			case 'z':	/*zero with char*/
				i = setz();
				break;
			case 'l':	/*hor line*/
				setline();
				goto g0;
			case 'L':	/*vert line*/
				setvline();
				goto g0;
			case 'b':	/*bracket*/
				setbra();
				goto g0;
			case 'o':	/*overstrike*/
				setov();
				goto g0;
			case 'k':	/*mark hor place*/
				if((i=findr(getsn())) == -1)goto g0;
				vlist[i] = v.hp;
				goto g0;
			case 'j':	/*mark output hor place*/
				if(!(i=getach()))goto g0;
				i = (i<<BYTE) | JREG;
				break;
			case '0':	/*number space*/
				i = makem(width('0' | chbits));
				break;
			case 'x':	/*extra line space*/
				if(i = xlss())break;
				goto g0;
			case 'u':	/*half em up*/
			case 'r':	/*full em up*/
			case 'd':	/*half em down*/
				i = sethl(k);
				break;
			default:
				i = j;
		}
	else{
		ch0 = j;
		i = eschar;
	}
g2:
	if((i & CMASK) == '\n'){
		nlflg++;
		v.hp = 0;
		if(ip == 0)v.cd++;
	}
	if(!--level){
		j = width(i);
		v.hp += j;
		cwidth = j;
	}
	return(i);
}

char ifilt[32] = {0,001,002,003,0,005,006,007,010,011,012};

int getch0(void) {
	register int i, j;

	if(ch0){i=ch0; ch0=0; return(i);}
	if(nchar){nchar--; return(rchar);}

again:
	if(cp){
		if((i = *cp++) == 0){
			cp = 0;
			goto again;
		}
	}else if(ap){
		if((i = *ap++) == 0){
			ap = 0;
			goto again;
		}
	}else if(ip){
		if(ip == -1)i = rdtty();
		else i = rbf();
	}else{
		if(donef)done(0);
		if(nx || ((ibufp >= eibuf) && (ibufp != MAXPTR))){
			if(nfo)goto g1;
		g0:
			if(nextfile()){
				if(ip)goto again;
				if(ibufp < eibuf)goto g2;
			}
		g1:
			nx = 0;
			if((j=read(ifile,ibuf,IBUFSZ)) <= 0)goto g0;
			ibufp = ibuf;
			eibuf = ibuf + j;
			if(ip)goto again;
		}
	g2:
		i = *ibufp++ & 0177;
		ioff++;
		if(i >= 040)goto g4; else i = ifilt[i];
	}
	if(raw)return(i);
	if((j = i & CMASK) == IMP)goto again;
	if((i == 0) && !init)goto again;
g4:
	if((copyf == 0) && ((i & ~BMASK) == 0) && ((i & CMASK) < 0370))
#ifndef NROFF
		if(spbits && (i>31) && ((codetab[i-32] & 0200))) i |= spbits;
		else
#endif
		i |= chbits;
	if((i & CMASK) == eschar)i = (i & ~CMASK) | ESC;
	return(i);
}

int nextfile(void) {
	char *p;

n0:
	if(ifile)close(ifile);
	if(nx){
		p = nextf;
		if(*p != 0)goto n1;
	}
	if(ifi > 0){
		if(popf())goto n0; /*popf error*/
		return(1); /*popf ok*/
	}
	if(rargc-- <= 0)goto n2;
	p = (argp++)[0];
n1:
	if((p[0] == '-') && (p[1] == 0)){
		ifile = 0;
	}else if((ifile=open(p,0)) < 0){
		prstr("Cannot open ");
		prstr(p);
		prstr("\n");
		nfo -= mflg;
		done(02);
	}
	nfo++;
	v.cd = 0;
	ioff = 0;
	return(0);
n2:
	if((nfo -= mflg) && !stdi)done(0);
	nfo++;
	v.cd = ifile =  stdi = mflg = 0;
	ioff = 0;
	return(0);
}

int popf(void) {
	register i;
	register char *p, *q;
	extern char *ttyname();

	ioff = offl[--ifi];
	ip = ipl[ifi];
	if((ifile = ifl[ifi]) == 0){
		p = xbuf;
		q = ibuf;
		ibufp = xbufp;
		eibuf = xeibuf;
		while(q < eibuf)*q++ = *p++;
		return(0);
	}
	if((lseek(ifile,(long)(ioff & ~(IBUFSZ-1)),0) < 0) ||
	   ((i = read(ifile,ibuf,IBUFSZ)) < 0))return(1);
	eibuf = ibuf + i;
	ibufp = ibuf;
	if(ttyname(ifile) == (char *)0)
		if((ibufp = ibuf + (int)(ioff & (IBUFSZ-1)))  >= eibuf)return(1);
	return(0);
}

void flushi(void) {
	if(nflush)return;
	ch = 0;
	if((ch0 & CMASK) == '\n')nlflg++;
	ch0 = 0;
	copyf++;
	while(!nlflg){
		if(donef && (frame == stk))break;
		getch();
	}
	copyf--;
	v.hp = 0;
}

int getach(void) {
	register i;

	lgf++;
	if(((i = getch()) & MOT) ||
	    ((i&CMASK) == ' ') ||
	    ((i&CMASK) == '\n')||
	    (i & 0200)){
			ch = i;
			i = 0;
	}
	lgf--;
	return(i & 0177);
}

void casenx(void) {
	lgf++;
	skip();
	getname();
	nx++;
	nextfile();
	nlflg++;
	ip = 0;
	ap = 0;
	nchar = pendt = 0;
	frame = stk;
	nxf = frame + 1;
}

int getname(void) {
	int i, j, k;

	lgf++;
	for(k=0; k < (NS-1); k++){
		if(((j=(i=getch()) & CMASK) <= ' ') ||
			(j > 0176))break;
		nextf[k] = j;
	}
	nextf[k] = 0;
	ch = i;
	lgf--;
	return(nextf[0]);
}

void caseso(void) {
	register i;
	register char *p, *q;

	lgf++;
	nextf[0] = 0;
	if(skip() || !getname() || ((i=open(nextf,0)) <0) || (ifi >= NSO)) {
		prstr("can't open file ");
		prstr(nextf);
		prstr("\n");
		done(02);
	}
	flushi();
	ifl[ifi] = ifile;
	ifile = i;
	offl[ifi] = ioff;
	ioff = 0;
	ipl[ifi] = ip;
	ip = 0;
	nx++;
	nflush++;
	if(!ifl[ifi++]){
		p = ibuf;
		q = xbuf;
		xbufp = ibufp;
		xeibuf = eibuf;
		while(p < eibuf)*q++ = *p++;
	}
}

void casecf(void) {	/* copy file without change */
	int fd, i, n;
	char buf[512];

	flusho();
	lgf++;
	nextf[0] = 0;
	if(skip() || !getname() || ((fd=open(nextf,0)) <0) || (ifi >= NSO)) {
		prstr("can't open file ");
		prstr(nextf);
		prstr("\n");
		done(02);
	}
	while ((n = read(fd, buf, 512)) > 0)
		for (i = 0; i < n; i++)
			oput(buf[i]);
	flusho();
	close(fd);
}

void getpn(char *a) {
	register i, neg;
	long atoi1();

	if((*a & 0177) == 0)return;
	neg = 0;
	ibufp = a;
	eibuf = MAXPTR;
	noscale++;
	while((i = getch() & CMASK) != 0)switch(i){
		case '+':
		case ',':
			continue;
		case '-':
			neg = MOT;
			goto d2;
		default:
			ch = i;
		d2:
			i = atoi1();
			if(nonumb)goto fini;
			else{
				*pnp++ = i | neg;
				neg = 0;
				if(pnp >= &pnlist[NPN-2]){
					prstr("Too many page numbers\n");
					done3(-3);
				}
			}
		}
fini:
	if(neg)*pnp++ = -2;
	*pnp = -1;
	ch = noscale = print = 0;
	pnp = pnlist;
	if(*pnp != -1)chkpn();
}


void setrpt(void) {
	int i, j;

	copyf++;raw++;
	i = getch0();
	copyf--;raw--;
	if((i < 0) ||
	   (((j = getch0()) & CMASK) == RPT))return;
	rchar = j;
	nchar = i & BMASK;
}
