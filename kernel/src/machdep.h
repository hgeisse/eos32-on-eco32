/*
 * machdep.h -- machine dependent parts of kernel
 */


#ifndef _MACHDEP_H_
#define _MACHDEP_H_


#define TIMER_BASE	((unsigned *) 0xF0000000)  /* timer base address */
#define TIMER_CTRL	(TIMER_BASE + 0)	/* timer control register */
#define TIMER_DIV	(TIMER_BASE + 1)	/* timer divisor register */
#define TIMER_IRQ	14			/* timer irq priority */
#define CLKS_PER_TICK	(50000000 / HZ)		/* time between ticks */


void initInterrupts(void);

int getMemorySize(void);

unsigned int spl0(void);
unsigned int spl1(void);
unsigned int spl4(void);
unsigned int spl5(void);
unsigned int spl6(void);
unsigned int spl7(void);
void splx(unsigned int mask);

void clockInit(void);

void sendsig(int signo, caddr_t fun, caddr_t ret);
void sigret(void);


#endif /* _MACHDEP_H_ */
