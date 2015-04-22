/*
 * clock.c -- clock tick handling
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/callo.h"
#include "../include/seg.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/proc.h"
#include "../include/reg.h"

#include "start.h"
#include "klib.h"
#include "slp.h"
#include "sig.h"
#include "clock.h"
#include "machdep.h"

#define	SCHMAG	8/10

time_t time;			/* time in sec from 1970 */
int lbolt;
struct callo callout[NCALL];

/*
 * update profiling data
 */
void addupc(caddr_t a1, struct prof *a2, int a3) {
  panic("addupc() not implemented yet");
}

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	lightning bolt wakeup (every second)
 *	alarm clock signals
 *	jab the scheduler
 */
void clock(InterruptContext *icp) {
	struct callo *p1, *p2;
	struct proc *pp;
	int a;

	/*
	 * callouts
	 * if none, just continue
	 * else update first non-zero time
	 */

	if (callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while (p2->c_time <= 0 && p2->c_func != NULL)
		p2++;
	p2->c_time--;

	/*
	 * if ps is high, just return
	 */

	if (BASEPRI(icp))
		goto out;

	/*
	 * callout
	 */

	spl5();
	if (callout[0].c_time <= 0) {
		p1 = &callout[0];
		while (p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while (p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

out:

	/*
	 * accounting
	 */

	if (USERMODE(icp)) {
		u.u_utime++;
		if (u.u_prof.pr_scale != 0)
			addupc((caddr_t) icp->ic_reg[30], &u.u_prof, 1);
	} else {
		u.u_stime++;
	}

	/*
	 * lightning bolt time-out
	 * and time of day
	 */

	pp = u.u_procp;
	if(++pp->p_cpu == 0)
		pp->p_cpu--;
	if(++lbolt >= HZ) {
		if (BASEPRI(icp))
			return;
		lbolt -= HZ;
		++time;
		spl1();
		runrun++;
		wakeup((caddr_t)&lbolt);
		for(pp = &proc[0]; pp < &proc[NPROC]; pp++)
		if (pp->p_stat && pp->p_stat<SZOMB) {
			if(pp->p_time != 127)
				pp->p_time++;
			if(pp->p_clktim)
				if(--pp->p_clktim == 0)
					psignal(pp, SIGCLK);
			a = (pp->p_cpu & 0377)*SCHMAG + pp->p_nice - NZERO;
			if(a < 0)
				a = 0;
			if(a > 255)
				a = 255;
			pp->p_cpu = a;
			if(pp->p_pri >= PUSER)
				setpri(pp);
		}
		if(runin!=0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
	}
}

/*
 * timeout is called to arrange that
 * fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout
 * structure. The time in each structure
 * entry is the number of HZ's more
 * than the previous entry.
 * In this way, decrementing the
 * first entry has the effect of
 * updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */
void timeout(void (*fun)(caddr_t), caddr_t arg, int tim)
{
	register struct callo *p1, *p2;
	register int t;
	int s;

	t = tim;
	p1 = &callout[0];
	s = spl7();
	while(p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	if (p1 >= &callout[NCALL-1])
		panic("timeout table overflow");
	p1->c_time -= t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	splx(s);
}
