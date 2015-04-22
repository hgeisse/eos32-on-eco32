/*
 * stat.h -- structure holding information about a file
 */


#ifndef _STAT_H_
#define _STAT_H_


/*
 * stat structure
 */
struct stat {
  dev_t		st_dev;
  ino_t		st_ino;
  int		st_mode;
  int		st_nlink;
  int		st_uid;
  int		st_gid;
  dev_t		st_rdev;
  off_t		st_size;
  time_t	st_atime;
  time_t	st_mtime;
  time_t	st_ctime;
};


/*
 * modes
 */
#define S_IFMT		0070000		/* type of file */
#define   S_IFREG	0040000		/* regular */
#define   S_IFDIR	0030000		/* directory */
#define   S_IFCHR	0020000		/* character special */
#define   S_IFBLK	0010000		/* block special */
#define S_ISUID		0004000		/* set user id on execution */
#define S_ISGID		0002000		/* set group id on execution */
#define S_ISVTX		0001000		/* save swapped text even after use */
#define S_IREAD		0000400		/* read permission, owner */
#define S_IWRITE	0000200		/* write permission, owner */
#define S_IEXEC		0000100		/* execute/search permission, owner */


#endif /* _STAT_H_ */
