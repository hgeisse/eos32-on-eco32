/*
 * nami.h -- convert path name to inode
 */

#ifndef _NAMI_H_
#define _NAMI_H_

extern Bool debugNamei;

struct inode *namei(int (*func)(void), int flag);
int schar(void);
int uchar(void);

#endif /* _NAMI_H_ */
