/*
 * file.h -- structure of the file table
 */


#ifndef _FILE_H_
#define _FILE_H_


/*
 * One file structure is allocated
 * for each open/creat/pipe call.
 * Main use is to hold the read/write
 * pointer associated with each open
 * file.
 */
struct file {
  char		f_flag;		/* mode flags */
  char		f_count;	/* reference count */
  struct inode *f_inode;	/* pointer to inode structure */
  off_t		f_offset;	/* read/write character pointer */
};


/*
 * mode flags in file structure
 */
#define	FREAD	01
#define	FWRITE	02
#define	FPIPE	04


extern struct file file[];	/* the file table itself */


#endif /* _FILE_H_ */
