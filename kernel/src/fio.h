/*
 * fio.h -- file I/O
 */

#ifndef _FIO_H_
#define _FIO_H_

extern Bool debugGetf;
extern Bool debugClosef;
extern Bool debugOpeni;
extern Bool debugAccess;
extern Bool debugOwner;
extern Bool debugSuser;
extern Bool debugUfalloc;
extern Bool debugFalloc;

struct file *getf(int f);
void closef(struct file *fp);
void openi(struct inode *ip, int rw);
int access(struct inode *ip, int mode);
struct inode *owner(char *fname);
int suser(void);
int ufalloc(void);
struct file *falloc(void);

#endif /* _FIO_H_ */
