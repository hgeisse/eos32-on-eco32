/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include <stdlib.h>


/*
 *	storage allocator
 *	(circular first fit strategy)
 */

#define BUSY 01
#define busy(x)	(Rcheat((x)->word)&BUSY)

POS		brkincr=BRKINCR;
BLKPTR		blokp;			/*current search pointer*/
BLKPTR		bloktop=BLK(_ebss);	/*top of arena (last blok)*/



ADDRESS	alloc(POS nbytes)
{
	REG POS		rbytes = round(nbytes+BYTESPERWORD,BYTESPERWORD);

	LOOP	INT		c=0;
		REG BLKPTR	p = blokp;
		REG BLKPTR	q;
		REP	IF !busy(p)
			THEN	WHILE !busy(q = p->word) DO p->word = q->word OD
				IF ADR(q)-ADR(p) >= rbytes
				THEN	blokp = BLK(ADR(p)+rbytes);
					IF q > blokp
					THEN	blokp->word = p->word;
					FI
					p->word=BLK(Rcheat(blokp)|BUSY);
					return(ADR(p+1));
				FI
			FI
			q = p; p = BLK(Rcheat(p->word)&~BUSY);
		PER	p>q ORF (c++)==0 DONE
		addblok(rbytes);
	POOL
}

VOID	addblok(POS reqd)
{
	IF stakbas!=staktop
	THEN	REG STKPTR	rndstak;
		REG BLKPTR	blokstak;

		pushstak(0);
		rndstak=(STKPTR)round(staktop,BYTESPERWORD);
		blokstak=BLK(stakbas)-1;
		blokstak->word=stakbsy; stakbsy=blokstak;
		bloktop->word=BLK(Rcheat(rndstak)|BUSY);
		bloktop=BLK(rndstak);
	FI
	reqd += brkincr; reqd &= ~(brkincr-1);
	blokp=bloktop;
	bloktop=bloktop->word=BLK(Rcheat(bloktop)+reqd);
	bloktop->word=BLK(ADR(_ebss)+1);
	BEGIN
	   REG STKPTR stakadr=STK(bloktop+2);
	   staktop=movstr(stakbot,stakadr);
	   stakbas=stakbot=stakadr;
	END
}

VOID	freemem(ADDRESS ap)
{
	REG BLKPTR	p;

	IF (p=ap) ANDF p<bloktop
	THEN	Lcheat((--p)->word) &= ~BUSY;
	FI
}

#ifdef DEBUG
VOID chkbptr(BLKPTR ptr)
{
	INT		exf=0;
	REG BLKPTR	p = (BLKPTR) _ebss;
	REG BLKPTR	q;
	INT		us=0, un=0;

	LOOP
	   q = (BLKPTR) (Rcheat(p->word)&~BUSY);
	   IF p==ptr THEN exf++ FI
	   IF q<(BLKPTR)_ebss ORF q>bloktop THEN abort() FI
	   IF p==bloktop THEN break FI
	   IF busy(p)
	   THEN us += q-p;
	   ELSE un += q-p;
	   FI
	   IF p>=q THEN abort() FI
	   p=q;
	POOL
	IF exf==0 THEN abort() FI
	prn(un); prc(SP); prn(us); prc(NL);
}
#endif
