#ifndef __PR_H_INCLUDED__
#define __PR_H_INCLUDED__

/* PROTOTYPES */
void done(void);
void onintr(int x);
void fixtty(void);
void print(char* fp, char** argp);
void mopen(char** ap);
void putpage(void);
void nexbuf(void);
int tpgetc(int ai);
int pgetc(int i);
void put(int ac);
void putcp(int c);

#endif 
