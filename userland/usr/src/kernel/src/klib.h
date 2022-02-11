/*
 * klib.h -- the kernel library
 */


#ifndef _KLIB_H_
#define _KLIB_H_


extern Bool debugCopyFrame;
extern Bool debugClearFrame;
extern Bool debugCopyIn;
extern Bool debugCopyOut;
extern Bool debugFubyte;
extern Bool debugFuword;
extern Bool debugSubyte;
extern Bool debugSuword;
extern Bool outputToLogfile;
extern Bool outputToConsole;
extern Bool outputToTerminal;


void debugBreak(int breakNumber);

void copyFrame(int src, int dst);
void clearFrame(int dst);

Bool copyIn(caddr_t src, caddr_t dst, int cnt);
Bool copyOut(caddr_t src, caddr_t dst, int cnt);

int fubyte(caddr_t uaddr);
int fuword(caddr_t uaddr);
int subyte(caddr_t uaddr, unsigned int val);
int suword(caddr_t uaddr, unsigned int val);

int strlen(char *s);
void strcpy(char *dst, char *src);

void printf(char *fmt, ...);

void panic(char *s);
void prdev(char *str, dev_t dev);
void deverror(dev_t dev, daddr_t blkno, unsigned int x);


#endif /* _KLIB_H_ */
