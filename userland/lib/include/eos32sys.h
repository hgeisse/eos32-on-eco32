/*
 * eos32sys.h -- EOS32 system calls
 */


#ifndef _EOS32SYS_H_
#define _EOS32SYS_H_


#include <sys/dev.h>
#include <sys/blk.h>
#include <sys/off.h>
#include <sys/tim.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/stat.h>


#ifndef NULL
#define NULL	((void *) 0)
#endif


/*
 * process management
 */

void _exit(int status);
int fork(void);
int execve(const char *name, char *const argv[], char *const envp[]);
int wait(int *status);
int brk(void *addr);
void *sbrk(int incr);
int getpid(void);
int getppid(void);
int nice(int incr);
int acct(char *name);
void profil(char *buffer, int bufsiz, int offset, int scale);
int ptrace(int request, int pid, int *addr, int data);
int getadr(int objdes, unsigned *addrp, unsigned *sizep);


/*
 * signals
 */

void (*_signal(int sig, void (*func)(int)))(int);
int kill(int pid, int sig);
unsigned int alarm(unsigned int seconds);
void pause(void);


/*
 * file management
 */

#define O_RDONLY	0		/* open mode: read only */
#define O_WRONLY	1		/* open mode: write only */
#define O_RDWR		2		/* open mode: read and write */

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

int access(const char *name, int mode);
int creat(const char *name, int mode);
int mknod(const char *name, int mode, int dev);
int open(const char *name, int mode);
int close(int fildes);
int read(int fildes, void *buffer, int nbytes);
int write(int fildes, void *buffer, int nbytes);
long lseek(int fildes, long offset, int whence);
int stat(const char *name, struct stat *buffer);
int fstat(int fildes, struct stat *buffer);
int dup(int oldfildes);
int dup2(int oldfildes, int newfildes);
int pipe(int fildes[2]);
int ioctl(int fildes, int request, void *argp);


/*
 * directory and file system management
 */

int link(const char *oldname, const char *newname);
int unlink(const char *name);
int mount(const char *special, const char *name, int rdonly);
int umount(const char *special);
void sync(void);
int chdir(const char *dirname);
int chroot(const char *dirname);


/*
 * protection
 */

int chmod(const char *name, int mode);
int chown(const char *name, int owner, int group);
int getuid(void);
int geteuid(void);
int getgid(void);
int getegid(void);
int setuid(int uid);
int setgid(int gid);
int umask(int complmode);


/*
 * time management
 */

struct tbuffer {
  long proc_user_time;
  long proc_system_time;
  long child_user_time;
  long child_system_time;
};

long _time(long *tp);
int ftime(struct timeb *tp);
int stime(long *tp);
int times(struct tbuffer *buffer);
int utime(const char *file, long timep[2]);


#endif /* _EOS32SYS_H_ */
