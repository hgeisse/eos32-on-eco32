/*
 * clock.h -- clock tick handling
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

extern time_t time;

void addupc(caddr_t a1, struct prof *a2, int a3);
void clock(InterruptContext *icp);
void timeout(void (*fun)(caddr_t), caddr_t arg, int tim);

#endif /* _CLOCK_H_ */
