/*
 * y1.h
 */


#ifndef _Y1_H_
#define _Y1_H_


void others(void);
char *chcopy(char *p, char *q);
char *writem(int *pp);
char *symnam(int i);
void summary(void);
void error(char *s, ...);
void aryfil(int *v, int n, int c);
int setunion(int *a, int *b);
void prlook(struct looksets *p);
void cpres(void);
void cpfir(void);
int state(int c);
void putitem(int *ptr, struct looksets *lptr);
void cempty(void);
void stagen(void);
void closure(int i);
struct looksets *flset(struct looksets *p);


#endif /* _Y1_H_ */
