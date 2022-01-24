/*
 * y2.h
 */


#ifndef _Y2_H_
#define _Y2_H_


void setup(int argc, char *argv[]);
void finact(void);
int defin(int t, char *s);
void defout(void);
char *cstash(char *s);
int gettok(void);
int fdtype(int t);
int chfind(int t, char *s);
void cpyunion(void);
void cpycode(void);
int skipcom(void);
void cpyact(int offset);


#endif /* _Y2_H_ */
