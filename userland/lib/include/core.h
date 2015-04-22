/*
 * core.h -- machine dependent stuff for core files
 */


#ifndef _CORE_H_
#define _CORE_H_


#define TXTRNDSIZ	4096
#define stacktop(siz)	(0x80000000)
#define stackbas(siz)	(0x80000000 - (siz))


#endif /* _CORE_H_ */
