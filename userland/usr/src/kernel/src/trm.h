/*
 * trm.h -- terminal driver
 */


#ifndef _TRM_H_
#define _TRM_H_


void trmOpen(dev_t dev, int flag);
void trmClose(dev_t dev, int flag);
void trmRead(dev_t dev);
void trmWrite(dev_t dev);
void trmIoctl(dev_t dev, int cmd, caddr_t addr, int flag);

void terminalOut(char c);


#endif /* _TRM_H_ */
