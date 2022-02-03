/*
 * lmain.h
 */


#ifndef _LMAIN_H_
#define _LMAIN_H_


void get1core(void);
void free1core(void);
void get2core(void);
void free2core(void);
void get3core(void);
void free3core(void);
void *myalloc(int a, int b);
void buserr(void);
void segviol(void);
void yyerror(char *s);


#endif /* _LMAIN_H_ */
