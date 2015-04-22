/*
 * sig.h -- signal handling
 */

#ifndef _SIG_H_
#define _SIG_H_

extern Bool debugPsignal;
extern Bool debugGrow;

void signal(int pgrp, int sig);
void psignal(struct proc *p, int sig);
int issig(void);
void stop(void);
void psig(void);
int fsig(struct proc *p);
int core(void);
int grow(unsigned sp);
void ptrace(void);
int procxmt(void);

#endif /* _SIG_H_ */
