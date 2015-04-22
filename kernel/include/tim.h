/*
 * tim.h -- types related to time
 */

#ifndef _TIM_H_
#define _TIM_H_

typedef long time_t;

struct timeb {
	time_t	time;
	unsigned short millitm;
	short	timezone;
	short	dstflag;
};

#endif /* _TIM_H_ */
