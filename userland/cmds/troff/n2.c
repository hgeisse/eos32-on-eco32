#include "tdef.h"
#include <eos32.h>
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

/*
troff2.c

output, cleanup
*/

/* PROTOTYPES */
void pchar(int c);
void pchar1(int c);
void oput(char i);
void oputs(char *i);
void flusho(void);
void done(int x);
void done1(int x);
void done2(int x);
void done3(int x);
void edone(int x);
void report(void);
void casepi(void);

/* GLOBAL VARIABLES */

extern struct s *frame, *stk, *nxf;
extern filep ip;
extern filep offset;
extern char *enda;


extern char obuf[OBUFSZ];
extern char *obufp;
extern int dilev;
extern int eschar;
extern int tlss;
extern int tflg;
extern int ascii;
extern int print;
extern char trtab[];
extern int waitf;
extern char ptname[];
extern int ptid;
extern int em;
extern int ds;
extern int mflg;
extern filep woff;
extern int nflush;
extern int lgf;
extern int app;
extern int nfo;
extern int donef;
extern int *pendw;
extern int nofeed;
extern int trap;
extern struct sgttyb ttys;
extern int ttysave;
extern int quiet;
extern int pendnf;
extern int ndone;
extern int lead;
extern int ralss;
extern int paper;
extern int gflag;
extern char *unlkp;
extern char nextf[];
extern int pipeflg;
extern int ejf;
extern int no_out;
extern int level;
extern int stopmesg;
extern int xxx;
int toolate;
int error;
#ifndef NROFF
extern int acctf;
#endif

void pchar(int c) {
	int i, j;

	if((i=c) & MOT){pchar1(i); return;}
	switch(j = i & CMASK){
		case 0:
		case IMP:
		case RIGHT:
		case LEFT:
			return;
		case HX:
			j = (tlss>>9) | ((i&~0777)>>3);
			if(i & 040000){
				j &= ~(040000>>3);
				if(j > dip->blss)dip->blss = j;
			}else{
				if(j > dip->alss)dip->alss = j;
				ralss = dip->alss;
			}
			tlss = 0;
			return;
		case LX:
			tlss = i;
			return;
		case PRESC:
			if(dip == &d[0])j = eschar;
		default:
			i = (trtab[j] & BMASK) | (i & ~CMASK);
	}
	pchar1(i);
}

void pchar1(int c) {
	register i, j, *k;
	extern int chtab[];

	j = (i = c) & CMASK;
	if(dip != &d[0]){
		wbf(i);
		dip->op = offset;
		return;
	}
	if(!tflg && !print){
		if(j == '\n')dip->alss = dip->blss = 0;
		return;
	}
	if(no_out || (j == FILLER))return;
#ifndef NROFF
	if(ascii){
		if(i & MOT){
			oput(' ');
			return;
		}
		if(j < 0177){
			oput(i);
			return;
		}
		switch(j){
			case 0200:
			case 0210:
				oput('-');
				break;
			case 0211:
				oputs("fi");
				break;
			case 0212:
				oputs("fl");
				break;
			case 0213:
				oputs("ff");
				break;
			case 0214:
				oputs("ffi");
				break;
			case 0215:
				oputs("ffl");
				break;
			default:
				for(k=chtab; *++k != j; k++)
					if(*k == 0)return;
				oput('\\');
				oput('(');
				oput(*--k & BMASK);
				oput(*k >> BYTE);
		}
	}else
#endif
	ptout(i);
}

void oput(char i) {
	*obufp++ = i;
	if(obufp == (obuf + OBUFSZ + ascii - 1))flusho();
}

void oputs(char *i) {
	while(*i != 0)oput(*i++);
}

void flusho(void) {
	if(!ascii)*obufp++ = 0;
	if(!ptid){
		while((ptid=open(ptname,1)) < 0){
			if(++waitf <=2)prstr("Waiting for Typesetter.\n");
			sleep(15);
		}
	}
	if(no_out == 0){
		if (!toolate) {
			toolate++;
#ifdef NROFF
			if(t.bset || t.breset){
				if(ttysave == -1) {
					gtty(1, &ttys);
					ttysave = ttys.sg_flags;
				}
				ttys.sg_flags &= ~t.breset;
				ttys.sg_flags |= t.bset;
				stty(1, &ttys);
			}
			{
			char *p = t.twinit;
			while (*p++)
				;
			write(ptid, t.twinit, p-t.twinit-1);
			}
#endif
		}
		toolate += write(ptid, obuf, obufp-obuf);
	}
	obufp = obuf;
}

void done(int x) {
	register i;

	error |= x;
	level = 0;
	app = ds = lgf = 0;
	if(i=em){
		donef = -1;
		em = 0;
		if(control(i,0))longjmp(sjbuf,1);
	}
	if(!nfo)done3(0);
	mflg = 0;
	dip = &d[0];
	if(woff)wbt(0);
	if(pendw)getword(1);
	pendnf = 0;
	if(donef == 1)done1(0);
	donef = 1;
	ip = 0;
	frame = stk;
	nxf = frame + 1;
	if(!ejf)tbreak();
	nflush++;
	eject((struct s *)0);
	longjmp(sjbuf,1);
}

void done1(int x) {
	error |= x;
	if(v.nl){
		trap = 0;
		eject((struct s *)0);
		longjmp(sjbuf,1);
	}
	if(nofeed){
		ptlead();
		flusho();
		done3(0);
	}else{
		if(!gflag)lead += TRAILER;
		done2(0);
	}
}

void done2(int x) {
	register i;

	ptlead();
#ifndef NROFF
	if(!ascii){
		oput(T_INIT);
		oput(T_STOP);
		if(!gflag)for(i=8; i>0; i--)oput(T_PAD);
		if(stopmesg)prstr("Troff finished.\n");
	}
#endif
	flusho();
	done3(x);
}

void done3(int x) {
	error |= x;
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	unlink(unlkp);
#ifdef NROFF
	twdone();
#endif
	if(quiet){
		ttys.sg_flags |= ECHO;
		stty(0, &ttys);
	}
	if(ascii)mesg(1);
#ifndef NROFF
	report();
#endif
	exit(error);
}

void edone(int x) {
	frame = stk;
	nxf = frame + 1;
	ip = 0;
	done(x);
}

#ifndef NROFF
void report(void) {
	struct {int use; int uid;} a;

	if((ptid != 1) && paper ){
		lseek(acctf,0L,2);
		a.use = paper;
		a.uid = getuid();
		write(acctf,(char *)&a,sizeof(a));
	}
}
#endif

#ifdef NROFF
void casepi(void) {
	register i;
	int id[2];

	if(toolate || skip() || !getname() || (pipe(id) == -1) ||
	   ((i=fork()) == -1)){
		prstr("Pipe not created.\n");
		return;
	}
	ptid = id[1];
	if(i>0){
		close(id[0]);
		toolate++;
		pipeflg++;
		return;
	}
	close(0);
	dup(id[0]);
	close(id[1]);
	execl(nextf,nextf,0);
	prstr("Cannot exec: ");
	prstr(nextf);
	prstr("\n");
	exit(-4);
}
#endif
