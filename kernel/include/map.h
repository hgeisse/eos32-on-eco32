/*
 * map.h -- structure used for mapped allocation
 */

#ifndef _MAP_H_
#define _MAP_H_

struct map {
  int m_size;	/* size of free area in units of this map, 0: sentinel */
  int m_addr;	/* base address of free area in units of this map */
};

#endif /* _MAP_H_ */
