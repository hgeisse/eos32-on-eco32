/*
 * buffer.c -- basic file buffer manipulation
 */


#include <stdio.h>

#include "joe.h"
#include "buffer.h"


/* Return size of hole */

int fmholesize(void) {
  return ehole-hole;
}


/* Read character at the point */

int fmrc(void) {
  return point==hole?*(point=ehole):*point;
}


/* Overtype character at the point */

void fmwc(int c) {
  if (point==hole) {
    point=ehole;
  }
  if (point==filend) {
    fmexpand(1);
    filend++;
  }
  *point=(c);
  changed=1;
}


/* Read character at point and advance point */

int fmgetc(void) {
  if (point==hole) {
    point=ehole+1;
    return *ehole;
  } else {
    return *(point++);
  }
}


/* Overtype character at point and advance point */

void fmputc(int c) {
  if (point==hole) {
    point=ehole;
  }
  if (point==filend) {
    fmexpand(1);
    filend++;
  }
  *(point++)=c;
  changed=1;
}


/* Insert character at point */

void fminsc(int c) {
  fminsu(1);
  if (point!=hole) {
    fmhole();
  }
  if (hole==ehole) {
    fmbig(1);
  }
  *(hole++)=c;
  changed=1;
}


/* Return the byte offset from the beginning of the buffer to the point */

int fmnote(void) {
  return (point>=ehole)?(point-buffer)-(ehole-hole):point-buffer;
}


/* Return the size of the file in the buffer */

int fmsize(void) {
  return (filend-buffer)-(ehole-hole);
}


/* Return true if the point is at the end of the file */

int fmeof(void) {
  return (point==hole) ? (ehole==filend) : (point==filend);
}


/* Position the point to a byte offset from the beginning of the file */

void fmpoint(int x) {
  point=buffer+x;
  if (point>hole) {
    point+=ehole-hole;
  }
}


/* Retreat the point and then read the character that's there */

int fmrgetc(void) {
  return point==ehole?*(point=hole-1):*(--point);
}


/* Position the point to the next NL or the end of the file.  If the point
   is already at a NL, it is set to the next NL. Return 0 if not found, 1
   if found */

int fmnnl(void) {
  if (fmeof()) {
    return 0;
  }
  fmgetc();
  return fmfnl();
}


/* Set the point to the beginning of the file or the previous NL.  If the
   point is already at a NL, it is set to the one before it.  Return 0 if
   not found, 0 if found */

int fmnrnl(void) {
  if (fmnote()) {
    fmrgetc();
    return fmrnl();
  }
  return 0;
}
