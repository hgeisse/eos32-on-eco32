/*
 * txt.h -- handle shared text segments
 */

#ifndef _TXT_H_
#define _TXT_H_

void xswap(struct proc *p, int ff, int os);
void xfree(void);
void xalloc(struct inode *ip);
void xexpand(struct text *xp);
void xlock(struct text *xp);
void xunlock(struct text *xp);
void xccdec(struct text *xp);
void xumount(dev_t dev);
void xrele(struct inode *ip);
void xuntext(struct text *xp);

#endif /* _TXT_H_ */
