/*
 * stdlib.c -- utility functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <eos32.h>
#include <assert.h>


/**************************************************************/

/*
 * number conversion
 */


/*
 * Convert a string to a double.
 */

double atof(const char *s) {
  return strtod(s, NULL);
}


/*
 * Convert a string to an integer.
 */

int atoi(const char *s) {
  return (int) strtol(s, NULL, 10);
}


/*
 * Convert a string to a long.
 */

long atol(const char *s) {
  return strtol(s, NULL, 10);
}


/*
 * Convert a string to a double
 * implemented by swlt16
 */

double strtod (const char* str, char** endptr) {
    assert(0 == 1); /* no floating points yet :(  */
    double result1 = 0.0; /* left site of dot */
    double result2 = 0.0; /* right site of dot */
    double* cur;
    char sign = +1;
    int rsc = 0; /* right site counter */
    int quot = 1; /* quotient */

    cur = &result1;
    if( ! str) goto output;
    while(*str) {
        switch(*str) {
            case ' ':  case '\n': case '\t': 
            case '\v': case '\f': case '\r':
                if(result1 != 0.0 || result2 != 0.0) goto output; 
                /* just ignore whitespaces */
                break;
            case '+':
                if(result1 != 0.0 || result2 != 0.0) goto output;
                sign = +1;
                break;
            case '-':
                if(result1 != 0.0 || result2 != 0.0) goto output; 
                sign = -1;
                break;
            case '.':
                cur = &result2;
                rsc = 0;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                *cur *= 10;
                *cur += (*str) - '0';
                rsc++;
                break;
            default:
                goto output;
        }
        *str++;
    }
    output:
    if(endptr) *endptr = (char*) str;
    /* move result2 to the right site of the dot (mathematically, if any) */
    while(rsc--) quot *= 10;
    if(result2 != 0.0) result2 /= quot;
    
    return sign * (result1 + result2);
}

/*
 * Convert a string to a long, using a given base.
 */

long strtol(const char *s, char **endp, int base) {
  int c;
  int negative;
  int overflow;
  long limit1;
  long limit2;
  long value;

  /* skip white space */
  do {
    c = *s++;
  } while (isspace(c));
  /* handle sign */
  negative = 0;
  if (c == '+') {
    c = *s++;
  } else
  if (c == '-') {
    negative = 1;
    c = *s++;
  }
  /* determine base */
  if (base == 0) {
    /* must figure it out from input string */
    if (c == '0') {
      if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
        c = *s++;
      } else {
        base = 8;
        c = *s++;
      }
    } else {
      base = 10;
    }
  }
  /* iterate through digits */
  overflow = 0;
  value = negative ? -LONG_MIN : LONG_MAX;
  limit1 = value / base;
  limit2 = value % base;
  value = 0;
  while (1) {
    if (isdigit(c)) {
      c = c - '0';
    } else
    if (isalpha(c)) {
      c = tolower(c) - 'a' + 10;
    } else {
      break;
    }
    if (c >= base) {
      break;
    }
    if (!overflow) {
      if (value > limit1 || (value == limit1 && c > limit2)) {
        overflow = 1;
      } else {
        value *= base;
        value += c;
      }
    }
    c = *s++;
  }
  /* store address of suffix */
  if (endp != NULL) {
    *endp = (char *) --s;
  }
  /* return value */
  if (overflow) {
    return negative ? LONG_MIN : LONG_MAX;
  }
  return negative ? -value : value;
}


/*
 * Convert a string to an unsigned long, using a given base.
 */

unsigned long strtoul(const char *s, char **endp, int base) {
  int c;
  int negative;
  int overflow;
  unsigned long limit1;
  unsigned long limit2;
  unsigned long value;

  /* skip white space */
  do {
    c = *s++;
  } while (isspace(c));
  /* handle sign */
  negative = 0;
  if (c == '+') {
    c = *s++;
  } else
  if (c == '-') {
    negative = 1;
    c = *s++;
  }
  /* determine base */
  if (base == 0) {
    /* must figure it out from input string */
    if (c == '0') {
      if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
        c = *s++;
      } else {
        base = 8;
        c = *s++;
      }
    } else {
      base = 10;
    }
  }
  /* iterate through digits */
  overflow = 0;
  value = ULONG_MAX;
  limit1 = value / base;
  limit2 = value % base;
  value = 0;
  while (1) {
    if (isdigit(c)) {
      c = c - '0';
    } else
    if (isalpha(c)) {
      c = tolower(c) - 'a' + 10;
    } else {
      break;
    }
    if (c >= base) {
      break;
    }
    if (!overflow) {
      if (value > limit1 || (value == limit1 && c > limit2)) {
        overflow = 1;
      } else {
        value *= base;
        value += c;
      }
    }
    c = *s++;
  }
  /* store address of suffix */
  if (endp != NULL) {
    *endp = (char *) --s;
  }
  /* return value */
  if (overflow) {
    return ULONG_MAX;
  }
  return negative ? -value : value;
}


/**************************************************************/

/*
 * pseudo random number generation
 */


static unsigned long randomNumber = 1;


/*
 * Compute the next random number in the range 0..RAND_MAX.
 */

int rand(void) {
  randomNumber = randomNumber * 1103515245 + 12345;
  return (unsigned int)(randomNumber >> 16) & RAND_MAX;
}


/*
 * Set the seed of the random number generator.
 */

void srand(unsigned int seed) {
  randomNumber = seed;
}


/**************************************************************/

/*
 * storage allocation
 */


/*
 * Minimum number of units to request from the system.
 * A unit is sizeof(Header) bytes large.
 */

#define MIN_ALLOC	1024


/*
 * Type definition for alignment to boundary of most
 * restrictive type on this architecture.
 */

typedef long Align;


/*
 * Memory block header definition.
 */

typedef union header {
  struct {
    union header *next;		/* next block if on free list */
    unsigned int size;		/* size of this block in units */
  } s;
  Align x;			/* force alignment of blocks */
} Header;


/*
 * Variables for heap management.
 */

static Header base;		/* empty list to get started */
static Header *freep = NULL;	/* start of free list */


/*
 * Ask the system for more memory.
 */

static Header *moreCore(unsigned int nu) {
  char *cp;
  Header *up;

  if (nu < MIN_ALLOC) {
    nu = MIN_ALLOC;
  }
  cp = sbrk(nu * sizeof(Header));
  if (cp == (char *) (unsigned) -1) {
    /* no space at all */
    return NULL;
  }
  up = (Header *) cp;
  up->s.size = nu;
  free((void *) (up + 1));
  return freep;
}


/*
 * Allocate memory.
 */

void *malloc(size_t size) {
  unsigned int nunits;
  Header *prevp;
  Header *p;

  nunits = (size + sizeof(Header) - 1) / sizeof(Header) + 1;
  if (freep == NULL) {
    /* no free list yet */
    base.s.next = &base;
    base.s.size = 0;
    freep = &base;
  }
  prevp = freep;
  p = prevp->s.next;
  while (1) {
    if (p->s.size >= nunits) {
      /* big enough */
      if (p->s.size == nunits) {
        /* exact fit */
        prevp->s.next = p->s.next;
      } else {
        /* allocate tail end */
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *) (p + 1);
    }
    if (p == freep) {
      /* wrapped around free list */
      p = moreCore(nunits);
      if (p == NULL) {
        /* no memory left */
        return NULL;
      }
    }
    /* advance pointers */
    prevp = p;
    p = p->s.next;
  }
  /* never reached */
  return NULL;
}


/*
 * Allocate memory for 'nobj' elements of size 'size' each.
 * Initialize the memory to zero. Return a pointer to the
 * allocated memory, or NULL if the request fails.
 */

void *calloc(size_t nobj, size_t size) {
  size_t nbytes;
  void *p;

  nbytes = nobj * size;
  p = malloc(nbytes);
  if (p == NULL) {
    return NULL;
  }
  return memset(p, 0, nbytes);
}


/*
 * Change the size of the object pointed to by 'p' to 'size'.
 * The contents will be unchanged up to the minimum of the old
 * and new sizes. If the new size is larger, the new space is
 * uninitialized. Return a pointer to the new space, or NULL
 * if the request fails, in which case *p is unchanged.
 */

void *realloc(void *p, size_t size) {
  Header *hp;
  size_t oldSize;
  void *q;

  /* !!!!! (correct, but inefficient) */
  /* let hp point to block header */
  hp = (Header *) p - 1;
  /* get old size */
  oldSize = hp->s.size * sizeof(Header);
  /* allocate new object */
  q = malloc(size);
  if (q == NULL) {
    return NULL;
  }
  /* copy contents */
  memcpy(q, p, oldSize < size ? oldSize : size);
  /* free old object */
  free(p);
  /* return new object */
  return q;
}


/*
 * Free memory.
 */

void free(void *p) {
  Header *hp;
  Header *q;

  /* if p is NULL there is nothing to be freed */
  if (p == NULL) {
    return;
  }
  /* let hp point to block header */
  hp = (Header *) p - 1;
  /* find right place in free list */
  for (q = freep; !(hp > q && hp < q->s.next); q = q->s.next) {
    if (q >= q->s.next && (hp > q || hp < q->s.next)) {
      /* freed block at start or end of arena */
      break;
    }
  }
  /* check for joining neighbours */
  if (hp + hp->s.size == q->s.next) {
    /* join to upper neighbour */
    hp->s.size += q->s.next->s.size;
    hp->s.next = q->s.next->s.next;
  } else {
    hp->s.next = q->s.next;
  }
  if (q + q->s.size == hp) {
    /* join to lower neighbour */
    q->s.size += hp->s.size;
    q->s.next = hp->s.next;
  } else {
    q->s.next = hp;
  }
  freep = q;
}


/**************************************************************/

/*
 * communication with the environment
 */


void abort(void) {
  raise(SIGABRT);
  exit(EXIT_FAILURE);
}


#define ATEXIT_MAX	32

static void (*atexitFuncs[ATEXIT_MAX])(void);
static int numFuncs = 0;


void exit(int status) {
  FILE *stream;

  while (numFuncs--) {
    (*atexitFuncs[numFuncs])();
  }
  for (stream = _files; stream < _files + FOPEN_MAX; stream++) {
    fclose(stream);
  }
  _exit(status);
}


int atexit(void (*fcn)(void)) {
  if (numFuncs == ATEXIT_MAX) {
    return -1;
  }
  atexitFuncs[numFuncs++] = fcn;
  return 0;
}


int system(const char *s) {
  int pid;
  void (*istat)(int);
  void (*qstat)(int);
  int w, status;

  if ((pid = fork()) == 0) {
    /* child */
    execl("/bin/sh", "sh", "-c", s, NULL);
    _exit(127);
  }
  /* parent */
  istat = signal(SIGINT, SIG_IGN);
  qstat = signal(SIGQUIT, SIG_IGN);
  while ((w = wait(&status)) != pid && w != -1) ;
  if (w == -1) {
    status = -1;
  }
  signal(SIGINT, istat);
  signal(SIGQUIT, qstat);
  return status;
}


static char *envmatch(const char *s1, char *s2) {
  /* s1 is either name, or name=value */
  /* s2 is name=value */
  while (*s1 == *s2++) {
    if (*s1++ == '=') {
      return s2;
    }
  }
  if (*s1 == '\0' && *(s2 - 1) == '=') {
    return s2;
  }
  return NULL;
}


char *getenv(const char *name) {
  char **p;
  char *v;

  p = environ;
  while (*p != NULL) {
    v = envmatch(name, *p++);
    if (v != NULL) {
      return v;
    }
  }
  return NULL;
}


/**************************************************************/

/*
 * searching and sorting
 */


/*
 * Perform a binary search in an array of items.
 */

void *bsearch(const void *key, const void *base, size_t n, size_t size,
              int (*cmp)(const void *keyval, const void *datum)) {
  char *lo, *hi, *tst;
  int res;

  lo = (char *) base;
  hi = (char *) base + (n - 1) * size;
  while (lo <= hi) {
    tst = lo + (((hi - lo) / size) / 2) * size;
    res = cmp(tst, key);
    if (res == 0) {
      return tst;
    }
    if (res < 0) {
      lo = tst + size;
    } else {
      hi = tst - size;
    }
  }
  return NULL;
}


/*
 * Exchange two array items of a given size.
 */

static void xchg(char *p, char *q, int size) {
  char t;

  while (size--) {
    t = *p;
    *p++ = *q;
    *q++ = t;
  }
}


/*
 * This is a recursive version of quicksort.
 */

static void sort(char *l, char *r, int size,
                 int (*cmp)(const void *, const void *)) {
  char *i;
  char *j;
  char *x;

  i = l;
  j = r;
  x = l + (((r - l) / size) / 2) * size;
  do {
    while (cmp(i, x) < 0) {
      i += size;
    }
    while (cmp(x, j) < 0) {
      j -= size;
    }
    if (i <= j) {
      /* exchange array elements i and j */
      /* attention: update x if it is one of these */
      if (x == i) {
        x = j;
      } else
      if (x == j) {
        x = i;
      }
      xchg(i, j, size);
      i += size;
      j -= size;
    }
  } while (i <= j);
  if (l < j) {
    sort(l, j, size, cmp);
  }
  if (i < r) {
    sort(i, r, size, cmp);
  }
}


/*
 * External interface for the quicksort algorithm.
 */

void qsort(void *base, size_t n, size_t size,
           int (*cmp)(const void *, const void *)) {
  if (n < 2) {
    return;
  }
  sort((char *) base, (char *) base + (n - 1) * size, size, cmp);
}


/**************************************************************/

/*
 * integer arithmetic
 */


int abs(int n) {
  if (n < 0) {
    return -n;
  }
  return n;
}


long labs(long n) {
  if (n < 0) {
    return -n;
  }
  return n;
}


div_t div(int num, int denom) {
  div_t rv;

  if (num < 0) {
    if (denom < 0) {
      rv.quot = -num / -denom;
    } else {
      rv.quot = -num / denom;
      rv.quot = -rv.quot;
    }
  } else {
    if (denom < 0) {
      rv.quot = num / -denom;
      rv.quot = -rv.quot;
    } else {
      rv.quot = num / denom;
    }
  }
  rv.rem = num - rv.quot * denom;
  return rv;
}


ldiv_t ldiv(long num, long denom) {
  ldiv_t rv;

  if (num < 0) {
    if (denom < 0) {
      rv.quot = -num / -denom;
    } else {
      rv.quot = -num / denom;
      rv.quot = -rv.quot;
    }
  } else {
    if (denom < 0) {
      rv.quot = num / -denom;
      rv.quot = -rv.quot;
    } else {
      rv.quot = num / denom;
    }
  }
  rv.rem = num - rv.quot * denom;
  return rv;
}
