/*
 * tt.h -- general TTY subroutines
 */

#ifndef _TT_H_
#define _TT_H_

void ttyopen(dev_t dev, struct tty *tp);
void ttychars(struct tty *tp);
void ttyclose(struct tty *tp);
void ioctl(void);
int ttioccomm(int com, struct tty *tp, caddr_t addr, dev_t dev);
void wflushtty(struct tty *tp);
void flushtty(struct tty *tp);
int canon(struct tty *tp);
void ttyrend(struct tty *tp, char *pb, char *pe);
void ttyinput(int c, struct tty *tp);
void ttyblock(struct tty *tp);
void ttyoutput(int c, struct tty *tp);
void ttrstrt(struct tty *tp);
void ttstart(struct tty *tp);
int ttread(struct tty *tp);
caddr_t ttwrite(struct tty *tp);

#endif /* _TT_H_ */
