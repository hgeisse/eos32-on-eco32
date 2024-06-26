#include <eos32.h>
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

/*
troff3.c

macro and string routines, storage allocation
*/

/* PROTOTYPES */

void caseig(void);
void casern(void);
void caserm(void);
void caseas(void);
void caseds(void);
void caseam(void);
void casede(void);
int findmn(int i);
void clrmn(int i);
filep finds(int mn);
int skip(void);
int copyb(void);
void copys(void);
filep alloc(void);
void ffree(filep i);
filep boff(int i);
void wbt(int i);
void wbf(int i);
void wbfl(void);
int blisti(filep i); /*???*/
int rbf(void);
int rbf0(filep p);
filep incoff(filep p);
int popi(void);
filep pushi(filep newip);
char* setbrk(int x);
int getsn(void);
int setstr(void);
void collect(void);
void seta(void);
void caseda(void);
void casedi(void);
void casedt(void);
void casetl(void);
void casepc(void);
int hseg(int (*f)(int), filep p);
void casepm(void);
char *kvt(int k, char *p);
void dummy(void);

/* GLOBAL VARIABLES */

unsigned blist[NBLIST];
extern struct s *frame, *stk, *nxf;
extern filep ip;
extern filep offset;
extern filep nextb;
extern char *enda;

extern int ch;
extern int ibf;
extern int lgf;
extern int copyf;
extern int ch0;
extern int app;
extern int ds;
extern int nlflg;
extern int *argtop;
extern int *ap;
extern int nchar;
extern int pendt;
extern int rchar;
extern int dilev;
extern int nonumb;
extern int lt;
extern int nrbits;
extern int nform;
extern int fmt[];
extern int oldmn;
extern int newmn;
extern int macerr;
extern filep apptr;
extern int diflg;
extern filep woff;
extern filep roff;
extern int wbfi;
extern int po;
extern int *cp;
extern int xxx;
int pagech = '%';
int strflg;
extern struct contab {
	int rq;
	union {
		int (*f)();
		unsigned mx;
	}x;
}contab[NM];
int wbuf[BLK];
int rbuf[BLK];

void caseig(void) {
	register i;

	offset = 0;
	if((i = copyb()) != '.')control(i,1);
}

void casern(void) {
	register i,j;

	lgf++;
	skip();
	if(((i=getrq())==0) || ((oldmn=findmn(i)) < 0))return;
	skip();
	clrmn(findmn(j=getrq()));
	if(j)contab[oldmn].rq = (contab[oldmn].rq & MMASK) | j;
}

void caserm(void) {
	lgf++;
	while(!skip()){
		clrmn(findmn(getrq()));
	}
}

void caseas(void) {
	app++;
	caseds();
}

void caseds(void) {
	ds++;
	casede();
}

void caseam(void) {
	app++;
	casede();
}

void casede(void) {
	register i, req;
	register filep savoff;
	extern filep finds();

	if(dip != d)wbfl();
	req = '.';
	lgf++;
	skip();
	if((i=getrq())==0)goto de1;
	if((offset=finds(i)) == 0)goto de1;
	if(ds)copys();
		else req = copyb();
	wbfl();
	clrmn(oldmn);
	if(newmn)contab[newmn].rq = i | MMASK;
	if(apptr){
		savoff = offset;
		offset = apptr;
		wbt(IMP);
		offset = savoff;
	}
	offset = dip->op;
	if(req != '.')control(req,1);
de1:
	ds = app = 0;
	return;
}

int findmn(int i) {
	register j;

	for(j=0;j<NM;j++){
		if(i == (contab[j].rq & ~MMASK))break;
	}
	if(j==NM)j = -1;
	return(j);
}

void clrmn(int i) {
	extern filep boff();
	if(i >= 0){
		if(contab[i].rq & MMASK)ffree(((filep)contab[i].x.mx)<<BLKBITS);
		contab[i].rq = 0;
		contab[i].x.mx = 0;
	}
}

filep finds(int mn) {
	register i;
	extern filep boff();
	register filep savip;
	extern filep alloc();
	extern filep incoff();

	oldmn = findmn(mn);
	newmn = 0;
	apptr = (filep)0;
	if(app && (oldmn >= 0) && (contab[oldmn].rq & MMASK)){
			savip = ip;
			ip = (((filep)contab[oldmn].x.mx)<<BLKBITS);
			oldmn = -1;
			while((i=rbf()) != 0);
			apptr = ip;
			if(!diflg)ip = incoff(ip);
			nextb = ip;
			ip = savip;
	}else{
		for(i=0;i<NM;i++){
			if(contab[i].rq == 0)break;
		}
		if((i==NM) ||
		   (nextb = alloc()) == 0){
			app = 0;
			if(macerr++ > 1)done2(02);
			prstr("Too many string/macro names.\n");
			edone(04);
			return(offset = 0);
		}
			contab[i].x.mx = (unsigned)(nextb>>BLKBITS);
		if(!diflg){
			newmn = i;
			if(oldmn == -1)contab[i].rq = -1;
		}else{
			contab[i].rq = mn | MMASK;
		}
	}

	app = 0;
	return(offset = nextb);
}

int skip(void) {
	register i;

	while(((i=getch()) & CMASK) == ' ');
	ch=i;
	return(nlflg);
}

int copyb(void) {
	register i, j, k;
	int ii, req, state;
	filep savoff;

	if(skip() || !(j=getrq()))j = '.';
	req = j;
	k = j>>BYTE;
	j &= BMASK;
	copyf++;
	flushi();
	nlflg = 0;
	state = 1;
	while(1){
		i = (ii = getch()) & CMASK;
		if(state == 3){
			if(i == k)break;
			if(!k){
				ch = ii;
				i = getach();
				ch = ii;
				if(!i)break;
			}
			state = 0;
			goto c0;
		}
		if(i == '\n'){
			state = 1;
			nlflg = 0;
			goto c0;
		}
		if((state == 1) && (i == '.')){
			state++;
			savoff = offset;
			goto c0;
		}
		if((state == 2) && (i == j)){
			state++;
			goto c0;
		}
		state = 0;
c0:
		if(offset)wbf(ii);
	}
	if(offset){
		wbfl();
		offset = savoff;
		wbt(0);
	}
	copyf--;
	return(req);
}

void copys(void) { 	
	register i;

	copyf++;
	if(skip())goto c0;
	if(((i=getch()) & CMASK) != '"')wbf(i);
	while(((i=getch()) & CMASK) != '\n')wbf(i);
c0:
	wbt(0);
	copyf--;
}

filep alloc(void) {
	register i;
	extern filep boff();
	filep j;

	for(i=0;i<NBLIST;i++){
		if(blist[i] == 0)break;
	}
	if(i==NBLIST){
		j = 0;
	}else{
		blist[i] = -1;
		if((j = boff(i)) < NEV*EVS)j = 0;
	}
	return(nextb = j);
}

void ffree(filep i) {
	register j;

	while((blist[j = blisti(i)]) != -1){
		i = ((filep)blist[j])<<BLKBITS;
		blist[j] = 0;
	}
	blist[j] = 0;
}

filep boff(int i) {
	return(((filep)i)*BLK + NEV*EVS);
}

void wbt(int i) {
	wbf(i);
	wbfl();
}

void wbf(int i) {
	register j;

	if(!offset)return;
	if(!woff){
		woff = offset;
		wbfi = 0;
	}
	wbuf[wbfi++] = i;
	if(!((++offset) & (BLK-1))){
		wbfl();
		if(blist[j = blisti(--offset)] == -1){
			if(alloc() == 0){
				prstr("Out of temp file space.\n");
				done2(01);
			}
			blist[j] = (unsigned)(nextb>>BLKBITS);
		}
		offset = ((filep)blist[j])<<BLKBITS;
	}
	if(wbfi >= BLK)wbfl();
}

void wbfl(void) {
	if(woff == 0)return;
	lseek(ibf, ((long)woff) * sizeof(int), 0);
	write(ibf, (char *)wbuf, wbfi * sizeof(int));
	if((woff & (~(BLK-1))) == (roff & (~(BLK-1))))roff = -1;
	woff = 0;
}

int blisti(filep i) {
	return((i-NEV*EVS)/(BLK));
}

int rbf(void) {
	register i;
	extern filep incoff();

	if((i=rbf0(ip)) == 0){
		if(!app)i = popi();
	}else{
		ip = incoff(ip);
	}
	return(i);
}

int rbf0(filep p) {
	register filep i;

	if((i = (p & (~(BLK-1)))) != roff){
		roff = i;
		lseek(ibf, ((long)roff) * sizeof(int), 0);
		if(read(ibf, (char *)rbuf, BLK * sizeof(int)) == 0)return(0);
	}
	return(rbuf[p & (BLK-1)]);
}

filep incoff(filep p) {
	register i;
	register filep j;
	if(!((j = (++p)) & (BLK-1))){
		if((i = blist[blisti(--p)]) == -1){
			prstr("Bad storage allocation.\n");
			done2(-5);
		}
		j = ((filep)i)<<BLKBITS;
	}
	return(j);
}

int popi(void) {
	register struct s *p;

	if(frame == stk)return(0);
	if(strflg)strflg--;
	p = nxf = frame;
	p->nargs = 0;
	frame = p->pframe;
	ip = p->pip;
	nchar = p->pnchar;
	rchar = p->prchar;
	pendt = p->ppendt;
	ap = p->pap;
	cp = p->pcp;
	ch0 = p->pch0;
	return(p->pch);
}

filep pushi(filep newip) {
	register struct s *p;
	extern char *setbrk();

	if((enda - sizeof(struct s)) < (char *)nxf)setbrk(DELTA);
	p = nxf;
	p->pframe = frame;
	p->pip = ip;
	p->pnchar = nchar;
	p->prchar = rchar;
	p->ppendt = pendt;
	p->pap = ap;
	p->pcp = cp;
	p->pch0 = ch0;
	p->pch = ch;
	cp = ap = 0;
	nchar = rchar = pendt = ch0 = ch = 0;
	frame = nxf;
	if(nxf->nargs == 0) nxf += 1;
		else nxf = (struct s *)argtop;
	return(ip = newip);
}

char* setbrk(int x) {
	register char *i;
	char *sbrk();

	if((i = sbrk(x)) == MAXPTR){
		prstrfl("Core limit reached.\n");
		edone(0100);
	}else{
		enda = i + x;
	}
	return(i);
}

int getsn(void) {
	register i;

	if((i=getach()) == 0)return(0);
	if(i == '(')return(getrq());
		else return(i);
}

int setstr(void) {
	register i;

	lgf++;
	if(((i=getsn()) == 0) ||
	   ((i=findmn(i)) == -1) ||
	   !(contab[i].rq & MMASK)){
		lgf--;
		return(0);
	}else{
		if((enda-2) < (char *)nxf)setbrk(DELTA);
		nxf->nargs = 0;
		strflg++;
		lgf--;
		return(pushi(((filep)contab[i].x.mx)<<BLKBITS));
	}
}

void collect(void) {
	register i;
	register int *strp;
	int *lim;
	int **argpp, **argppend;
	int quote;
	struct s *savnxf;

	copyf++;
	nxf->nargs = 0;
	savnxf = nxf;
	if(skip())goto rtn;
	lim = (int *)(nxf = savnxf + sizeof(struct s)/sizeof(savnxf));
	strflg = 0;
	if((argppend =
		(argpp = (int **)savnxf+(sizeof(struct s)/sizeof(int **))) + (sizeof(struct s)-1))
		> (int **)enda)setbrk(DELTA);
	strp = (int *)argppend;
	for(i=8; i>=0; i--)argpp[i] = 0;
	while((argpp != argppend) && (!skip())){
		*argpp++ = strp;
		quote = 0;
		if(((i = getch()) & CMASK) == '"')quote++;
			else ch = i;
		while(1){
			i = getch();
			if( nlflg ||
			  ((!quote) && ((i & CMASK) == ' ')))break;
			if(quote && ((i & CMASK) == '"') &&
			  (((i=getch()) & CMASK) != '"')){
				ch = i;
				break;
			}
			*strp++ = i;
			if(strflg && (strp >= lim)){
				prstrfl("Macro argument too long.\n");
				copyf--;
				edone(004);
			}
			if((enda-4) <= (char *)strp)setbrk(DELTA);
		}
		*strp++ = 0;
	}
	nxf = savnxf;
	nxf->nargs = argpp -(int **)(nxf + 1);
	argtop = strp;
rtn:
	copyf--;
}

void seta(void) {
	register i;

	if(((i = (getch() & CMASK) - '0') > 0) &&
		(i <= 9) && (i <= frame->nargs))ap = *((int **)frame + i-1 + (sizeof(struct s)/sizeof(int **)));
}

void caseda(void) {
	app++;
	casedi();
}

void casedi(void) {
	register i, j;
	register *k;

	lgf++;
	if(skip() || ((i=getrq()) == 0)){
		if(dip != d)wbt(0);
		if(dilev > 0){
			v.dn = dip->dnl;
			v.dl = dip->maxl;
			dip = &d[--dilev];
			offset = dip->op;
		}
		goto rtn;
	}
	if(++dilev == NDI){
		--dilev;
		prstr("Cannot divert.\n");
		edone(02);
	}
	if(dip != d)wbt(0);
	diflg++;
	dip = &d[dilev];
	dip->op = finds(i);
	dip->curd = i;
	clrmn(oldmn);
	k = (int *)&dip->dnl;
	for(j=0; j<10; j++)k[j] = 0;	/*not op and curd*/
rtn:
	app = 0;
	diflg = 0;
}

void casedt(void) {
	lgf++;
	dip->dimac = dip->ditrap = dip->ditf = 0;
	skip();
	dip->ditrap = vnumb((int *)0);
	if(nonumb)return;
	skip();
	dip->dimac = getrq();
}

void casetl(void) {
	register i, j;
	int w1, w2, w3, delim;
	filep begin;
	extern width(), pchar();

	dip->nls = 0;
	skip();
	if(dip != d)wbfl();
	if((offset = begin = alloc()) == 0)return;
	if((delim = getch()) & MOT){
		ch = delim;
		delim = '\'';
	}else delim &= CMASK;
	if(!nlflg)
		while(((i = getch()) & CMASK) != '\n'){
			if((i & CMASK) == delim)i = IMP;
			wbf(i);
		}
	wbf(IMP);wbf(IMP);wbt(0);

	w1 = hseg(width,begin);
	w2 = hseg(width,(filep)0);
	w3 = hseg(width,(filep)0);
	offset = dip->op;
#ifdef NROFF
	if(!offset)horiz(po);
#endif
	hseg(pchar,begin);
	if(w2 || w3)horiz(j=quant((lt - w2)/2-w1,HOR));
	hseg(pchar,(filep)0);
	if(w3){
		horiz(lt-w1-w2-w3-j);
		hseg(pchar,(filep)0);
	}
	newline(0);
	if(dip != d){if(dip->dnl > dip->hnl)dip->hnl = dip->dnl;}
	else{if(v.nl > dip->hnl)dip->hnl = v.nl;}
	ffree(begin);
}

void casepc(void) {
	pagech = chget(IMP);
}

int hseg(int (*f)(int), filep p) {
	register acc, i;
	static filep q;

	acc = 0;
	if(p)q = p;
	while(1){
		i = rbf0(q);
		q = incoff(q);
		if(!i || (i == IMP))return(acc);
		if((i & CMASK) == pagech){
			nrbits = i & ~CMASK;
			nform = fmt[findr('%')];
			acc += fnumb(v.pn,f);
		}else acc += (*f)(i);
	}
}

void casepm(void) {
	register i, k;
	register char *p;
	int xx, cnt, kk, tot;
	filep j;
	char *kvt();
	char pmline[10];

	kk = cnt = 0;
	tot = !skip();
	for(i = 0; i<NM; i++){
		if(!((xx = contab[i].rq) & MMASK))continue;
		p = pmline;
		j = (((filep)contab[i].x.mx)<<BLKBITS);
		k = 1;
		while((j = blist[blisti(j)]) != -1){k++; j <<= BLKBITS;}
		cnt++;
		kk += k;
		if(!tot){
			*p++ = xx & 0177;
			if(!(*p++ = (xx >> BYTE) & 0177))*(p-1) = ' ';
			*p++ = ' ';
			kvt(k,p);
			prstr(pmline);
		}
	}
	if(tot || (cnt > 1)){
		kvt(kk,pmline);
		prstr(pmline);
	}
}

char *kvt(int k, char *p) {
	if(k>=100)*p++ = k/100 + '0';
	if(k>=10)*p++ = (k%100)/10 + '0';
	*p++ = k%10 + '0';
	*p++ = '\n';
	*p = 0;
	return(p);
}

void dummy(void) {
	/* noop */
}
