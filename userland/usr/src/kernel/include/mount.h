/*
 * mount.h -- structure of the mount table
 */

#ifndef _MOUNT_H_
#define _MOUNT_H_

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	mount
{
	dev_t	m_dev;		/* device mounted */
	struct buf *m_bufp;	/* pointer to superblock */
	struct inode *m_inodp;	/* pointer to mounted on inode */
};

extern struct mount mount[NMOUNT];

#endif /* _MOUNT_H_ */
