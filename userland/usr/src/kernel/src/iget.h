/*
 * iget.h -- inode handling
 */

#ifndef _IGET_H_
#define _IGET_H_

extern Bool debugIget;
extern Bool debugIput;
extern Bool debugIupdat;
extern Bool debugItrunc;
extern Bool debugMaknode;
extern Bool debugWdir;

struct inode *iget(dev_t dev, ino_t ino);
void iput(struct inode *ip);
void iupdat(struct inode *ip, time_t *ta, time_t *tm);
void itrunc(struct inode *ip);
struct inode *maknode(int mode);
void wdir(struct inode *ip);

#endif /* _IGET_H_ */
