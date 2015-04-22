/*
 * eos32lib.h -- EOS32 system library
 */


#ifndef _EOS32LIB_H_
#define _EOS32LIB_H_


#ifndef NULL
#define NULL	((void *) 0)
#endif


/*
 * global data
 */

extern char **environ;
extern void *curbrk;


/*
 * process management
 */

int execl(const char *name, const char *arg0, ...);
int execv(const char *name, char *const argv[]);
int execlp(const char *name, const char *arg0, ...);
int execvp(const char *name, char *const argv[]);
int execle(const char *name, const char *arg0, ...);


/*
 * signals
 */

void sleep(unsigned int seconds);


/*
 * file management
 */

#define TIOCGETD	(('t' << 8) | 0)
#define TIOCSETD	(('t' << 8) | 1)
#define TIOCHPCL	(('t' << 8) | 2)
#define TIOCMODG	(('t' << 8) | 3)
#define TIOCMODS	(('t' << 8) | 4)
#define TIOCGETP	(('t' << 8) | 8)
#define TIOCSETP	(('t' << 8) | 9)
#define TIOCSETN	(('t' << 8) | 10)
#define TIOCEXCL	(('t' << 8) | 13)
#define TIOCNXCL	(('t' << 8) | 14)
#define TIOHMODE	(('t' << 8) | 15)
#define TIOCTSTP	(('t' << 8) | 16)
#define TIOCSETC	(('t' << 8) | 17)
#define TIOCGETC	(('t' << 8) | 18)
#define FIOCLEX		(('f' << 8) | 1)
#define FIONCLEX	(('f' << 8) | 2)

struct sgttyb {
  char sg_ispeed;	/* input speed */
  char sg_ospeed;	/* output speed */
  char sg_erase;	/* erase character */
  char sg_kill;		/* kill character */
  int sg_flags;		/* mode flags */
};

struct tchars {
  char t_intrc;		/* interrupt */
  char t_quitc;		/* quit */
  char t_startc;	/* start output */
  char t_stopc;		/* stop output */
  char t_eofc;		/* end-of-file */
  char t_brkc;		/* input delimiter (like nl) */
};

#define TANDEM		01
#define CBREAK		02
#define LCASE		04
#define ECHO		010
#define CRMOD		020
#define RAW		040
#define ODDP		0100
#define EVENP		0200
#define ANYP		0300
#define NLDELAY		001400
#define TBDELAY		006000
#define XTABS		06000
#define CRDELAY		030000
#define VTDELAY		040000
#define BSDELAY		0100000
#define ALLDELAY	0177400

#define CR0		0
#define CR1		010000
#define CR2		020000
#define CR3		030000
#define NL0		0
#define NL1		000400
#define NL2		001000
#define NL3		001400
#define TAB0		0
#define TAB1		002000
#define TAB2		004000
#define FF0		0
#define FF1		040000
#define BS0		0
#define BS1		0100000

#define B0		0
#define B50		1
#define B75		2
#define B110		3
#define B134		4
#define B150		5
#define B200		6
#define B300		7
#define B600		8
#define B1200		9
#define B1800		10
#define B2400		11
#define B4800		12
#define B9600		13
#define EXTA		14
#define EXTB		15

int stty(int fildes, struct sgttyb *argp);
int gtty(int fildes, struct sgttyb *argp);
int isatty(int fildes);
char *ttyname(int fildes);
int ttyslot(void);

char *mktemp(char *as);


/*
 * directory and file system management
 */


/*
 * protection
 */

struct utmp {
  char ut_line[8];		/* tty name */
  char ut_name[8];		/* user id */
  long ut_time;			/* time on */
};

char *getlogin(void);
char *getpass(char *prompt);

struct passwd {
  char *pw_name;
  char *pw_passwd;
  int pw_uid;
  int pw_gid;
  int pw_quota;
  char *pw_comment;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};

void setpwent(void);
void endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwnam(char *name);
struct passwd *getpwuid(int uid);

struct group {
  char *gr_name;
  char *gr_passwd;
  int gr_gid;
  char **gr_mem;
};

void setgrent(void);
void endgrent(void);
struct group *getgrent(void);
struct group *getgrnam(char *name);
struct group *getgrgid(int gid);


/*
 * encryption
 */

void setkey(char *key);
void encrypt(char *block, int edflag);
char *crypt(char *pw, char *salt);


/*
 * time management
 */

char *timezone(int zone, int dst);


#endif /* _EOS32LIB_H_ */
