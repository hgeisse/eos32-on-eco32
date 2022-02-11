/*
 * sys.h -- indirect driver for controlling tty
 */

#ifndef _SYS_H_
#define _SYS_H_

void syopen(dev_t dev, int flag);
void syclose(dev_t dev, int flag);
void syread(dev_t dev);
void sywrite(dev_t dev);
void syioctl(dev_t dev, int cmd, caddr_t addr, int flag);

#endif /* _SYS_H_ */
