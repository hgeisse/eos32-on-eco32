/*
 * pipe.h -- implementation of pipes
 */


#ifndef _PIPE_H_
#define _PIPE_H_


extern Bool debugPlock;
extern Bool debugPrele;


void pipe(void);
void readp(struct file *fp);
void writep(struct file *fp);
void plock(struct inode *ip);
void prele(struct inode *ip);


#endif /* _PIPE_H_ */
