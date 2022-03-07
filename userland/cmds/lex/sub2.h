/*
 * sub2.h
 */


#ifndef _SUB2_H_
#define _SUB2_H_


void cfoll(int v);
void pfoll(void);
void add(int **array, int n);
void follow(int v);
void first(int v);
void cgoto(void);
void nextstate(int s, int c);
int notin(int n);
void packtrans(int st, char *tch, int *tst, int cnt, int tryit);
void pstate(int s);
int member(int d, char *t);
void stprt(int i);
void acompute(int s);
void pccl(void);
void mkmatch(void);
void layout(void);
void rprint(int *a, char *s, int n);
void shiftr(int *a, int n);
void upone(int *a, int n);
void bprint(char *a, char *s, int n);
void padd(int **array, int n);


#endif /* _SUB2_H_ */
