/*
 * fio.c -- file I/O
 */


#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/systm.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/filsys.h"
#include "../include/file.h"
#include "../include/conf.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/reg.h"
#include "../include/acct.h"

#include "klib.h"
#include "txt.h"
#include "slp.h"
#include "pipe.h"
#include "fio.h"
#include "iget.h"
#include "alloc.h"
#include "nami.h"


Bool debugGetf = FALSE;
Bool debugClosef = FALSE;
Bool debugOpeni = FALSE;
Bool debugAccess = FALSE;
Bool debugOwner = FALSE;
Bool debugSuser = FALSE;
Bool debugUfalloc = FALSE;
Bool debugFalloc = FALSE;


/*
 * Convert a user supplied
 * file descriptor into a pointer
 * to a file structure.
 * Only task is to check range
 * of the descriptor.
 */
struct file *getf(int f) {
  struct file *fp;

  if (debugGetf) {
    printf("-----  getf f = %d  -----\n", f);
  }
  if (0 <= f && f < NOFILE) {
    fp = u.u_ofile[f];
    if (fp != NULL) {
      if (debugGetf) {
        printf("       getf = 0x%08X\n", fp);
      }
      return fp;
    }
  }
  u.u_error = EBADF;
  if (debugGetf) {
    printf("       getf = NULL\n");
  }
  return NULL;
}


/*
 * Internal form of close.
 * Decrement reference count on
 * file structure.
 * Also make sure the pipe protocol
 * does not constipate.
 *
 * Decrement reference count on the inode following
 * removal to the referencing file structure.
 * Call device handler on last close.
 */
void closef(struct file *fp) {
  struct inode *ip;
  int flag, mode;
  dev_t dev;
  void (*cfunc)(dev_t, int);

  if (debugClosef) {
    printf("-----  closef fp = 0x%08X  -----\n", fp);
  }
	if (fp == NULL) {
		return;
	}
	if (fp->f_count > 1) {
		fp->f_count--;
		return;
	}
	ip = fp->f_inode;
	flag = fp->f_flag;
	dev = (dev_t)ip->i_un.i_s2.i_rdev;
	mode = ip->i_mode;

	plock(ip);
	fp->f_count = 0;
	if(flag & FPIPE) {
		ip->i_mode &= ~(IREAD | IWRITE);
		wakeup((caddr_t)ip+1);
		wakeup((caddr_t)ip+2);
	}
	iput(ip);

	switch (mode & IFMT) {

	case IFCHR:
		cfunc = cdevsw[major(dev)].d_close;
		break;

	case IFBLK:
		cfunc = bdevsw[major(dev)].d_close;
		break;
	default:
		return;
	}

	for (fp=file; fp < &file[NFILE]; fp++)
		if (fp->f_count && fp->f_inode==ip)
			return;
	(*cfunc)(dev, flag);
}


/*
 * openi called to allow handler
 * of special files to initialize and
 * validate before actual I/O.
 */
void openi(struct inode *ip, int rw) {
  dev_t dev;
  unsigned int maj;

  if (debugOpeni) {
    printf("-----  openi ip = 0x%08X, rw = %d  -----\n", ip, rw);
  }
	dev = (dev_t)ip->i_un.i_s2.i_rdev;
	maj = major(dev);
	switch (ip->i_mode & IFMT) {

	case IFCHR:
		if(maj >= nchrdev)
			goto bad;
		(*cdevsw[maj].d_open)(dev, rw);
		break;

	case IFBLK:
		if(maj >= nblkdev)
			goto bad;
		(*bdevsw[maj].d_open)(dev, rw);
	}
	return;

bad:
	u.u_error = ENXIO;
}


/*
 * Check mode permission on inode pointer.
 * Mode is READ, WRITE or EXEC.
 * In the case of WRITE, the
 * read-only status of the file
 * system is checked.
 * Also in WRITE, prototype text
 * segments cannot be written.
 * The mode is shifted to select
 * the owner/group/other fields.
 * The super user is granted all
 * permissions.
 */
int access(struct inode *ip, int mode) {
  int m;

  if (debugAccess) {
    printf("-----  access ip = 0x%08X, mode = %d  -----\n", ip, mode);
  }
	m = mode;
	if (m == IWRITE) {
		if (getfs(ip->i_dev)->s_ronly != 0) {
			u.u_error = EROFS;
			if (debugAccess) {
			  printf("       access = NOT OK\n");
			}
			return 1;
		}
		if (ip->i_flag & ITEXT) {
			/* try to free text */
			xrele(ip);
		}
		if (ip->i_flag & ITEXT) {
			u.u_error = ETXTBSY;
			if (debugAccess) {
			  printf("       access = NOT OK\n");
			}
			return 1;
		}
	}
	if (u.u_uid == 0) {
		if (debugAccess) {
		  printf("       access = OK\n");
		}
		return 0;
	}
	if (u.u_uid != ip->i_uid) {
		m >>= 3;
		if (u.u_gid != ip->i_gid) {
			m >>= 3;
		}
	}
	if ((ip->i_mode & m) != 0) {
		if (debugAccess) {
		  printf("       access = OK\n");
		}
		return 0;
	}

	u.u_error = EACCES;
	if (debugAccess) {
	  printf("       access = NOT OK\n");
	}
	return 1;
}


/*
 * Look up a pathname and test if
 * the resultant inode is owned by the
 * current user.
 * If not, try for super-user.
 * If permission is granted,
 * return inode pointer.
 */
struct inode *owner(char *fname) {
  struct inode *ip;

  if (debugOwner) {
    printf("-----  owner  -----\n");
  }
  u.u_dirp = fname;
  ip = namei(uchar, 0);
  if (ip == NULL) {
    if (debugOwner) {
      printf("       owner = NULL\n");
    }
    return NULL;
  }
  if (u.u_uid == ip->i_uid) {
    if (debugOwner) {
      printf("       owner = 0x%08X\n", ip);
    }
    return ip;
  }
  if (suser()) {
    if (debugOwner) {
      printf("       owner = 0x%08X\n", ip);
    }
    return ip;
  }
  iput(ip);
  if (debugOwner) {
    printf("       owner = NULL\n");
  }
  return NULL;
}


/*
 * Test if the current user is the
 * super user.
 */
int suser(void) {
  if (debugSuser) {
    printf("-----  suser  -----\n");
  }
  if (u.u_uid == 0) {
    u.u_acflag |= ASU;
    if (debugSuser) {
      printf("       suser = OK\n");
    }
    return 1;
  }
  u.u_error = EPERM;
  if (debugSuser) {
    printf("       suser = NOT OK\n");
  }
  return 0;
}


/*
 * Allocate a user file descriptor.
 */
int ufalloc(void) {
  int i;

  if (debugUfalloc) {
    printf("-----  ufalloc  -----\n");
  }
	for (i=0; i<NOFILE; i++)
		if (u.u_ofile[i] == NULL) {
			u.u_r.r_val.r_val1 = i;
			u.u_pofile[i] = 0;
			if (debugUfalloc) {
			  printf("       ufalloc = %d\n", i);
			}
			return i;
		}
	u.u_error = EMFILE;
	if (debugUfalloc) {
	  printf("       ufalloc = -1\n");
	}
	return -1;
}


/*
 * Allocate a user file descriptor
 * and a file structure.
 * Initialize the descriptor
 * to point at the file structure.
 *
 * no file -- if there are no available
 * 	file structures.
 */
struct file *falloc(void) {
  struct file *fp;
  int i;

  if (debugFalloc) {
    printf("-----  falloc  -----\n");
  }
	i = ufalloc();
	if (i < 0) {
		if (debugFalloc) {
		  printf("       falloc = NULL\n");
		}
		return NULL;
	}
	for (fp = &file[0]; fp < &file[NFILE]; fp++)
		if (fp->f_count == 0) {
			u.u_ofile[i] = fp;
			fp->f_count++;
			fp->f_offset = 0;
			if (debugFalloc) {
			  printf("       falloc = 0x%08X\n", fp);
			}
			return fp;
		}
	printf("no file\n");
	u.u_error = ENFILE;
	if (debugFalloc) {
	  printf("       falloc = NULL\n");
	}
	return NULL;
}
