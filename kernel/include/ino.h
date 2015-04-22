/*
 * ino.h -- structure of an inode on disk
 */


#ifndef _INO_H_
#define _INO_H_


/*
 * inode modes
 */
#define IFMT	070000		/* type of file */
#define   IFREG	040000		/* regular */
#define   IFDIR	030000		/* directory */
#define   IFCHR	020000		/* character special */
#define   IFBLK	010000		/* block special */
#define ISUID	004000		/* set user id on execution */
#define ISGID	002000		/* set group id on execution */
#define ISVTX	001000		/* save swapped text even after use */
#define IREAD	000400		/* read, write, execute permissions */
#define IWRITE	000200
#define IEXEC	000100


/*
 * disk block addresses in inode
 */
#define NADDR		8	/* number of block addresses in inode */
#define NDADDR		6	/* number of direct block addresses */
#define SINGLE_INDIR	(NDADDR + 0)	/* index of single indirect block */
#define DOUBLE_INDIR	(NDADDR + 1)	/* index of double indirect block */


/*
 * number of inodes per block
 */
#define INOPB		(BSIZE / sizeof(struct dinode))


/*
 * inode structure as it appears on a disk block
 */
struct dinode {
  int		di_mode;	/* type and mode of file */
  int		di_nlink;	/* number of links to file */
  int		di_uid;		/* owner's user id */
  int		di_gid;		/* owner's group id */
  time_t	di_ctime;	/* time created */
  time_t	di_mtime;	/* time last modified */
  time_t	di_atime;	/* time last accessed */
  off_t		di_size;	/* number of bytes in file */
  daddr_t	di_addr[NADDR];	/* disk block addresses */
};


/*
 * The following two macros convert an inode number to the disk
 * block containing the inode and an inode number within the block.
 */
#define itod(i)		(2 + (i) / INOPB)
#define itoo(i)		((i) % INOPB)


/*
 * representation of an inode number
 */
typedef int ino_t;


#endif /* _INO_H_ */
