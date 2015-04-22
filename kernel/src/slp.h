/*
 * slp.h -- process management
 */

#ifndef _SLP_H_
#define _SLP_H_

extern Bool debugSleep;
extern Bool debugWakeup;
extern Bool debugSetpri;
extern Bool debugSched;
extern Bool debugSwtch;
extern Bool debugNewproc;
extern Bool debugExpand;

extern struct proc proc[NPROC];

void showProcTable(char *header);
void sleep(caddr_t chan, int pri);
void wakeup(caddr_t chan);
void setrq(struct proc *p);
void setrun(struct proc *p);
int setpri(struct proc *pp);
void sched(void);
int swapin(struct proc *p);
void qswtch(void);
void swtch(void);
int newproc(void);
void expand(int newsize);

#endif /* _SLP_H_ */
