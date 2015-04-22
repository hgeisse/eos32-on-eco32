/*
 * setjmp.h -- non-local jumps
 */


#ifndef _SETJMP_H_
#define _SETJMP_H_


typedef unsigned int jmp_buf[10];


int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);


#endif /* _SETJMP_H_ */
