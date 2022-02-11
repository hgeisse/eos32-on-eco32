/*
 * sys3.h -- system calls, part 3
 */

#ifndef _SYS3_H_
#define _SYS3_H_

void fstat(void);
void stat(void);
void stat1(struct inode *ip, struct stat *ub, off_t pipeadj);
void dup(void);
void smount(void);
void sumount(void);
dev_t getmdev(char *fspec);

#endif /* _SYS3_H_ */
