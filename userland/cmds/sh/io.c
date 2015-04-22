/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include <eos32.h>
#include	"defs.h"


IOPTR		iotemp;
INT		serial;


/* ========	input output and file copying ======== */

VOID initf(UFD fd)
{
	REG FILE	f=standin;

	f->fdes=fd; f->fsiz=((flags&(oneflg|ttyflg))==0 ? BUFSIZ : 1);
	f->fnxt=f->fend=f->fbuf; f->feval=0; f->flin=1;
	f->feof=FALSE;
}

INT estabf(STRING s)
{
	REG FILE	f;

	(f=standin)->fdes = -1;
	f->fend=length(s)+(f->fnxt=s);
	f->flin=1;
	return(f->feof=(s==0));
}

VOID push(FILE af)
{
	REG FILE	f;

	(f=af)->fstak=standin;
	f->feof=0; f->feval=0;
	standin=f;
}

INT pop(VOID)
{
	REG FILE	f;

	IF (f=standin)->fstak
	THEN	IF f->fdes>=0 THEN close(f->fdes) FI
		standin=f->fstak;
		return(TRUE);
	ELSE	return(FALSE);
	FI
}

VOID chkpipe(INT *pv)
{
	IF pipe(pv)<0 ORF pv[INPIPE]<0 ORF pv[OTPIPE]<0
	THEN	error(piperr);
	FI
}

INT chkopen(STRING idf)
{
	REG INT		rc;

	IF (rc=open(idf,0))<0
	THEN	failed(idf,badopen);
	ELSE	return(rc);
	FI
	/* HG: never reached */
	return -1;
}

VOID renam(INT f1, INT f2)
{
	IF f1!=f2
	THEN	dup2(f1, f2);
		close(f1);
		IF f2==0 THEN ioset|=1 FI
	FI
}

INT create(STRING s)
{
	REG INT		rc;

	IF (rc=creat(s,0666))<0
	THEN	failed(s,badcreate);
	ELSE	return(rc);
	FI
	/* HG: never reached */
	return -1;
}

INT tmpfil(VOID)
{
	itos(serial++); movstr(numbuf,tmpname);
	return(create(tmpout));
}

/* set by trim */
extern BOOL	nosubst;

VOID copy(IOPTR ioparg)
{
	CHAR		c, *ends;
	REG CHAR	*cline, *clinep;
	INT		fd;
	REG IOPTR	iop;

	IF iop=ioparg
	THEN	copy(iop->iolst);
		ends=mactrim(iop->ioname); IF nosubst THEN iop->iofile &= ~IODOC FI
		fd=tmpfil();
		iop->ioname=cpystak(tmpout);
		iop->iolst=iotemp; iotemp=iop;
		cline=locstak();

		LOOP	clinep=cline; chkpr(NL);
			WHILE (c = (nosubst ? readc() :  nextc(*ends)),  !eolchar(c)) DO *clinep++ = c OD
			*clinep=0;
			IF eof ORF eq(cline,ends) THEN break FI
			*clinep++=NL;
			write(fd,cline,clinep-cline);
		POOL
		close(fd);
	FI
}
