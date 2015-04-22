/*
 * string.c -- string functions
 */


#include <string.h>


/*
 * Copy string ct to string s (includes terminating null character).
 * Return string s.
 */

char *strcpy(char *s, const char *ct) {
  char *p;

  p = s;
  while ((*s++ = *ct++) != '\0') ;
  return p;
}


/*
 * Copy string ct to string s, truncating or null-padding to always
 * copy n bytes. Return string s.
 */

char *strncpy(char *s, const char *ct, size_t n) {
  char *p;
  size_t i;

  p = s;
  for (i = 0; i < n; i++) {
    if ((*s++ = *ct++) == '\0') {
      while (++i < n) {
        *s++ = '\0';
      }
      break;
    }
  }
  return p;
}


/*
 * Append string ct to string s. Return string s.
 */

char *strcat(char *s, const char *ct) {
  char *p;

  p = s;
  while (*s++ != '\0') ;
  s--;
  while ((*s++ = *ct++) != '\0') ;
  return p;
}


/*
 * Append string ct to string s. At most n characters are moved.
 * Return string s.
 */

char *strncat(char *s, const char *ct, size_t n) {
  char *p;

  p = s;
  while (*s++ != '\0') ;
  s--;
  while ((*s++ = *ct++) != '\0') {
    if (n-- == 0) {
      *--s = '\0';
      break;
    }
  }
  return p;
}


/*
 * Compare two strings.
 * Return a number < 0, = 0, or > 0 iff the first string is less
 * than, equal to, or greater than the second one, respectively.
 */

int strcmp(const char *cs, const char *ct) {
  while (*cs == *ct) {
    if (*cs == '\0') {
      return 0;
    }
    cs++;
    ct++;
  }
  return (* (unsigned char *) cs) - (* (unsigned char *) ct);
}


/*
 * Compare two strings. At most n characters are compared.
 * Return a number < 0, = 0, or > 0 iff the first string is less
 * than, equal to, or greater than the second one, respectively.
 */
int strncmp(const char *cs, const char *ct, size_t n) {
  if (n == 0) {
    return 0;
  }
  while (*cs == *ct) {
    if (*cs == '\0' || --n == 0) {
      return 0;
    }
    cs++;
    ct++;
  }
  return (* (unsigned char *) cs) - (* (unsigned char *) ct);
}


/*
 * Locate first appearance of character c in string cs. Return a
 * pointer into cs, or NULL if the character could not be found.
 */

char *strchr(const char *cs, int c) {
  do {
    if (*cs == c) {
      return (char *) cs;
    }
  } while (*cs++ != '\0');
  return NULL;
}


/*
 * Locate last appearance of character c in string cs. Return a
 * pointer into cs, or NULL if the character could not be found.
 */

char *strrchr(const char *cs, int c) {
  char *p;

  p = NULL;
  do {
    if (*cs == c) {
      p = (char *) cs;
    }
  } while (*cs++ != '\0');
  return p;
}


/*
 * Calculate the length of the initial segment of cs which
 * consists entirely of characters contained in ct.
 */
size_t strspn(const char *cs, const char *ct) {
  const char *p;
  const char *q;

  p = cs;
  while (*cs != '\0') {
    for (q = ct; *q != '\0'; q++) {
      if (*cs == *q) {
        break;
      }
    }
    if (*q == '\0') {
      break;
    }
    cs++;
  }
  return cs - p;
}


/*
 * Calculate the length of the initial segment of cs which
 * consists entirely of characters not contained in ct.
 */
size_t strcspn(const char *cs, const char *ct) {
  const char *p;
  const char *q;

  p = cs;
  while (*cs != '\0') {
    for (q = ct; *q != '\0'; q++) {
      if (*cs == *q) {
        break;
      }
    }
    if (*q != '\0') {
      break;
    }
    cs++;
  }
  return cs - p;
}


/*
 * Locate the first occurrence of any character in string ct
 * within the string cs. Return NULL if none are present.
 */
char *strpbrk(const char *cs, const char *ct) {
  char c;
  const char *q;

  while (*cs != '\0') {
    c = *cs;
    q = ct;
    while (*q != '\0') {
      if (*q == c) {
        return (char *) cs;
      }
      q++;
    }
    cs++;
  }
  return NULL;
}


/*
 * Locate the first occurrence of string ct within the string
 * cs. The terminating null characters are not compared. Return
 * NULL if the string cannot be found. If the string ct has
 * zero length, return cs.
 */
char *strstr(const char *cs, const char *ct) {
  const char *p;
  const char *q;

  if (*ct == '\0') {
    return (char *) cs;
  }
  while (*cs != '\0') {
    if (*cs == *ct) {
      p = cs;
      q = ct;
      do {
        p++;
        q++;
        if (*q == '\0') {
          return (char *) cs;
        }
      } while (*p == *q);
    }
    cs++;
  }
  return NULL;
}


/*
 * Count the length of a string
 * (without the terminating null character).
 */

size_t strlen(const char *cs) {
  const char *p;

  p = cs;
  while (*cs != '\0') {
    cs++;
  }
  return cs - p;
}


/*
 * This is a list of error messages.
 */

static char *errlist[] = {
  /*  0 */  "no error",
  /*  1 */  "not owner",
  /*  2 */  "no such file or directory",
  /*  3 */  "no such process",
  /*  4 */  "interrupted system call",
  /*  5 */  "I/O error",
  /*  6 */  "no such device or address",
  /*  7 */  "argument list too long",
  /*  8 */  "exec format error",
  /*  9 */  "bad file descriptor",
  /* 10 */  "no children",
  /* 11 */  "no more processes",
  /* 12 */  "not enough memory",
  /* 13 */  "permission denied",
  /* 14 */  "bad address",
  /* 15 */  "block device required",
  /* 16 */  "mount device busy",
  /* 17 */  "file exists",
  /* 18 */  "cross-device link",
  /* 19 */  "no such device",
  /* 20 */  "not a directory",
  /* 21 */  "is a directory",
  /* 22 */  "invalid argument",
  /* 23 */  "file table overflow",
  /* 24 */  "too many open files",
  /* 25 */  "not a terminal",
  /* 26 */  "text file busy",
  /* 27 */  "file too large",
  /* 28 */  "no space left on device",
  /* 29 */  "illegal seek",
  /* 30 */  "read-only file system",
  /* 31 */  "too many links",
  /* 32 */  "broken pipe",
  /* 33 */  "math argument",
  /* 34 */  "result too large",
};


/*
 * Return a pointer to an implementation-defined string
 * corresponding to error n.
 */

char *strerror(int n) {
  if (n < 0 || n >= sizeof(errlist)/sizeof(errlist[0])) {
    return "unknown error";
  }
  return errlist[n];
}


/*
 * Extract the next token from the string s, delimited by any
 * character from the delimiter string ct.
 */

char *strtok(char *s, const char *ct) {
  static char *p;
  char *q;

  if (s != NULL) {
    p = s;
  } else {
    p++;
  }
  while (*p != '\0' && strchr(ct, *p) != NULL) {
    p++;
  }
  if (*p == '\0') {
    return NULL;
  }
  q = p++;
  while (*p != '\0' && strchr(ct, *p) == NULL) {
    p++;
  }
  if (*p != '\0') {
    *p = '\0';
  } else {
    p--;
  }
  return q;
}


/*
 * Copy n characters from ct to s, and return s.
 */

void *memcpy(void *s, const void *ct, size_t n) {
  unsigned char *dst;
  const unsigned char *src;

  dst = s;
  src = ct;
  while (n--) {
    *dst++ = *src++;
  }
  return s;
}


/*
 * Same as memcpy, but works even if addresses overlap.
 */

void *memmove(void *s, const void *ct, size_t n) {
  unsigned char *dst;
  const unsigned char *src;

  dst = s;
  src = ct;
  if (dst < src) {
    /* copy in ascending order of addresses */
    while (n--) {
      *dst++ = *src++;
    }
  } else
  if (dst > src) {
    /* copy in descending order of addresses */
    dst += n;
    src += n;
    while (n--) {
      *--dst = *--src;
    }
  } else {
    /* dst == src: do not copy anything */
  }
  return s;
}


/*
 * Compare the first n characters of cs with ct;
 * return as with strcmp.
 */

int memcmp(const void *cs, const void *ct, size_t n) {
  const unsigned char *p;
  const unsigned char *q;

  p = cs;
  q = ct;
  while (n--) {
    if (*p != *q) {
      return *p - *q;
    }
    p++;
    q++;
  }
  return 0;
}


/*
 * Return pointer to first occurrence of character c in cs,
 * or NULL if not present among the first n characters.
 */

void *memchr(const void *cs, int c, size_t n) {
  const unsigned char *p;

  p = cs;
  while (n--) {
    if (*p++ == (unsigned char) c) {
      return (void *) p;
    }
  }
  return NULL;
}


/*
 * Place character c into first n characters of s, return s.
 */

void *memset(void *s, int c, size_t n) {
  unsigned char *dst;

  dst = s;
  while (n--) {
    *dst++ = (unsigned char) c;
  }
  return s;
}
