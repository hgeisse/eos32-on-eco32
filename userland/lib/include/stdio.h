/*
 * stdio.h -- input and output
 */


#ifndef _STDIO_H_
#define _STDIO_H_


#include <stddef.h>
#include <stdarg.h>


#ifndef NULL
#define NULL	((void *) 0)
#endif


#define BUFSIZ		4096
#define EOF		-1
#define FOPEN_MAX	20

#define stdin		(&_files[0])
#define stdout		(&_files[1])
#define stderr		(&_files[2])


typedef struct {
  int cnt;		/* chars (rd) or free slots (wr) left in buffer */
  unsigned char *ptr;	/* next char (rd) or free slot (wr) in buffer */
  unsigned char *buf;	/* start of buffer (common to read and write) */
  int fd;		/* file descriptor as returned by open() */
  int flags;		/* access mode, etc. */
} FILE;

#define fileno(stream)	((stream)->fd)

#define _STRM_RD	0x01	/* stream open for reading */
#define _STRM_WR	0x02	/* stream open for writing */
#define _STRM_RDWR	0x04	/* stream open for reading and writing */
#define _STRM_NOBUF	0x08	/* stream is unbuffered */
#define _STRM_MYBUF	0x10	/* stream has allocated a buffer */
#define _STRM_EOF	0x20	/* program tried to read past end of file */
#define _STRM_ERR	0x40	/* error occurred while reading or writing */
#define _STRM_STRG	0x80	/* I/O is in fact operating on a string */


extern FILE _files[FOPEN_MAX];


/*
 * file operations
 */

#define L_tmpnam	15

#define _IONBF		0
#define _IOLBF		1
#define _IOFBF		2

FILE *fopen(const char *filename, const char *mode);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
int fflush(FILE *stream);
int fclose(FILE *stream);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
FILE *tmpfile(void);
char *tmpnam(char *s);
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);


/*
 * formatted output
 */

int vfprintf(FILE *stream, const char *format, va_list ap);
int fprintf(FILE *stream, const char *format, ...);
int vprintf(const char *format, va_list ap);
int printf(const char *format, ...);
int vsprintf(char *s, const char *format, va_list ap);
int sprintf(char *s, const char *format, ...);


/*
 * formatted input
 */

int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *s, const char *format, ...);


/*
 * character input and output
 */

#define getc(stream) \
	(--(stream)->cnt >= 0 ? \
	  (int) (*(stream)->ptr++) : \
	  _fillBuffer(stream))
#define getchar()	getc(stdin)

#define putc(c, stream) \
	(--(stream)->cnt >= 0 ? \
	  (int) (*(stream)->ptr++ = (c)) : \
	  _flushBuffer(c, stream))
#define putchar(c)	putc((c), stdout)

int _fillBuffer(FILE *stream);
int _flushBuffer(int c, FILE *stream);

int fgetc(FILE *stream);
char *fgets(char *s, int n, FILE *stream);
char *gets(char *s);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int puts(const char *s);
int ungetc(int c, FILE *stream);


/*
 * direct input and output
 */

size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream);


/*
 * file positioning
 */

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

typedef long fpos_t;

int fseek(FILE *stream, long offset, int origin);
long ftell(FILE *stream);
void rewind(FILE *stream);
int fgetpos(FILE *stream, fpos_t *ptr);
int fsetpos(FILE *stream, const fpos_t *ptr);


/*
 * error handling
 */

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);


#endif /* _STDIO_H_ */
