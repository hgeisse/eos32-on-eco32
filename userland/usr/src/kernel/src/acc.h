/*
 * acc.h -- do process accounting
 */

#ifndef _ACC_H_
#define _ACC_H_

void sysacct(void);
void acct(void);
int compress(time_t t);

#endif /* _ACC_H_ */
