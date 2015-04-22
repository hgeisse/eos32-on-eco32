/*
 * prim.h -- primitive character list handling
 */


#ifndef _PRIM_H_
#define _PRIM_H_


extern Bool debugCinit;


int getc(struct clist *p);
int q_to_b(struct clist *q, char *cp, int cc);
int ndqb(struct clist *q, int flag);
void ndflush(struct clist *q, int cc);
int putc(int c, struct clist *p);
int b_to_q(char *cp, int cc, struct clist *q);
void cinit(void);


#endif /* _PRIM_H_ */
