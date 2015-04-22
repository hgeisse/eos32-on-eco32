/*
 * sys2.h -- system calls, part 2
 */

#ifndef _SYS2_H_
#define _SYS2_H_

void read(void);
void write(void);
void rdwr(int mode);
void open(void);
void creat(void);
void open1(struct inode *ip, int mode, int trf);
void close(void);
void seek(void);
void link(void);
void mknod(void);
void saccess(void);

#endif /* _SYS2_H_ */
