/*
 * inode.h -- structure of an inode in memory
 */


#ifndef _INODE_H_
#define _INODE_H_


/*
 * The I node is the focus of all
 * file activity in unix. There is a unique
 * inode allocated for each active file,
 * each current directory, each mounted-on
 * file, text file, and the root. An inode is 'named'
 * by its dev/inumber pair. (iget/iget.c)
 * Data, from mode on, is read in
 * from permanent inode on volume.
 */
struct inode {
  char	i_flag;		/* various flags */
  char	i_count;	/* reference count */
  dev_t	i_dev;		/* device where inode resides */
  ino_t	i_number;	/* inode number on device */
  int	i_mode;		/* type and mode of file */
  int	i_nlink;	/* directory entries */
  int	i_uid;		/* owner */
  int	i_gid;		/* group of owner */
  off_t	i_size;		/* size of file */
  union {
    struct {
      daddr_t i_addr[NADDR];	/* if normal file/directory */
      daddr_t i_lastr;		/* last logical block read (for read-ahead) */
    } i_s1;
    struct {
      daddr_t i_rdev;		/* i_addr[0] */
    } i_s2;
  } i_un;
};


/*
 * flags
 */
#define	ILOCK	01		/* inode is locked */
#define	IUPD	02		/* file has been modified */
#define	IACC	04		/* inode access time to be updated */
#define	IMOUNT	010		/* inode is mounted on */
#define	IWANT	020		/* some process waiting on lock */
#define	ITEXT	040		/* inode is pure text prototype */
#define	ICHG	0100		/* inode has been changed */


extern struct inode inode[];	/* the inode table itself */


#endif /* _INODE_H_ */
