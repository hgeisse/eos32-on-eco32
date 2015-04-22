/*
 * errno.h -- error numbers
 */


#ifndef _ERRNO_H_
#define _ERRNO_H_


#define ENOERR		0	/* no error */
#define EPERM		1	/* not owner */
#define ENOENT		2	/* no such file or directory */
#define ESRCH		3	/* no such process */
#define EINTR		4	/* interrupted system call */
#define EIO		5	/* I/O error */
#define ENXIO		6	/* no such device or address */
#define E2BIG		7	/* argument list too long */
#define ENOEXEC		8	/* exec format error */
#define EBADF		9	/* bad file descriptor */
#define ECHILD		10	/* no children */
#define EAGAIN		11	/* no more processes */
#define ENOMEM		12	/* not enough memory */
#define EACCES		13	/* permission denied */
#define EFAULT		14	/* bad address */
#define ENOTBLK		15	/* block device required */
#define EBUSY		16	/* mount device busy */
#define EEXIST		17	/* file exists */
#define EXDEV		18	/* cross-device link */
#define ENODEV		19	/* no such device */
#define ENOTDIR		20	/* not a directory */
#define EISDIR		21	/* is a directory */
#define EINVAL		22	/* invalid argument */
#define ENFILE		23	/* file table overflow */
#define EMFILE		24	/* too many open files */
#define ENOTTY		25	/* not a terminal */
#define ETXTBSY		26	/* text file busy */
#define EFBIG		27	/* file too large */
#define ENOSPC		28	/* no space left on device */
#define ESPIPE		29	/* illegal seek */
#define EROFS		30	/* read-only file system */
#define EMLINK		31	/* too many links */
#define EPIPE		32	/* broken pipe */
#define EDOM		33	/* math argument */
#define ERANGE		34	/* result too large */


extern int errno;


#endif /* _ERRNO_H_ */
