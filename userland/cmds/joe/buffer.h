/*
 * buffer.h -- basic file buffer manipulation
 */


/* Return size of hole */

int fmholesize(void);


/* Read character at the point */

int fmrc(void);


/* Overtype character at the point */

void fmwc(int c);


/* Read character at point and advance point */

int fmgetc(void);


/* Overtype character at point and advance point */

void fmputc(int c);


/* Insert character at point */

void fminsc(int c);


/* Return the byte offset from the beginning of the buffer to the point */

int fmnote(void);


/* Return the size of the file in the buffer */

int fmsize(void);


/* Return true if the point is at the end of the file */

int fmeof(void);


/* Position the point to a byte offset from the beginning of the file */

void fmpoint(int x);


/* Retreat the point and then read the character that's there */

int fmrgetc(void);


/* Position the point to the next NL or the end of the file.  If the point
   is already at a NL, it is set to the next NL. Return 0 if not found, 1
   if found */

int fmnnl(void);


/* Set the point to the beginning of the file or the previous NL.  If the
   point is already at a NL, it is set to the one before it.  Return 0 if
   not found, 0 if found */

int fmnrnl(void);
