/*
 * eos32lib.c -- EOS32 system library
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <eos32sys.h>
#include <eos32lib.h>


/**************************************************************/

/*
 * global data
 */


char **environ;		/* environment, initialized by startup code */
void *curbrk;		/* current break, initialized by startup code */


/**************************************************************/

/*
 * process management
 */


int execl(const char *name, const char *arg0, ...) {
  return execv(name, (char *const *) &arg0);
}


int execv(const char *name, char *const argv[]) {
  return execve(name, argv, environ);
}


int execlp(const char *name, const char *arg0, ...) {
  return execvp(name, (char *const *) &arg0);
}


static char *execat(char *s1, const char *s2, char *si) {
  char *s;

  s = si;
  while (*s1 != '\0' && *s1 != ':' && *s1 != '-') {
    *s++ = *s1++;
  }
  if (si != s) {
    *s++ = '/';
  }
  while (*s2 != '\0') {
    *s++ = *s2++;
  }
  *s = '\0';
  return *s1 != '\0' ? ++s1 : NULL;
}


int execvp(const char *name, char *const argv[]) {
  char *pathstr;
  char *cp;
  char fname[128];
  char *newargs[256];
  int i;
  int etxtbsy = 1;
  int eacces = 0;

  pathstr = getenv("PATH");
  if (pathstr == NULL) {
    pathstr = ":/bin:/usr/bin";
  }
  cp = (strchr(name, '/') != NULL ? "" : pathstr);
  do {
    cp = execat(cp, name, fname);
retry:
    execv(fname, argv);
    switch (errno) {
      case ENOEXEC:
        newargs[0] = "sh";
        newargs[1] = fname;
        for (i = 1; (newargs[i + 1] = argv[i]) != NULL; i++) {
          if (i >= 254) {
            errno = E2BIG;
            return -1;
          }
        }
        execv("/bin/sh", newargs);
        return -1;
      case ETXTBSY:
        if (++etxtbsy > 5) {
          return -1;
        }
        sleep(etxtbsy);
        goto retry;
      case EACCES:
        eacces++;
        break;
      case ENOMEM:
      case E2BIG:
        return -1;
    }
  } while (cp != NULL);
  if (eacces) {
    errno = EACCES;
  }
  return -1;
}


int execle(const char *name, const char *arg0, ...) {
  const char **p;
  char ***q;

  p = &arg0;
  while (*p++ != NULL) ;
  q = (char ***) p;
  return execve(name, (char *const *) &arg0, *q);
}


/**************************************************************/

/*
 * signals
 */


static jmp_buf sleepjmp;


static void sleepx(int sig) {
  longjmp(sleepjmp, 1);
}


void sleep(unsigned int seconds) {
  unsigned int altime;
  void (*alsig)(int) = SIG_DFL;

  if (seconds == 0) {
    return;
  }
  altime = alarm(1000);  /* time to maneuver */
  if (setjmp(sleepjmp)) {
    signal(SIGALRM, alsig);
    alarm(altime);
    return;
  }
  if (altime != 0) {
    if (altime > seconds) {
      altime -= seconds;
    } else {
      seconds = altime;
      altime = 1;
    }
  }
  alsig = signal(SIGALRM, sleepx);
  alarm(seconds);
  while (1) {
    pause();
  }
}


/**************************************************************/

/*
 * file management
 */


int stty(int fildes, struct sgttyb *argp) {
  return ioctl(fildes, TIOCSETP, argp);
}


int gtty(int fildes, struct sgttyb *argp) {
  return ioctl(fildes, TIOCGETP, argp);
}


int isatty(int fildes) {
  struct sgttyb ttyb;

  if (gtty(fildes, &ttyb) < 0) {
    return 0;
  }
  return 1;
}


/*
 * ttyname(fd): return "/dev/ttyXX" which is the name
 * of the tty belonging to file descriptor fd, or NULL
 * if it is not a tty
 */

char *ttyname(int fildes) {
  static char dev[] = "/dev/";
  struct stat fsb;
  struct stat tsb;
  struct direct db;
  static char rbuf[32];
  int df;

  if (isatty(fildes) == 0) {
    return NULL;
  }
  if (fstat(fildes, &fsb) < 0) {
    return NULL;
  }
  if ((fsb.st_mode&S_IFMT) != S_IFCHR) {
    return NULL;
  }
  if ((df = open(dev, 0)) < 0) {
    return NULL;
  }
  while (read(df, (char *)&db, sizeof(db)) == sizeof(db)) {
    if (db.d_ino == 0) {
      continue;
    }
    if (db.d_ino != fsb.st_ino) {
      continue;
    }
    strcpy(rbuf, dev);
    strcat(rbuf, db.d_name);
    if (stat(rbuf, &tsb) < 0) {
      continue;
    }
    if (tsb.st_dev == fsb.st_dev && tsb.st_ino == fsb.st_ino) {
      close(df);
      return rbuf;
    }
  }
  close(df);
  return NULL;
}


/*
 * Return the number of the slot in the utmp file
 * corresponding to the current user: try for file 0, 1, 2.
 * Definition is the line number in the /etc/ttys file.
 */

static char *getttys(int f) {
  static char line[32];
  char *lp;

  lp = line;
  for (;;) {
    if (read(f, lp, 1) != 1) {
      return NULL;
    }
    if (*lp =='\n') {
      *lp = '\0';
      return line + 2;
    }
    if (lp >= &line[32]) {
      return line + 2;
    }
    lp++;
  }
}

int ttyslot(void) {
  static char ttys[] = "/etc/ttys";
  char *tp, *p;
  int s, tf;

  if ((tp = ttyname(0)) == NULL &&
      (tp = ttyname(1)) == NULL &&
      (tp = ttyname(2)) == NULL) {
    return 0;
  }
  if ((p = strrchr(tp, '/')) == NULL) {
    p = tp;
  } else {
    p++;
  }
  if ((tf = open(ttys, 0)) < 0) {
    return 0;
  }
  s = 0;
  while (tp = getttys(tf)) {
    s++;
    if (strcmp(p, tp) == 0) {
      close(tf);
      return s;
    }
  }
  close(tf);
  return 0;
}


/*
 * Make a unique temporary filename.
 */

char *mktemp(char *as) {
  unsigned pid;
  char *s;
  int i;

  pid = getpid();
  s = as;
  while (*s++) ;
  s--;
  while (*--s == 'X') {
    *s = (pid % 10) + '0';
    pid /= 10;
  }
  s++;
  i = 'a';
  while (access(as, 0) != -1) {
    if (i == 'z') {
      return "/";
    }
    *s = i++;
  }
  return as;
}


/**************************************************************/

/*
 * directory and file system management
 */


/**************************************************************/

/*
 * protection
 */


char *getlogin(void) {
  static char UTMP[] = "/etc/utmp";
  static struct utmp ubuf;
  int me, uf;
  char *cp;

  if (!(me = ttyslot())) {
    return 0;
  }
  if ((uf = open( UTMP, 0 )) < 0) {
    return 0;
  }
  lseek(uf, (long) (me * sizeof(ubuf)), 0);
  if (read(uf, (char *) &ubuf, sizeof(ubuf)) != sizeof(ubuf)) {
    return 0;
  }
  close(uf);
  ubuf.ut_name[8] = ' ';
  for (cp = ubuf.ut_name; *cp++ != ' '; ) ;
  *--cp = '\0';
  return ubuf.ut_name;
}


char *getpass(char *prompt) {
  struct sgttyb ttyb;
  int flags;
  char *p;
  int c;
  FILE *fi;
  static char pbuf[9];
  void (*sig)(int signo);

  if ((fi = fopen("/dev/tty", "r")) == NULL) {
    fi = stdin;
  } else {
    setbuf(fi, (char *)NULL);
  }
  sig = signal(SIGINT, SIG_IGN);
  gtty(fileno(fi), &ttyb);
  flags = ttyb.sg_flags;
  ttyb.sg_flags &= ~ECHO;
  stty(fileno(fi), &ttyb);
  fprintf(stderr, prompt);
  for (p = pbuf; (c = getc(fi)) != '\n' && c != EOF; ) {
    if (p < &pbuf[8]) {
      *p++ = c;
    }
  }
  *p = '\0';
  fprintf(stderr, "\n");
  ttyb.sg_flags = flags;
  stty(fileno(fi), &ttyb);
  signal(SIGINT, sig);
  if (fi != stdin) {
    fclose(fi);
  }
  return pbuf;
}


static char PASSWD[] = "/etc/passwd";
static char EMPTY[] = "";
static FILE *pwf = NULL;
static char pwline[BUFSIZ+1];
static struct passwd passwd;


void setpwent(void) {
  if (pwf == NULL) {
    pwf = fopen(PASSWD, "r");
  } else {
    rewind(pwf);
  }
}


void endpwent(void) {
  if (pwf != NULL) {
    fclose(pwf);
    pwf = NULL;
  }
}


static char *pwskip(char *p) {
  while ( *p && *p != ':' ) {
    ++p;
  }
  if (*p) {
    *p++ = 0;
  }
  return p;
}


struct passwd *getpwent(void) {
  char *p;

  if (pwf == NULL) {
    if ( (pwf = fopen( PASSWD, "r" )) == NULL ) {
      return 0;
    }
  }
  p = fgets(pwline, BUFSIZ, pwf);
  if (p == NULL) {
    return 0;
  }
  passwd.pw_name = p;
  p = pwskip(p);
  passwd.pw_passwd = p;
  p = pwskip(p);
  passwd.pw_uid = atoi(p);
  p = pwskip(p);
  passwd.pw_gid = atoi(p);
  passwd.pw_quota = 0;
  passwd.pw_comment = EMPTY;
  p = pwskip(p);
  passwd.pw_gecos = p;
  p = pwskip(p);
  passwd.pw_dir = p;
  p = pwskip(p);
  passwd.pw_shell = p;
  while (*p && *p != '\n') {
    p++;
  }
  *p = '\0';
  return &passwd;
}


struct passwd *getpwnam(char *name) {
  struct passwd *p;

  setpwent();
  while ( (p = getpwent()) && strcmp(name,p->pw_name) ) ;
  endpwent();
  return p;
}


struct passwd *getpwuid(int uid) {
  struct passwd *p;

  setpwent();
  while ( (p = getpwent()) && p->pw_uid != uid ) ;
  endpwent();
  return p;
}


#define CL	':'
#define CM	','
#define NL	'\n'
#define MAXGRP	100


static char GROUP[] = "/etc/group";
static FILE *grf = NULL;
static char grline[BUFSIZ+1];
static struct group group;
static char *gr_mem[MAXGRP];


void setgrent(void) {
  if (grf == NULL) {
    grf = fopen(GROUP, "r");
  } else {
    rewind(grf);
  }
}


void endgrent(void) {
  if (grf != NULL) {
    fclose(grf);
    grf = NULL;
  }
}


static char *grskip(char *p, int c) {
  while ( *p && *p != c ) {
    ++p;
  }
  if (*p) {
    *p++ = 0;
  }
  return p;
}


struct group *getgrent(void) {
  char *p, **q;

  if ( !grf && !(grf = fopen( GROUP, "r" )) ) {
    return NULL;
  }
  if ( !(p = fgets( grline, BUFSIZ, grf )) ) {
    return NULL;
  }
  group.gr_name = p;
  group.gr_passwd = p = grskip(p, CL);
  group.gr_gid = atoi( p = grskip(p, CL) );
  group.gr_mem = gr_mem;
  p = grskip(p, CL);
  grskip(p, NL);
  q = gr_mem;
  while (*p) {
    *q++ = p;
    p = grskip(p, CM);
  }
  *q = NULL;
  return &group;
}


struct group *getgrnam(char *name) {
  struct group *p;

  setgrent();
  while ( (p = getgrent()) && strcmp(p->gr_name, name) ) ;
  endgrent();
  return p;
}


struct group *getgrgid(int gid) {
  struct group *p;

  setgrent();
  while ( (p = getgrent()) && p->gr_gid != gid ) ;
  endgrent();
  return p;
}


/**************************************************************/

/*
 * encryption
 */


/*
 * This program implements the
 * Proposed Federal Information Processing
 * Data Encryption Standard.
 * See Federal Register, March 17, 1975 (40FR12134)
 */

/*
 * Initial permutation,
 */
static char IP[] = {
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7,
};

/*
 * Final permutation, FP = IP^(-1)
 */
static char FP[] = {
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25,
};

/*
 * Permuted-choice 1 from the key bits
 * to yield C and D.
 * Note that bits 8,16... are left out:
 * They are intended for a parity check.
 */
static char PC1_C[] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
};

static char PC1_D[] = {
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4,
};

/*
 * Sequence of shifts used for the key schedule.
*/
static char shifts[] = {
	1, 1, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 1,
};

/*
 * Permuted-choice 2, to pick out the bits from
 * the CD array that generate the key schedule.
 */
static char PC2_C[] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
};

static char PC2_D[] = {
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32,
};

/*
 * The C and D arrays used to calculate the key schedule.
 */
static char C[28];
static char D[28];

/*
 * The key schedule.
 * Generated from the key.
 */
static char KS[16][48];

/*
 * Set up the key schedule from the key.
 */
void setkey(char *key) {
	int i, j, k;
	int t;

	/*
	 * First, generate C and D by permuting
	 * the key.  The low order bit of each
	 * 8-bit char is not used, so C and D are only 28
	 * bits apiece.
	 */
	for (i=0; i<28; i++) {
		C[i] = key[PC1_C[i]-1];
		D[i] = key[PC1_D[i]-1];
	}
	/*
	 * To generate Ki, rotate C and D according
	 * to schedule and pick up a permutation
	 * using PC2.
	 */
	for (i=0; i<16; i++) {
		/*
		 * rotate.
		 */
		for (k=0; k<shifts[i]; k++) {
			t = C[0];
			for (j=0; j<28-1; j++)
				C[j] = C[j+1];
			C[27] = t;
			t = D[0];
			for (j=0; j<28-1; j++)
				D[j] = D[j+1];
			D[27] = t;
		}
		/*
		 * get Ki. Note C and D are concatenated.
		 */
		for (j=0; j<24; j++) {
			KS[i][j] = C[PC2_C[j]-1];
			KS[i][j+24] = D[PC2_D[j]-28-1];
		}
	}
}

/*
 * The E bit-selection table.
 */
static char E[48];
static char e[] = {
	32,  1,  2,  3,  4,  5,
	 4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1,
};

/*
 * The 8 selection functions.
 * For some reason, they give a 0-origin
 * index, unlike everything else.
 */
static char S[8][64] = {
	{
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
	},

	{
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
	},

	{
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
	},

	{
	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
	},

	{
	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
	},

	{
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	},

	{
	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
	},

	{
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11,
	},
};

/*
 * P is a permutation on the selected combination
 * of the current L and key.
 */
static char P[] = {
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25,
};

/*
 * The current block, divided into 2 halves.
 */
static char L[32], R[32];
static char tempL[32];
static char f[32];

/*
 * The combination of the key and the input, before selection.
 */
static char preS[48];

/*
 * The payoff: encrypt a block.
 */
void encrypt(char *block, int edflag) {
	int i, ii;
	int t, j, k;

	/*
	 * First, permute the bits in the input
	 */
	for (j=0; j<64; j++) {
		if (j < 32) {
			L[j] = block[IP[j]-1];
		} else {
			R[j-32] = block[IP[j]-1];
		}
	}
	/*
	 * Perform an encryption operation 16 times.
	 */
	for (ii=0; ii<16; ii++) {
		/*
		 * Set direction
		 */
		if (edflag)
			i = 15-ii;
		else
			i = ii;
		/*
		 * Save the R array,
		 * which will be the new L.
		 */
		for (j=0; j<32; j++)
			tempL[j] = R[j];
		/*
		 * Expand R to 48 bits using the E selector;
		 * exclusive-or with the current key bits.
		 */
		for (j=0; j<48; j++)
			preS[j] = R[E[j]-1] ^ KS[i][j];
		/*
		 * The pre-select bits are now considered
		 * in 8 groups of 6 bits each.
		 * The 8 selection functions map these
		 * 6-bit quantities into 4-bit quantities
		 * and the results permuted
		 * to make an f(R, K).
		 * The indexing into the selection functions
		 * is peculiar; it could be simplified by
		 * rewriting the tables.
		 */
		for (j=0; j<8; j++) {
			t = 6*j;
			k = S[j][(preS[t+0]<<5)+
				(preS[t+1]<<3)+
				(preS[t+2]<<2)+
				(preS[t+3]<<1)+
				(preS[t+4]<<0)+
				(preS[t+5]<<4)];
			t = 4*j;
			f[t+0] = (k>>3)&01;
			f[t+1] = (k>>2)&01;
			f[t+2] = (k>>1)&01;
			f[t+3] = (k>>0)&01;
		}
		/*
		 * The new R is L ^ f(R, K).
		 * The f here has to be permuted first, though.
		 */
		for (j=0; j<32; j++)
			R[j] = L[j] ^ f[P[j]-1];
		/*
		 * Finally, the new L (the original R)
		 * is copied back.
		 */
		for (j=0; j<32; j++)
			L[j] = tempL[j];
	}
	/*
	 * The output L and R are reversed.
	 */
	for (j=0; j<32; j++) {
		t = L[j];
		L[j] = R[j];
		R[j] = t;
	}
	/*
	 * The final output
	 * gets the inverse permutation of the very original.
	 */
	for (j=0; j<64; j++) {
		k = FP[j] - 1;
		if (k < 32) {
			block[j] = L[k];
		} else {
			block[j] = R[k-32];
		}
	}
}

char *crypt(char *pw, char *salt) {
	int i, j, c;
	int temp;
	static char block[66], iobuf[16];
	for(i=0; i<66; i++)
		block[i] = 0;
	for(i=0; (c= *pw) && i<64; pw++){
		for(j=0; j<7; j++, i++)
			block[i] = (c>>(6-j)) & 01;
		i++;
	}

	setkey(block);

	for(i=0; i<66; i++)
		block[i] = 0;

	for(i=0;i<48;i++)
		E[i] = e[i];

	for(i=0;i<2;i++){
		c = *salt++;
		iobuf[i] = c;
		if(c>'Z') c -= 6;
		if(c>'9') c -= 7;
		c -= '.';
		for(j=0;j<6;j++){
			if((c>>j) & 01){
				temp = E[6*i+j];
				E[6*i+j] = E[6*i+j+24];
				E[6*i+j+24] = temp;
				}
			}
		}

	for(i=0; i<25; i++)
		encrypt(block,0);

	for(i=0; i<11; i++){
		c = 0;
		for(j=0; j<6; j++){
			c <<= 1;
			c |= block[6*i+j];
			}
		c += '.';
		if(c>'9') c += 7;
		if(c>'Z') c += 6;
		iobuf[i+2] = c;
	}
	iobuf[i+2] = 0;
	if(iobuf[1]==0)
		iobuf[1] = iobuf[0];
	return(iobuf);
}


/**************************************************************/

/*
 * time management
 */


/*
 * The arguments are the number of minutes of time
 * you are westward from Greenwich and whether DST
 * is in effect. It returns a string giving the name
 * of the local timezone.
 *
 * Sorry, I don't know all the names.
 */

static struct zone {
	int	offset;
	char	*stdzone;
	char	*dlzone;
} zonetab[] = {
	4*60, "AST", "ADT",		/* Atlantic */
	5*60, "EST", "EDT",		/* Eastern */
	6*60, "CST", "CDT",		/* Central */
	7*60, "MST", "MDT",		/* Mountain */
	8*60, "PST", "PDT",		/* Pacific */
	0, "GMT", 0,			/* Greenwich */
	-1
};

char *timezone(int zone, int dst) {
	struct zone *zp;
	static char czone[10];
	char *sign;

	for (zp=zonetab; zp->offset!=-1; zp++)
		if (zp->offset==zone) {
			if (dst && zp->dlzone)
				return(zp->dlzone);
			if (!dst && zp->stdzone)
				return(zp->stdzone);
		}
	if (zone<0) {
		zone = -zone;
		sign = "+";
	} else
		sign = "-";
	sprintf(czone, "GMT%s%d:%02d", sign, zone/60, zone%60);
	return(czone);
}
