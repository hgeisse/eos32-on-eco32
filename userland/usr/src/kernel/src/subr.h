/*
 * subr.h -- common subroutines
 */

#ifndef _SUBR_H_
#define _SUBR_H_

extern Bool debugBmap;
extern Bool debugBcopy;

daddr_t bmap(struct inode *ip, daddr_t bn, int rwflg);
int passc(int c);
int cpass(void);
void bcopy(caddr_t from, caddr_t to, int count);

#endif /* _SUBR_H_ */
