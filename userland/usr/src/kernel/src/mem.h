/*
 * mem.h -- memory devices
 */

#ifndef _MEM_H_
#define _MEM_H_

void mmopen(dev_t dev, int flag);
void mmclose(dev_t dev, int flag);
void mmread(dev_t dev);
void mmwrite(dev_t dev);

#endif /* _MEM_H_ */
