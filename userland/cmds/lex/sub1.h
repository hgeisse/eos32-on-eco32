/*
 * sub1.h
 */


#ifndef _SUB1_H_
#define _SUB1_H_


char *getl(char *p);
int space(int ch);
int digit(int c);
void error(char *s, ...);
void warning(char *s, ...);
int myindex(int a, char *s);
int alpha(int c);
int printable(int c);
void lgate(void);
void scopy(char *s, char *t);
int siconv(char *t);
int slength(char *s);
int scomp(char *x, char *y);
int ctrans(char **ss);
void cclinter(int sw);
int usescape(int c);
int lookup(char *s, char **t);
int cpyact(void);
int gch(void);
int mn2(int a, int d, int c);
int mn1(int a, int d);
int mn0(int a);
void munput(int t, int c, char *p);
int dupl(int n);
void allprint(char c);
void strpt(char *s);
void sect1dump(void);
void sect2dump(void);
void treedump(void);


#endif /* _SUB1_H_ */
