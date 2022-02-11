/*
 * dev.h -- device data type
 */


#ifndef _DEV_H_
#define _DEV_H_


/* major part of a device */
#define	major(x)	((int) ((unsigned) (x) >> 16))

/* minor part of a device */
#define	minor(x)	((int) ((unsigned) (x) & 0xFFFF))

/* make a device number */
#define	makedev(x,y)	((dev_t) ((x) << 16 | (y)))


/* representation of a device */
typedef int dev_t;


#endif /* _DEV_H_ */
