/*
 * stdio.c -- input and output
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <eos32.h>
#include <assert.h>


/**************************************************************/

/*
 * data
 */


static unsigned char stdinBuf[BUFSIZ];
static unsigned char stdoutBuf[BUFSIZ];


FILE _files[FOPEN_MAX] = {
  { 0, stdinBuf, stdinBuf, 0, _STRM_RD               },
  { 0, NULL,     NULL,     1, _STRM_WR               },
  { 0, NULL,     NULL,     2, _STRM_WR | _STRM_NOBUF },
};


/**************************************************************/

/*
 * file operations
 */


static FILE *findStream(void) {
  FILE *stream;

  for (stream = _files; stream < _files + FOPEN_MAX; stream++) {
    if ((stream->flags & (_STRM_RD | _STRM_WR | _STRM_RDWR)) == 0) {
      return stream;
    }
  }
  return NULL;
}


static int createFile(const char *filename, int rw) {
  int fd;

  fd = creat(filename, 0666);
  if (rw && fd >= 0) {
    close(fd);
    fd = open(filename, O_RDWR);
  }
  return fd;
}


static FILE *openStream(const char *filename,
                        const char *mode,
                        FILE *stream) {
  char m;
  int rw;
  int fd;

  if (stream == NULL) {
    return NULL;
  }
  m = *mode++;
  if (*mode == 'b') {
    mode++;
  }
  rw = (*mode == '+');
  switch (m) {
    case 'r':
      fd = open(filename, rw ? O_RDWR : O_RDONLY);
      break;
    case 'w':
      fd = createFile(filename, rw);
      break;
    case 'a':
      fd = open(filename, rw ? O_RDWR : O_WRONLY);
      if (fd < 0 && errno == ENOENT) {
        fd = createFile(filename, rw);
      }
      lseek(fd, 0, 2);
      break;
    default:
      return NULL;
  }
  if (fd < 0) {
    return NULL;
  }
  stream->cnt = 0;
  /* do not touch stream->ptr */
  /* do not touch stream->buf */
  stream->fd = fd;
  if (rw) {
    stream->flags |= _STRM_RDWR;
  } else {
    if (m == 'r') {
      stream->flags |= _STRM_RD;
    } else {
      stream->flags |= _STRM_WR;
    }
  }
  return stream;
}


FILE *fopen(const char *filename, const char *mode) {
  return openStream(filename, mode, findStream());
}


FILE *freopen(const char *filename, const char *mode, FILE *stream) {
  fclose(stream);
  return openStream(filename, mode, stream);
}


int fflush(FILE *stream) {
  unsigned char *base;
  int n;

  if ((stream->flags & (_STRM_NOBUF | _STRM_WR)) == _STRM_WR &&
      (base = stream->buf) != NULL &&
      (n = stream->ptr - base) > 0) {
    stream->ptr = base;
    stream->cnt = BUFSIZ;
    if (write(fileno(stream), base, n) != n) {
      stream->flags |= _STRM_ERR;
      return EOF;
    }
  }
  return 0;
}


int fclose(FILE *stream) {
  int r;

  r = EOF;
  if ((stream->flags & (_STRM_RD | _STRM_WR | _STRM_RDWR)) != 0 &&
      (stream->flags & _STRM_STRG) == 0) {
    r = fflush(stream);
    if (close(fileno(stream)) < 0) {
      r = EOF;
    }
    if (stream->flags & _STRM_MYBUF) {
      free(stream->buf);
    }
    if (stream->flags & (_STRM_MYBUF|_STRM_NOBUF)) {
      stream->buf = NULL;
    }
  }
  stream->flags &=
    ~(_STRM_RD | _STRM_WR | _STRM_NOBUF | _STRM_MYBUF |
      _STRM_ERR | _STRM_EOF | _STRM_STRG | _STRM_RDWR);
  stream->cnt = 0;
  return r;
}


int remove(const char *filename) {
  return unlink(filename);
}


int rename(const char *oldname, const char *newname) {
  if (link(oldname, newname)) {
    return -1;
  }
  return unlink(oldname);
}


FILE *tmpfile(void) {
  /* !!!!! */
  assert(1 == 0);
  return NULL;
}


char *tmpnam(char *s) {
  static char tempName[L_tmpnam];
  static unsigned long randomNumber = 0;
  int i;

  if (s == NULL) {
    /* we must provide the buffer */
    s = tempName;
  }
  if (randomNumber == 0) {
    /* first call */
    randomNumber = getpid();
  }
  strcpy(s, "/tmp/tmp");
  for (i = 8; i < L_tmpnam - 1; i++) {
    randomNumber = randomNumber * 1103515245 + 12345;
    s[i] = 'a' + (randomNumber >> 16) % 26;
  }
  s[i] = '\0';
  return s;
}


int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


void setbuf(FILE *stream, char *buf) {
  if (stream->buf != NULL && (stream->flags & _STRM_MYBUF) != 0) {
    free(stream->buf);
  }
  stream->flags &= ~(_STRM_MYBUF | _STRM_NOBUF);
  if ((stream->buf = (unsigned char *) buf) == NULL) {
    stream->flags |= _STRM_NOBUF;
  } else {
    stream->ptr = stream->buf;
  }
  stream->cnt = 0;
}


/**************************************************************/

/*
 * formatted output
 */


#define FLAG_ALT	0x01		/* alternate form of output */
#define FLAG_ZERO	0x02		/* pad with leading zeroes */
#define FLAG_PLUS	0x04		/* always print with a sign */
#define FLAG_LEFT	0x08		/* left adjustment within field */
#define FLAG_SPACE	0x10		/* if no sign, prefix a space */
#define FLAG_PREC	0x20		/* a precision was specified */
#define FLAG_CAPS	0x40		/* use A-F instead of a-f */

#define SIZE_NORMAL	1		/* no length modifier */
#define SIZE_SHORT	2		/* 'h' length modifier */
#define SIZE_LONG	3		/* 'l' length modifier */


static int printString(char *str, int flags,
                       int width, int prec, FILE *stream) {
  int length;
  int i;

  /* determine the length of the string */
  length = 0;
  while (str[length] != '\0') {
    /* if a precision is given, it limits the number of chars */
    if (flags & FLAG_PREC) {
      if (length == prec) {
        break;
      }
    }
    length++;
  }
  /* pad to the left if necessary */
  if (!(flags & FLAG_LEFT)) {
    for (i = 0; i < width - length; i++) {
      putc(' ', stream);
    }
  }
  /* print the string proper */
  for (i = 0; i < length; i++) {
    putc(str[i], stream);
  }
  /* pad to the right if necessary */
  if (flags & FLAG_LEFT) {
    for (i = 0; i < width - length; i++) {
      putc(' ', stream);
    }
  }
  /* return number of chars actually printed */
  return length >= width ? length : width;
}


static int printSigned(signed long snum, int flags,
                       int width, int prec, FILE *stream) {
  int isZero;
  int isNegative;
  int numDigits;
  int digits[10];
  int numZeroes;	/* number of leading zeroes */
  int length;		/* output length, without padding */
  int i;

  /* remember if number is equal to 0 */
  isZero = (snum == 0);
  /* remember if number is less than 0; if so, negate */
  isNegative = (snum < 0);
  if (isNegative) {
    snum = -snum;
  }
  /* if snum and prec are both zero, print nothing but padding spaces */
  if (isZero && (flags & FLAG_PREC) && prec == 0) {
    for (i = 0; i < width; i++) {
      putc(' ', stream);
    }
    return width;
  }
  /* compute digits in reverse order */
  numDigits = 0;
  do {
    digits[numDigits++] = snum % 10;
    snum /= 10;
  } while (snum);
  /* determine the length of the string */
  numZeroes = 0;
  length = numDigits;
  /* if more digits are needed, these are zeroes */
  i = numZeroes + numDigits;
  if ((flags & FLAG_PREC) && prec > i) {
    numZeroes += prec - i;
    length += prec - i;
  }
  /* don't forget the sign */
  if (isNegative || (flags & (FLAG_PLUS | FLAG_SPACE))) {
    length++;
  }
  /* pad to the left if necessary */
  if (!(flags & FLAG_LEFT) && width > length) {
    if (flags & FLAG_ZERO) {
      /* pad with zeroes, after the sign */
      numZeroes += width - length;
    } else {
      /* pad with spaces, now */
      for (i = 0; i < width - length; i++) {
        putc(' ', stream);
      }
    }
  }
  /* print the sign if necessary */
  if (isNegative) {
    putc('-', stream);
  } else
  if (flags & FLAG_PLUS) {
    putc('+', stream);
  } else
  if (flags & FLAG_SPACE) {
    putc(' ', stream);
  }
  /* print the leading zeroes */
  for (i = 0; i < numZeroes; i++) {
    putc('0', stream);
  }
  /* print the digits proper */
  while (numDigits--) {
    fputc('0' + digits[numDigits], stream);
  }
  /* pad to the right if necessary */
  if (flags & FLAG_LEFT) {
    for (i = 0; i < width - length; i++) {
      putc(' ', stream);
    }
  }
  /* return number of chars actually printed */
  return width > length ? width : length;
}


static int printUnsigned(unsigned long unum, int base, int flags,
                         int width, int prec, FILE *stream) {
  static char *lowerCase = "0123456789abcdef";
  static char *upperCase = "0123456789ABCDEF";
  char *dp;
  int isZero;
  int numDigits;
  int digits[11];
  int numZeroes;	/* number of leading zeroes */
  int length;		/* output length, without padding */
  int i;

  /* choose upper or lower case hex digits */
  dp = flags & FLAG_CAPS ? upperCase : lowerCase;
  /* remember if number is equal to 0 */
  isZero = (unum == 0);
  /* if unum and prec are both zero, print nothing but padding spaces */
  if (isZero && (flags & FLAG_PREC) && prec == 0) {
    for (i = 0; i < width; i++) {
      putc(' ', stream);
    }
    return width;
  }
  /* compute digits in reverse order */
  numDigits = 0;
  do {
    digits[numDigits++] = unum % base;
    unum /= base;
  } while (unum);
  /* determine the length of the string */
  numZeroes = 0;
  length = numDigits;
  /* consider alternate output form */
  if ((flags & FLAG_ALT) && !isZero) {
    if (base == 8) {
      /* prepend zero */
      numZeroes++;
      length++;
    } else
    if (base == 16) {
      /* prepend 0x or 0X */
      length += 2;
    }
  }
  /* if more digits are needed, these are zeroes */
  i = numZeroes + numDigits;
  if ((flags & FLAG_PREC) && prec > i) {
    numZeroes += prec - i;
    length += prec - i;
  }
  /* pad to the left if necessary */
  if (!(flags & FLAG_LEFT) && width > length) {
    if (flags & FLAG_ZERO) {
      /* pad with zeroes, after the prefix */
      numZeroes += width - length;
    } else {
      /* pad with spaces, now */
      for (i = 0; i < width - length; i++) {
        putc(' ', stream);
      }
    }
  }
  /* print 0x or 0X if necessary */
  if ((flags & FLAG_ALT) && !isZero && base == 16) {
    putc('0', stream);
    if (flags & FLAG_CAPS) {
      putc('X', stream);
    } else {
      putc('x', stream);
    }
  }
  /* print the leading zeroes */
  for (i = 0; i < numZeroes; i++) {
    putc('0', stream);
  }
  /* print the digits proper */
  while (numDigits--) {
    fputc(dp[digits[numDigits]], stream);
  }
  /* pad to the right if necessary */
  if (flags & FLAG_LEFT) {
    for (i = 0; i < width - length; i++) {
      putc(' ', stream);
    }
  }
  /* return number of chars actually printed */
  return width > length ? width : length;
}


static int doPrintf(FILE *stream, const char *format, va_list ap) {
  int count;
  char c;
  int flags;
  int width;
  int prec;
  int size;
  char *str;
  signed long snum;
  unsigned long unum;
  int base;

  count = 0;
  while ((c = *format++) != '\0') {
    /* conversion specifier? */
    if (c != '%') {
      /* no, output character verbatim */
      putc(c, stream);
      count++;
      continue;
    }
    /* else, look one character ahead */
    c = *format++;
    /* set some defaults */
    flags = 0;
    width = 0;
    prec = 0;
    size = SIZE_NORMAL;
    /* first, the flags */
    while (c == '#' || c == '0' || c == '+' || c == '-' || c == ' ') {
      switch (c) {
        case '#':
          /* alternate output form */
          flags |= FLAG_ALT;
          break;
        case '0':
          /* pad numbers with zeroes */
          flags |= FLAG_ZERO;
          break;
        case '+':
          /* print numbers always with a sign */
          flags |= FLAG_PLUS;
          break;
        case '-':
          /* left adjustment */
          flags |= FLAG_LEFT;
          break;
        case ' ':
          /* prefix a space, if first char is not a sign */
          flags |= FLAG_SPACE;
          break;
      }
      c = *format++;
    }
    /* next, the width field */
    if (c == '*') {
      /* computed value */
      width = va_arg(ap, int);
      /* negative width means left adjustment */
      if (width < 0) {
        width = -width;
        flags |= FLAG_LEFT;
      }
      /* maintain one char lookahead */
      c = *format++;
    } else {
      /* constant value */
      while (c >= '0' && c <= '9') {
        width = width * 10 + (c - '0');
        c = *format++;
      }
    }
    /* then, the precision field */
    if (c == '.') {
      c = *format++;
      flags |= FLAG_PREC;
      if (c == '*') {
        /* computed value */
        prec = va_arg(ap, int);
        /* negative precision counts as 0 */
        if (prec < 0) {
          prec = 0;
        }
        /* maintain one char lookahead */
        c = *format++;
      } else {
        /* constant value */
        while (c >= '0' && c <= '9') {
          prec = prec * 10 + (c - '0');
          c = *format++;
        }
      }
    }
    /* now, the length modifier */
    if (c == 'h') {
      size = SIZE_SHORT;
      c = *format++;
    } else
    if (c == 'l') {
      size = SIZE_LONG;
      c = *format++;
    }
    /* finally, the conversion character */
    switch (c) {
      case '%':
        putc(c, stream);
        count++;
        break;
      case 'c':
        c = va_arg(ap, char);
        putc(c, stream);
        count++;
        break;
      case 's':
        str = va_arg(ap, char *);
        count += printString(str, flags, width, prec, stream);
        break;
      case 'd':
      case 'i':
        switch (size) {
          case SIZE_NORMAL:
            snum = va_arg(ap, int);
            break;
          case SIZE_SHORT:
            snum = va_arg(ap, short);
            break;
          case SIZE_LONG:
            snum = va_arg(ap, long);
            break;
        }
        count += printSigned(snum, flags, width, prec, stream);
        break;
      case 'u':
      case 'o':
      case 'x':
      case 'X':
        switch (c) {
          case 'u':
            base = 10;
            break;
          case 'o':
            base = 8;
            break;
          case 'x':
            base = 16;
            break;
          case 'X':
            base = 16;
            flags |= FLAG_CAPS;
            break;
        }
        switch (size) {
          case SIZE_NORMAL:
            unum = va_arg(ap, unsigned int);
            break;
          case SIZE_SHORT:
            unum = va_arg(ap, unsigned short);
            break;
          case SIZE_LONG:
            unum = va_arg(ap, unsigned long);
            break;
        }
        count += printUnsigned(unum, base, flags, width, prec, stream);
        break;
      case 'p':
        flags |= FLAG_ALT | FLAG_ZERO;
        unum = (unsigned long) va_arg(ap, void *);
        count += printUnsigned(unum, 16, flags, 10, 0, stream);
        break;
      default:
        /* unknown conversion type, return error */
        return -1;
    }
  }
  return count;
}


int vfprintf(FILE *stream, const char *format, va_list ap) {
  int count;

  count = doPrintf(stream, format, ap);
  return ferror(stream) ? EOF : count;
}


int fprintf(FILE *stream, const char *format, ...) {
  va_list ap;
  int count;

  va_start(ap, format);
  count = doPrintf(stream, format, ap);
  va_end(ap);
  return ferror(stream) ? EOF : count;
}


int vprintf(const char *format, va_list ap) {
  int count;

  count = doPrintf(stdout, format, ap);
  return ferror(stdout) ? EOF : count;
}


int printf(const char *format, ...) {
  va_list ap;
  int count;

  va_start(ap, format);
  count = doPrintf(stdout, format, ap);
  va_end(ap);
  return ferror(stdout) ? EOF : count;
}


int vsprintf(char *s, const char *format, va_list ap) {
  FILE stream;
  int count;

  stream.cnt = INT_MAX;
  stream.ptr = (unsigned char *) s;
  stream.buf = NULL;
  stream.fd = -1;
  stream.flags = _STRM_WR | _STRM_STRG;
  count = doPrintf(&stream, format, ap);
  putc('\0', &stream);
  return count;
}


int sprintf(char *s, const char *format, ...) {
  FILE stream;
  va_list ap;
  int count;

  stream.cnt = INT_MAX;
  stream.ptr = (unsigned char *) s;
  stream.buf = NULL;
  stream.fd = -1;
  stream.flags = _STRM_WR | _STRM_STRG;
  va_start(ap, format);
  count = doPrintf(&stream, format, ap);
  va_end(ap);
  putc('\0', &stream);
  return count;
}


/**************************************************************/

/*
 * formatted input
 */


int scanf(const char *format, ...) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


int fscanf(FILE *stream, const char *format, ...) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


int sscanf(const char *s, const char *format, ...) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


/**************************************************************/

/*
 * character input and output
 */


int _fillBuffer(FILE *stream) {
  static unsigned char smallbuf[FOPEN_MAX];

  if (stream->flags & _STRM_RDWR) {
    stream->flags |= _STRM_RD;
  }
  if ((stream->flags & _STRM_RD) == 0 ||
      (stream->flags & _STRM_STRG) != 0) {
    return EOF;
  }
again:
  if (stream->buf == NULL) {
    if (stream->flags & _STRM_NOBUF) {
      stream->buf = &smallbuf[fileno(stream)];
      goto again;
    }
    if ((stream->buf = malloc(BUFSIZ)) == NULL) {
      stream->flags |= _STRM_NOBUF;
      goto again;
    }
    stream->flags |= _STRM_MYBUF;
  }
  stream->ptr = stream->buf;
  stream->cnt = read(fileno(stream), stream->ptr,
                     stream->flags & _STRM_NOBUF ? 1 : BUFSIZ);
  if (--stream->cnt < 0) {
    if (stream->cnt == -1) {
      stream->flags |= _STRM_EOF;
      if (stream->flags & _STRM_RDWR) {
        stream->flags &= ~_STRM_RD;
      }
    } else {
      stream->flags |= _STRM_ERR;
    }
    stream->cnt = 0;
    return EOF;
  }
  return *stream->ptr++ & 0377;
}


int _flushBuffer(int c, FILE *stream) {
  unsigned char *buf;
  int n, rn;
  char c1;

  if (stream->flags & _STRM_RDWR) {
    stream->flags |= _STRM_WR;
    stream->flags &= ~_STRM_EOF;
  }
again:
  if (stream->flags & _STRM_NOBUF) {
    c1 = c;
    rn = 1;
    n = write(fileno(stream), &c1, rn);
    stream->cnt = 0;
  } else {
    if ((buf = stream->buf) == NULL) {
      if (stream == stdout) {
        if (isatty(fileno(stdout))) {
          stream->flags |= _STRM_NOBUF;
          goto again;
        }
        stream->buf = stdoutBuf;
        stream->ptr = stdoutBuf;
        goto again;
      }
      if ((stream->buf = buf = malloc(BUFSIZ)) == NULL) {
        stream->flags |= _STRM_NOBUF;
        goto again;
      }
      stream->flags |= _STRM_MYBUF;
      rn = n = 0;
    } else
    if ((rn = n = stream->ptr - buf) > 0) {
      stream->ptr = buf;
      n = write(fileno(stream), buf, n);
    }
    stream->cnt = BUFSIZ - 1;
    *buf++ = c;
    stream->ptr = buf;
  }
  if (rn != n) {
    stream->flags |= _STRM_ERR;
    return EOF;
  }
  return c;
}


int fgetc(FILE *stream) {
  return getc(stream);
}


char *fgets(char *s, int n, FILE *stream) {
  char *p;
  int c;

  p = s;
  while (--n > 0 && (c = getc(stream)) >= 0) {
    *p++ = c;
    if (c == '\n') {
      break;
    }
  }
  if (c < 0 && p == s) {
    return NULL;
  }
  *p = '\0';
  return s;
}


char *gets(char *s) {
  char *p;
  int c;

  p = s;
  while ((c = getchar()) != '\n' && c >= 0) {
    *p++ = c;
  }
  if (c < 0 && p == s) {
    return NULL;
  }
  *p = '\0';
  return s;
}


int fputc(int c, FILE *stream) {
  return putc(c, stream);
}


int fputs(const char *s, FILE *stream) {
  int c;
  int r;

  while ((c = *s++) != '\0') {
    r = putc(c, stream);
  }
  return r;
}


int puts(const char *s) {
  int c;

  while ((c = *s++) != '\0') {
    putchar(c);
  }
  return putchar('\n');
}


int ungetc(int c, FILE *stream) {
  if (c == EOF) {
    return -1;
  }
  if ((stream->flags & _STRM_RD) == 0 || stream->ptr <= stream->buf) {
    if (stream->ptr == stream->buf && stream->cnt == 0) {
      stream->ptr++;
    } else {
      return -1;
    }
  }
  stream->cnt++;
  *--stream->ptr = c;
  return 0;
}


/**************************************************************/

/*
 * direct input and output
 */


size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream) {
  unsigned char *p;
  unsigned ndone;
  int c;
  unsigned s;

  p = ptr;
  ndone = 0;
  if (size != 0) {
    for (; ndone < nobj; ndone++) {
      s = size;
      do {
        if ((c = getc(stream)) >= 0) {
          *p++ = c;
        } else {
          return ndone;
        }
      } while (--s);
    }
  }
  return ndone;
}


size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream) {
  const unsigned char *p;
  unsigned ndone;
  unsigned s;

  p = ptr;
  ndone = 0;
  if (size != 0) {
    for (; ndone < nobj; ndone++) {
      s = size;
      do {
        putc(*p++, stream);
      } while (--s);
      if (ferror(stream)) {
        break;
      }
    }
  }
  return ndone;
}


/**************************************************************/

/*
 * file positioning
 */


int fseek(FILE *stream, long offset, int origin) {
  int cnt;
  long pos;

  stream->flags &= ~_STRM_EOF;
  if (stream->flags & _STRM_RD) {
    if (origin < 2 &&
        stream->buf != NULL &&
        !(stream->flags & _STRM_NOBUF)) {
      cnt = stream->cnt;
      pos = offset;
      if (origin == 0) {
        pos += cnt - lseek(fileno(stream), 0, 1);
      } else {
        offset -= cnt;
      }
      if (!(stream->flags & _STRM_RDWR) &&
          cnt > 0 &&
          pos <= cnt &&
          pos >= stream->buf - stream->ptr) {
        stream->ptr += (int) pos;
        stream->cnt -= (int) pos;
        return 0;
      }
    }
    if (stream->flags & _STRM_RDWR) {
      stream->ptr = stream->buf;
      stream->flags &= ~_STRM_RD;
    }
    pos = lseek(fileno(stream), offset, origin);
    stream->cnt = 0;
  } else
  if (stream->flags & (_STRM_WR | _STRM_RDWR)) {
    fflush(stream);
    if (stream->flags & _STRM_RDWR) {
      stream->cnt = 0;
      stream->flags &= ~_STRM_WR;
      stream->ptr = stream->buf;
    }
    pos = lseek(fileno(stream), offset, origin);
  }
  return pos == -1 ? -1 : 0;
}


long ftell(FILE *stream) {
  long pos;
  int adjust;

  if (stream->cnt < 0) {
    stream->cnt = 0;
  }
  if (stream->flags & _STRM_RD) {
    adjust = - stream->cnt;
  } else
  if (stream->flags & (_STRM_WR | _STRM_RDWR)) {
    adjust = 0;
    if ((stream->flags & _STRM_WR) != 0 &&
        stream->buf != NULL &&
        (stream->flags & _STRM_NOBUF) == 0) {
      adjust = stream->ptr - stream->buf;
    }
  } else {
    return -1;
  }
  pos = lseek(fileno(stream), 0, 1);
  if (pos < 0) {
    return pos;
  }
  pos += adjust;
  return pos;
}


void rewind(FILE *stream) {
  fflush(stream);
  lseek(fileno(stream), 0, 0);
  stream->cnt = 0;
  stream->ptr = stream->buf;
  stream->flags &= ~(_STRM_EOF | _STRM_ERR);
  if (stream->flags & _STRM_RDWR) {
    stream->flags &= ~(_STRM_RD | _STRM_WR);
  }
}


int fgetpos(FILE *stream, fpos_t *ptr) {
  long pos;

  pos = ftell(stream);
  if (pos < 0) {
    return -1;
  }
  *ptr = pos;
  return 0;
}


int fsetpos(FILE *stream, const fpos_t *ptr) {
  return fseek(stream, *ptr, SEEK_SET);
}


/**************************************************************/

/*
 * error handling
 */


void clearerr(FILE *stream) {
  stream->flags &= ~(_STRM_EOF | _STRM_ERR);
}


int feof(FILE *stream) {
  return (stream->flags & _STRM_EOF) != 0;
}


int ferror(FILE *stream) {
  return (stream->flags & _STRM_ERR) != 0;
}


void perror(const char *s) {
  if (s != NULL && *s != '\0') {
    fprintf(stderr, "%s: ", s);
  }
  fprintf(stderr, "%s\n", strerror(errno));
}
