#ifndef __MV_H_INCLUDED__
#define __MV_H_INCLUDED__

/* defines / settings */
#define	DOT	"."
#define	DOTDOT	".."
#define	DELIM	'/'
#define SDELIM "/"
#define	MAXN	100
#define MODEBITS 07777
#define ROOTINO 2

/* prototypes */
int move(char*, char*);
int mvdir(char*, char*);
char* pname(char*);
char* dname(char*);
int check(char*, ino_t);
int chkdot(char*);

#endif
