/*
 * trap.h -- traps and other exceptions
 */


#ifndef _TRAP_H_
#define _TRAP_H_


#define EXC_BUS_TIMEOUT	16	/* bus timeout exception */
#define EXC_ILL_INSTRCT	17	/* illegal instruction exception */
#define EXC_PRV_INSTRCT	18	/* privileged instruction exception */
#define EXC_DIVIDE	19	/* divide instruction exception */
#define EXC_TRAP	20	/* trap instruction exception */
#define EXC_TLB_MISS	21	/* TLB miss exception */
#define EXC_TLB_WRITE	22	/* TLB write exception */
#define EXC_TLB_INVALID	23	/* TLB invalid exception */
#define EXC_ILL_ADDRESS	24	/* illegal address exception */
#define EXC_PRV_ADDRESS	25	/* privileged address exception */


extern Bool debugSyscall;
extern Bool debugTrap;


void trapInit(void);
void trapISR(int tn, InterruptContext *icp);


#endif /* _TRAP_H_ */
