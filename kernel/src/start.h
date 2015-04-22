/*
 * start.h -- startup and assembler support code
 */


#ifndef _START_H_
#define _START_H_


typedef struct {
  unsigned int ic_reg[32];	/* general purpose registers */
  unsigned int ic_psw;		/* PSW */
  unsigned int ic_tlbHi;	/* TLB EntryHi */
} InterruptContext;

typedef void (*ISR)(int irq, InterruptContext *icp);


#define USERMODE(icp)	(((icp)->ic_psw & (1 << 25)) != 0)
#define BASEPRI(icp)	(((icp)->ic_psw & 0xFFFF) != 0xFFFF)


extern caddr_t proc0uarea;
extern caddr_t allocStart;


void idle(void);

Bool save(label_t regs);
void resume(int addr, label_t regs);

void savfp(struct fps *fpsp);
void restfp(struct fps *fpsp);

void flushTLB(void);
void setTLB(int index, unsigned int entryHi, unsigned int entryLo);

Bool enableInt(void);
Bool disableInt(void);
void restoreInt(Bool enable);

unsigned int getMask(void);
unsigned int setMask(unsigned int mask);

ISR getISR(int irq);
ISR setISR(int irq, ISR isr);


#endif /* _START_H_ */
