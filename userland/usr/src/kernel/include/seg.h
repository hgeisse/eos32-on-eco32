/*
 * seg.h -- KT-11 addresses and bits
 */

#ifndef _SEG_H_
#define _SEG_H_

#define	UISD	((physadr)0177600)	/* first user I-space descriptor register */
#define	UISA	((physadr)0177640)	/* first user I-space address register */
#define	UDSA	((physadr)0177660)	/* first user D-space address register */

//#define	RO	02		/* access abilities */
//#define	RW	06
#define RO	FALSE		/* access is read only */
#define RW	TRUE		/* access is read/write */

#define	ED	010		/* extend direction */
#define	TX	020		/* Software: text segment */
#define	ABS	040		/* Software: absolute address */

#endif /* _SEG_H_ */
