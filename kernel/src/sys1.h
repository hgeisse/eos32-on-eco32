/*
 * sys1.h -- system calls, part 1
 */

#ifndef _SYS1_H_
#define _SYS1_H_

void getadr(void);
void exece(void);
void getxfile(struct inode *ip, int nargc);
void setregs(int sp);
void rexit(void);
void exit(int rv);
void wait(void);
void fork(void);
void sbreak(void);

#endif /* _SYS1_H_ */
