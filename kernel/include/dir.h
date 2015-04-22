/*
 * dir.h -- structure of a directory entry
 */


#ifndef _DIR_H_
#define _DIR_H_


#define	DIRSIZ	60		/* max length of a path name component */


struct direct {
  ino_t		d_ino;			/* inode */
  char		d_name[DIRSIZ];		/* name */
};


#endif /* _DIR_H_ */
