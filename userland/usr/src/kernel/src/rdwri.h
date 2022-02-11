/*
 * rdwri.h -- read and write operations by inode
 */

#ifndef _RDWRI_H_
#define _RDWRI_H_

extern Bool debugReadi;
extern Bool debugWritei;
extern Bool debugIomove;

void readi(struct inode *ip);
void writei(struct inode *ip);
unsigned max(unsigned a, unsigned b);
unsigned min(unsigned a, unsigned b);
void iomove(caddr_t cp, int n, int flag);

#endif /* _RDWRI_H_ */
