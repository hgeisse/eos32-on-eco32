/*
 * sys4.h -- system calls, part 4
 */

#ifndef _SYS4_H_
#define _SYS4_H_

void gtime(void);
void ftime(void);
void stime(void);
void setuid(void);
void getuid(void);
void setgid(void);
void getgid(void);
void getpid(void);
void sync(void);
void nice(void);
void unlink(void);
void chdir(void);
void chroot(void);
void chdirec(struct inode **ipp);
void chmod(void);
void chown(void);
void ssig(void);
void kill(void);
void times(void);
void profil(void);
void alarm(void);
void pause(void);
void umask(void);
void utime(void);

#endif /* _SYS4_H_ */
