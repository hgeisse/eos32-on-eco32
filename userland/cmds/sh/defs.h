/*
 *	UNIX shell
 */

/* error exits from various parts of shell */
#define ERROR	1
#define SYNBAD	2
#define SIGFAIL 3
#define SIGFLG	0200

/* command tree */
#define FPRS	020
#define FINT	040
#define FAMP	0100
#define FPIN	0400
#define FPOU	01000
#define FPCL	02000
#define FCMD	04000
#define COMMSK	017

#define TCOM	0
#define TPAR	1
#define TFIL	2
#define TLST	3
#define TIF	4
#define TWH	5
#define TUN	6
#define TSW	7
#define TAND	8
#define TORF	9
#define TFORK	10
#define TFOR	11

/* execute table */
#define SYSSET	1
#define SYSCD	2
#define SYSEXEC	3
#define SYSLOGIN 4
#define SYSTRAP	5
#define SYSEXIT	6
#define SYSSHFT 7
#define SYSWAIT	8
#define SYSCONT 9
#define SYSBREAK 10
#define SYSEVAL 11
#define SYSDOT	12
#define SYSRDONLY 13
#define SYSTIMES 14
#define SYSXPORT 15
#define SYSNULL 16
#define SYSREAD 17
#define SYSTST	18
#define	SYSUMASK	19

/* used for input and output of shell */
#define INIO 10
#define OTIO 11

/*io nodes*/
#define USERIO	10
#define IOUFD	15
#define IODOC	16
#define IOPUT	32
#define IOAPP	64
#define IOMOV	128
#define IORDW	256
#define INPIPE	0
#define OTPIPE	1

/* arg list terminator */
#define ENDARGS	0

#include	"mac.h"
#include	"mode.h"
#include	"name.h"


/*
 * function prototypes
 */

/* args.c */
INT		options(INT argc, STRING *argv);
VOID		setargs(STRING argi[]);
DOLPTR		freeargs(DOLPTR blk);
LOCAL STRING *	copyargs(STRING from[], INT n);
VOID		clearup(VOID);
DOLPTR		useargs(VOID);

/* blok.c */
ADDRESS		alloc(POS nbytes);
VOID		addblok(POS reqd);
VOID		freemem(ADDRESS ap);

/* builtin.c */
INT builtin(INT argn, STRING *com);

/* cmd.c */
TREPTR		makefork(INT flgs, TREPTR i);
TREPTR		cmd(INT sym, INT flg);

/* ctyp.c */

/* error.c */
VOID		exitset(VOID);
VOID		sigchk(VOID);
VOID		failed(STRING s1, STRING s2);
VOID		error(STRING s);
VOID		exitsh(INT xno);
VOID		done(VOID);
VOID		rmtemp(IOPTR base);

/* expand.c */
INT		expand(STRING as, INT rflg);
INT		gmatch(STRING s, STRING p);
VOID		makearg(STRING args);

/* fault.c */
VOID		fault(INT sig);
VOID		stdsigs(VOID);
INT		ignsig(INT n);
VOID		getsig(INT n);
VOID		oldsigs(VOID);
VOID		clrsig(INT i);
VOID		chktrap(VOID);

/* io.c */
VOID		initf(UFD fd);
INT		estabf(STRING s);
VOID		push(FILE af);
INT		pop(VOID);
VOID		chkpipe(INT *pv);
INT		chkopen(STRING idf);
VOID		renam(INT f1, INT f2);
INT		create(STRING s);
INT		tmpfil(VOID);
VOID		copy(IOPTR ioparg);

/* macro.c */
STRING		macro(STRING as);
VOID		subst(INT in, INT ot);

/* main.c */
INT		main(INT c, STRING v[]);
VOID		chkpr(CHAR eor);
VOID		settmp(VOID);
VOID		Ldup(INT fa, INT fb);

/* msg.c */

/* name.c */
INT		syslook(STRING w, SYSTAB syswds);
VOID		setlist(ARGPTR arg, INT xp);
VOID		setname(STRING argi, INT xp);
VOID		replace(STRING *a, STRING v);
VOID		dfault(NAMPTR n, STRING v);
VOID		assign(NAMPTR n, STRING v);
INT		readvar(STRING *names);
VOID		assnum(STRING *p, INT i);
STRING		make(STRING v);
NAMPTR		lookup(STRING nam);
VOID		namscan(VOID (*fn)(NAMPTR n));
VOID		printnam(NAMPTR n);
VOID		exname(NAMPTR n);
VOID		printflg(NAMPTR n);
VOID		getenviron(VOID);
VOID		countnam(NAMPTR n);
VOID		pushnam(NAMPTR n);
STRING		*setenviron(VOID);

/* print.c */
VOID		newline(VOID);
VOID		blank(VOID);
VOID		prp(VOID);
VOID		prs(STRING as);
VOID		prc(CHAR c);
VOID		prt(L_INT t);
VOID		prn(INT n);
VOID		itos(INT n);
INT		stoi(STRING icp);

/* service.c */
VOID		initio(IOPTR iop);
STRING		getpath(STRING s);
INT		pathopen(STRING path, STRING name);
STRING		catpath(STRING path, STRING name);
VOID		execa(STRING at[]);
VOID		postclr(VOID);
VOID		post(INT pcsid);
VOID		await(INT i);
VOID		trim(STRING at);
STRING		mactrim(STRING s);
STRING		*scan(INT argn);
INT		getarg(COMPTR ac);

/* setbrk.c */
BYTPTR		setbrk(INT incr);

/* stak.c */
STKPTR		getstak(INT asize);
STKPTR		locstak(VOID);
STKPTR		savstak(VOID);
STKPTR		endstak(STRING argp);
VOID		tdystak(STKPTR x);
VOID		stakchk(VOID);
STKPTR		cpystak(STKPTR x);

/* string.c */
STRING		movstr(STRING a, STRING b);
INT		any(CHAR c, STRING s);
INT		cf(STRING s1, STRING s2);
INT		length(STRING as);

/* word.c */
INT		word(VOID);
CHAR		nextc(CHAR quote);
CHAR		readc(VOID);

/* xec.c */
INT		execute(TREPTR argt, INT execflg, INT *pf1, INT *pf2);
VOID		execexp(STRING s, UFD f);


#define attrib(n,f)	(n->namflg |= f)
#define round(a,b)	(((int)((ADR(a)+b)-1))&~((b)-1))
#define closepipe(x)	(close(x[INPIPE]), close(x[OTPIPE]))
#define eq(a,b)		(cf(a,b)==0)
#define max(a,b)	((a)>(b)?(a):(b))
#define assert(x)	;

/* temp files and io */
extern UFD	output;
extern INT	ioset;
extern IOPTR	iotemp;		/* files to be deleted sometime */
extern IOPTR	iopend;		/* documents waiting to be read at NL */

/* substitution */
extern INT	dolc;
extern STRING	*dolv;
extern DOLPTR	argfor;
extern ARGPTR	gchain;

/* stack */
#define		BLK(x)	((BLKPTR)(x))
#define		BYT(x)	((BYTPTR)(x))
#define		STK(x)	((STKPTR)(x))
#define		ADR(x)	((char*)(x))

/* stak stuff */
#include	"stak.h"

/* string constants */
extern MSG	atline;
extern MSG	readmsg;
extern MSG	colon;
extern MSG	minus;
extern MSG	nullstr;
extern MSG	sptbnl;
extern MSG	unexpected;
extern MSG	endoffile;
extern MSG	synmsg;

/* name tree and words */
extern SYSTAB	reserved;
extern INT	wdval;
extern INT	wdnum;
extern ARGPTR	wdarg;
extern INT	wdset;
extern BOOL	reserv;

/* prompting */
extern MSG	stdprompt;
extern MSG	supprompt;
extern MSG	profile;

/* built in names */
extern NAMNOD	fngnod;
extern NAMNOD	ifsnod;
extern NAMNOD	homenod;
extern NAMNOD	mailnod;
extern NAMNOD	pathnod;
extern NAMNOD	ps1nod;
extern NAMNOD	ps2nod;

/* special names */
extern MSG	flagadr;
extern STRING	cmdadr;
extern STRING	exitadr;
extern STRING	dolladr;
extern STRING	pcsadr;
extern STRING	pidadr;

extern MSG	defpath;

/* names always present */
extern MSG	mailname;
extern MSG	homename;
extern MSG	pathname;
extern MSG	fngname;
extern MSG	ifsname;
extern MSG	ps1name;
extern MSG	ps2name;

/* transput */
extern CHAR	tmpout[];
extern STRING	tmpname;
extern INT	serial;
#define		TMPNAM 7
extern FILE	standin;
#define input	(standin->fdes)
#define eof	(standin->feof)
extern INT	peekc;
extern STRING	comdiv;
extern MSG	devnull;

/* flags */
#define		noexec	01
#define		intflg	02
#define		prompt	04
#define		setflg	010
#define		errflg	020
#define		ttyflg	040
#define		forked	0100
#define		oneflg	0200
#define		rshflg	0400
#define		waiting	01000
#define		stdflg	02000
#define		execpr	04000
#define		readpr	010000
#define		keyflg	020000
extern INT	flags;

/* error exits from various parts of shell */
#include	<setjmp.h>
extern jmp_buf	subshell;
extern jmp_buf	errshell;

/* fault handling */
#include	"brkincr.h"
extern POS	brkincr;

#define MINTRAP	0
#define MAXTRAP	17

#define INTR	2
#define QUIT	3
#define MEMF	11
#define ALARM	14
#define KILL	15
#define TRAPSET	2
#define SIGSET	4
#define SIGMOD	8

extern BOOL	trapnote;
extern STRING	trapcom[];
extern BOOL	trapflg[];

/* name tree and words */
extern STRING	*environ;
extern CHAR	numbuf[];
extern MSG	export;
extern MSG	readonly;

/* execflgs */
extern INT	exitval;
extern BOOL	execbrk;
extern INT	loopcnt;
extern INT	breakcnt;

/* messages */
extern MSG	mailmsg;
extern MSG	coredump;
extern MSG	badopt;
extern MSG	badparam;
extern MSG	badsub;
extern MSG	nospace;
extern MSG	notfound;
extern MSG	badtrap;
extern MSG	baddir;
extern MSG	badshift;
extern MSG	illegal;
extern MSG	restricted;
extern MSG	execpmsg;
extern MSG	notid;
extern MSG	wtfailed;
extern MSG	badcreate;
extern MSG	piperr;
extern MSG	badopen;
extern MSG	badnum;
extern MSG	arglist;
extern MSG	txtbsy;
extern MSG	toobig;
extern MSG	badexec;
extern MSG	notfound;
extern MSG	badfile;

extern address	_ebss[];

#include	"ctyp.h"
