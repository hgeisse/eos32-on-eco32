/*
 * klib.c -- the kernel library
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/conf.h"
#include "../include/stdarg.h"

#include "start.h"
#include "klib.h"
#include "alloc.h"
#include "ureg.h"
#include "logfile.h"
#include "console.h"
#include "trm.h"


/**************************************************************/


Bool debugCopyFrame = FALSE;
Bool debugClearFrame = FALSE;
Bool debugCopyIn = FALSE;
Bool debugCopyOut = FALSE;
Bool debugFubyte = FALSE;
Bool debugFuword = FALSE;
Bool debugSubyte = FALSE;
Bool debugSuword = FALSE;

Bool outputToLogfile = FALSE;
Bool outputToConsole = FALSE;
Bool outputToTerminal = FALSE;


/**************************************************************/


/*
 * This is only for debugging.
 * Call this routine wherever you want to break execution.
 * Supply a unique break number to distinguish the different
 * calls. When running the program, place a breakpoint at
 * the very beginning of this routine.
 */
void debugBreak(int breakNumber) {
}


/**************************************************************/


/*
 * Copy the contents of a source frame
 * to a destination frame (no checks).
 */
void copyFrame(int src, int dst) {
  unsigned int *s;
  unsigned int *d;
  int n;

  if (debugCopyFrame) {
    printf("-----  copyFrame %d (0x%X) --> %d (0x%X)  -----\n",
           src, src, dst, dst);
  }
  s = (unsigned int *) FRAME2ADDR(src);
  d = (unsigned int *) FRAME2ADDR(dst);
  n = PAGE_SIZE / sizeof(unsigned int);
  while (n--) {
    *d++ = *s++;
  }
}


/*
 * Clear the contents of a destination
 * frame (no checks).
 */
void clearFrame(int dst) {
  unsigned int *d;
  int n;

  if (debugClearFrame) {
    printf("-----  clearFrame %d (0x%X)  -----\n",
           dst, dst);
  }
  d = (unsigned int *) FRAME2ADDR(dst);
  n = PAGE_SIZE / sizeof(unsigned int);
  while (n--) {
    *d++ = 0;
  }
}


/**************************************************************/


/*
 * Copy bytes from user to kernel space. Check if
 * the user space addresses are readable.
 * Return FALSE if ok, TRUE on error.
 */
Bool copyIn(caddr_t src, caddr_t dst, int cnt) {
  if (debugCopyIn) {
    printf("-----  copyIn src = 0x%X, dst = 0x%X, cnt = 0x%X  -----\n",
           src, dst, cnt);
  }
  if (checkAddrs(src, cnt, FALSE)) {
    return TRUE;
  }
  while (cnt--) {
    *dst++ = *src++;
  }
  return FALSE;
}


/*
 * Copy bytes from kernel to user space. Check if
 * the user space addresses are writeable.
 * Return FALSE if ok, TRUE on error.
 */
Bool copyOut(caddr_t src, caddr_t dst, int cnt) {
  if (debugCopyOut) {
    printf("-----  copyOut src = 0x%X, dst = 0x%X, cnt = 0x%X  -----\n",
           src, dst, cnt);
  }
  if (checkAddrs(dst, cnt, TRUE)) {
    return TRUE;
  }
  while (cnt--) {
    *dst++ = *src++;
  }
  return FALSE;
}


/**************************************************************/


/*
 * Fetch a byte from the user's address space.
 * Check if the location is accessible. Return
 * -1 if it isn't, otherwise return the byte.
 */
int fubyte(caddr_t uaddr) {
  int c;

  if (debugFubyte) {
    printf("-----  fubyte uaddr = 0x%08X  -----\n", uaddr);
  }
  if (checkAddrs(uaddr, 1, FALSE)) {
    c = -1;
  } else {
    c = *uaddr;
    c &= 0xFF;
  }
  if (debugFubyte) {
    printf("       fubyte = 0x%08X\n", c);
  }
  return c;
}


/*
 * Fetch a word from the user's address space.
 * Check if the location is accessible. Return
 * -1 if it isn't, otherwise return the word.
 */
int fuword(caddr_t uaddr) {
  int c;

  if (debugFuword) {
    printf("-----  fuword uaddr = 0x%08X  -----\n", uaddr);
  }
  if (checkAddrs(uaddr, 4, FALSE)) {
    c = -1;
  } else {
    c = *(int *)uaddr;
  }
  if (debugFuword) {
    printf("       fuword = 0x%08X\n", c);
  }
  return c;
}


/*
 * Set a byte in the user's address space.
 * Check if the location is writeable. Return
 * -1 if it isn't, otherwise return 0.
 */
int subyte(caddr_t uaddr, unsigned int val) {
  int c;

  if (debugSubyte) {
    printf("-----  subyte uaddr = 0x%08X, val = 0x%08X  -----\n",
           uaddr, val);
  }
  if (checkAddrs(uaddr, 1, TRUE)) {
    c = -1;
  } else {
    c = 0;
  }
  *uaddr = val;
  if (debugSubyte) {
    printf("       subyte = 0x%08X\n", c);
  }
  return c;
}


/*
 * Set a word in the user's address space.
 * Check if the location is writeable. Return
 * -1 if it isn't, otherwise return 0.
 */
int suword(caddr_t uaddr, unsigned int val) {
  int c;

  if (debugSuword) {
    printf("-----  suword uaddr = 0x%08X, val = 0x%08X  -----\n",
           uaddr, val);
  }
  if (checkAddrs(uaddr, 4, TRUE)) {
    c = -1;
  } else {
    c = 0;
  }
  *(unsigned int *)uaddr = val;
  if (debugSuword) {
    printf("       suword = 0x%08X\n", c);
  }
  return c;
}


/**************************************************************/


/*
 * Count the length of a string (without terminating null character).
 */
int strlen(char *s) {
  int n;

  n = 0;
  while (*s++ != '\0') {
    n++;
  }
  return n;
}


/*
 * Copy a string from src to dst (with terminating null character).
 */
void strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0') ;
}


/**************************************************************/


/*
 * Output a character to logfile, console, or terminal.
 * Replace LF by CR/LF.
 */
static void charOut(char c) {
  if (c == '\n') {
    if (outputToLogfile) {
      logfileOut('\r');
    }
    if (outputToConsole) {
      consoleOut('\r');
    }
    if (outputToTerminal) {
      terminalOut('\r');
    }
  }
  if (outputToLogfile) {
    logfileOut(c);
  }
  if (outputToConsole) {
    consoleOut(c);
  }
  if (outputToTerminal) {
    terminalOut(c);
  }
}


/*
 * Count the number of characters needed to represent
 * a given number in base 10.
 */
static int countPrintn(long n) {
  long a;
  int res;

  res = 0;
  if (n < 0) {
    res++;
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    res += countPrintn(a);
  }
  return res + 1;
}


/*
 * Output a number in base 10.
 */
static void printn(long n) {
  long a;

  if (n < 0) {
    charOut('-');
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    printn(a);
  }
  charOut(n % 10 + '0');
}


/*
 * Count the number of characters needed to represent
 * a given number in a given base.
 */
static int countPrintu(unsigned long n, unsigned long b) {
  unsigned long a;
  int res;

  res = 0;
  a = n / b;
  if (a != 0) {
    res += countPrintu(a, b);
  }
  return res + 1;
}


/*
 * Output a number in a given base.
 */
static void printu(unsigned long n, unsigned long b, Bool upperCase) {
  unsigned long a;

  a = n / b;
  if (a != 0) {
    printu(a, b, upperCase);
  }
  if (upperCase) {
    charOut("0123456789ABCDEF"[n % b]);
  } else {
    charOut("0123456789abcdef"[n % b]);
  }
}


/*
 * Output a number of filler characters.
 */
static void fill(int numFillers, char filler) {
  while (numFillers-- > 0) {
    charOut(filler);
  }
}


/*
 * Formatted output with a variable argument list.
 */
static void vprintf(char *fmt, va_list ap) {
  char c;
  int n;
  long ln;
  unsigned int u;
  unsigned long lu;
  char *s;
  Bool negFlag;
  char filler;
  int width, count;

  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        return;
      }
      charOut(c);
    }
    c = *fmt++;
    if (c == '-') {
      negFlag = TRUE;
      c = *fmt++;
    } else {
      negFlag = FALSE;
    }
    if (c == '0') {
      filler = '0';
      c = *fmt++;
    } else {
      filler = ' ';
    }
    width = 0;
    while (c >= '0' && c <= '9') {
      width *= 10;
      width += c - '0';
      c = *fmt++;
    }
    if (c == 'd') {
      n = va_arg(ap, int);
      count = countPrintn(n);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printn(n);
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
      u = va_arg(ap, int);
      count = countPrintu(u,
                c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printu(u,
             c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
             c == 'X');
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'l') {
      c = *fmt++;
      if (c == 'd') {
        ln = va_arg(ap, long);
        count = countPrintn(ln);
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printn(ln);
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else
      if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
        lu = va_arg(ap, long);
        count = countPrintu(lu,
                  c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printu(lu,
               c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
               c == 'X');
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else {
        charOut('l');
        charOut(c);
      }
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      count = strlen(s);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      while ((c = *s++) != '\0') {
        charOut(c);
      }
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'c') {
      c = va_arg(ap, char);
      charOut(c);
    } else {
      charOut(c);
    }
  }
}


/*
 * Formatted output.
 * This is a scaled-down version of the C library's
 * printf. Used to print diagnostic information on
 * the console (and optionally to a logfile).
 */
void printf(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}


/**************************************************************/


/*
 * panic is called on unresolvable
 * fatal errors.
 * It prints "kernel panic: mesg",
 * syncs, and then loops.
 */
void panic(char *s) {
  printf("kernel panic: %s\n", s);
  update();
  for (;;) {
    idle();
  }
}


/*
 * prdev prints a warning message of the
 * form "mesg on dev x/y".
 * x and y are the major and minor parts of
 * the device argument.
 */
void prdev(char *str, dev_t dev) {
  printf("%s on device %u/%u\n", str, major(dev), minor(dev));
}


/*
 * deverr prints a diagnostic from
 * a device driver.
 * It prints the device, block number,
 * and a hex word (usually some error
 * status register) passed as argument.
 */
void deverror(dev_t dev, daddr_t blkno, unsigned int x) {
  prdev("error", dev);
  printf("block = %ld, error = 0x%08X\n", blkno, x);
}
