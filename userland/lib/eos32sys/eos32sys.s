;
; eos32sys.s -- EOS32 system calls
;


	.export	_exit
	.export	fork
	.export	execve
	.export	wait
	.export	brk
	.export	sbrk
	.export	getpid
	.export	getppid
	.export	nice
	.export	acct
	.export	profil
	.export	ptrace
	.export	getadr

	.export	_signal
	.export	kill
	.export	alarm
	.export	pause

	.export	access
	.export	creat
	.export	mknod
	.export	open
	.export	close
	.export	read
	.export	write
	.export	lseek
	.export	stat
	.export	fstat
	.export	dup
	.export	dup2
	.export	pipe
	.export	ioctl

	.export	link
	.export	unlink
	.export	mount
	.export	umount
	.export	sync
	.export	chdir
	.export	chroot

	.export	chmod
	.export	chown
	.export	getuid
	.export	geteuid
	.export	getgid
	.export	getegid
	.export	setuid
	.export	setgid
	.export	umask

	.export	_time
	.export	ftime
	.export	stime
	.export	times
	.export	utime


	.import	errno
	.import	curbrk


	.set	SYS_GETADR,0
	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_READ,3
	.set	SYS_WRITE,4
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_WAIT,7
	.set	SYS_CREAT,8
	.set	SYS_LINK,9
	.set	SYS_UNLINK,10
	.set	SYS_CHDIR,12
	.set	SYS_TIME,13
	.set	SYS_MKNOD,14
	.set	SYS_CHMOD,15
	.set	SYS_CHOWN,16
	.set	SYS_BREAK,17
	.set	SYS_STAT,18
	.set	SYS_SEEK,19
	.set	SYS_GETPID,20
	.set	SYS_MOUNT,21
	.set	SYS_UMOUNT,22
	.set	SYS_SETUID,23
	.set	SYS_GETUID,24
	.set	SYS_STIME,25
	.set	SYS_PTRACE,26
	.set	SYS_ALARM,27
	.set	SYS_FSTAT,28
	.set	SYS_PAUSE,29
	.set	SYS_UTIME,30
	.set	SYS_ACCESS,33
	.set	SYS_NICE,34
	.set	SYS_FTIME,35
	.set	SYS_SYNC,36
	.set	SYS_KILL,37
	.set	SYS_DUP,41
	.set	SYS_PIPE,42
	.set	SYS_TIMES,43
	.set	SYS_PROFIL,44
	.set	SYS_SETGID,46
	.set	SYS_GETGID,47
	.set	SYS_SIGNAL,48
	.set	SYS_SIGRET,49
	.set	SYS_ACCT,51
	.set	SYS_IOCTL,54
	.set	SYS_EXECE,59
	.set	SYS_UMASK,60
	.set	SYS_CHROOT,61


;
; process management
;


	; void _exit(int status)
_exit:
	add	$8,$0,SYS_EXIT
	trap


	; int fork(void)
fork:
	add	$8,$0,SYS_FORK
	trap
	bne	$8,$0,sysError
	jr	$31


	; int execve(const char *name,
	;            char *const argv[],
	;            char *const envp[])
execve:
	add	$8,$0,SYS_EXECE
	trap
	j	sysError


	; int wait(int *status)
wait:
	add	$8,$0,SYS_WAIT
	trap
	bne	$8,$0,sysError
	beq	$4,$0,wait1
	stw	$3,$4,0
wait1:
	jr	$31


	; int brk(void *addr)
brk:
	add	$8,$0,SYS_BREAK
	trap
	bne	$8,$0,sysError
	stw	$4,$0,curbrk
	add	$2,$0,0
	jr	$31


	; void *sbrk(int incr)
sbrk:
	ldw	$5,$0,curbrk
	beq	$4,$0,sbrk1
	add	$4,$4,$5
	add	$8,$0,SYS_BREAK
	trap
	bne	$8,$0,sysError
	stw	$4,$0,curbrk
sbrk1:
	add	$2,$5,$0
	jr	$31


	; int getpid(void)
getpid:
	add	$8,$0,SYS_GETPID
	trap
	jr	$31


	; int getppid(void)
getppid:
	add	$8,$0,SYS_GETPID
	trap
	add	$2,$3,$0
	jr	$31


	; int nice(int incr)
nice:
	add	$8,$0,SYS_NICE
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int acct(char *name)
acct:
	add	$8,$0,SYS_ACCT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; void profil(char *buffer, int bufsiz, int offset, int scale)
profil:
	add	$8,$0,SYS_PROFIL
	trap
	jr	$31


	; int ptrace(int request, int pid, int *addr, int data)
ptrace:
	stw	$0,$0,errno
	add	$8,$0,SYS_PTRACE
	trap
	bne	$8,$0,sysError
	jr	$31


	; int getadr(int objdes, unsigned *addrp, unsigned *sizep)
getadr:
	add	$8,$0,SYS_GETADR
	trap
	bne	$8,$0,sysError
	beq	$5,$0,getadr1
	stw	$2,$5,0
getadr1:
	beq	$6,$0,getadr2
	stw	$3,$6,0
getadr2:
	add	$2,$0,0
	jr	$31


;
; signals
;


	; void (*_signal(int sig, void (*func)(int)))(int)
_signal:
	add	$6,$0,sigret
	add	$8,$0,SYS_SIGNAL
	trap
	bne	$8,$0,sysError
	jr	$31

sigret:
	add	$8,$0,SYS_SIGRET
	trap


	; int kill(int pid, int sig)
kill:
	add	$8,$0,SYS_KILL
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; unsigned int alarm(unsigned int seconds)
alarm:
	add	$8,$0,SYS_ALARM
	trap
	jr	$31


	; void pause(void)
pause:
	add	$8,$0,SYS_PAUSE
	trap
	jr	$31


;
; file management
;


	; int access(const char *name, int mode)
access:
	add	$8,$0,SYS_ACCESS
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int creat(const char *name, int mode)
creat:
	add	$8,$0,SYS_CREAT
	trap
	bne	$8,$0,sysError
	jr	$31


	; int mknod(const char *name, int mode, int dev)
mknod:
	add	$8,$0,SYS_MKNOD
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int open(const char *name, int mode)
open:
	add	$8,$0,SYS_OPEN
	trap
	bne	$8,$0,sysError
	jr	$31


	; int close(int fildes)
close:
	add	$8,$0,SYS_CLOSE
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int read(int fildes, void *buffer, int nbytes)
read:
	add	$8,$0,SYS_READ
	trap
	bne	$8,$0,sysError
	jr	$31


	; int write(int fildes, void *buffer, int nbytes)
write:
	add	$8,$0,SYS_WRITE
	trap
	bne	$8,$0,sysError
	jr	$31


	; long lseek(int fildes, long offset, int whence)
lseek:
	add	$8,$0,SYS_SEEK
	trap
	bne	$8,$0,sysError
	jr	$31


	; int stat(const char *name, struct stat *buffer)
stat:
	add	$8,$0,SYS_STAT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int fstat(int fildes, struct stat *buffer)
fstat:
	add	$8,$0,SYS_FSTAT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int dup(int oldfildes)
dup:
	add	$5,$0,0
	add	$6,$0,0
	add	$8,$0,SYS_DUP
	trap
	bne	$8,$0,sysError
	jr	$31


	; int dup2(int oldfildes, int newfildes)
dup2:
	add	$6,$0,1
	add	$8,$0,SYS_DUP
	trap
	bne	$8,$0,sysError
	jr	$31


	; int pipe(int fildes[2])
pipe:
	add	$8,$0,SYS_PIPE
	trap
	bne	$8,$0,sysError
	stw	$2,$4,0
	stw	$3,$4,4
	add	$2,$0,0
	jr	$31


	; int ioctl(int fildes, int request, void *argp)
ioctl:
	add	$8,$0,SYS_IOCTL
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


;
; directory and file system management
;


	; int link(const char *oldname, const char *newname)
link:
	add	$8,$0,SYS_LINK
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int unlink(const char *name)
unlink:
	add	$8,$0,SYS_UNLINK
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int mount(const char *special, const char *name, int rdonly)
mount:
	add	$8,$0,SYS_MOUNT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int umount(const char *special)
umount:
	add	$8,$0,SYS_UMOUNT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; void sync(void)
sync:
	add	$8,$0,SYS_SYNC
	trap
	jr	$31


	; int chdir(const char *dirname)
chdir:
	add	$8,$0,SYS_CHDIR
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int chroot(const char *dirname)
chroot:
	add	$8,$0,SYS_CHROOT
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


;
; protection
;


	; int chmod(const char *name, int mode)
chmod:
	add	$8,$0,SYS_CHMOD
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int chown(const char *name, int owner, int group)
chown:
	add	$8,$0,SYS_CHOWN
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int getuid(void)
getuid:
	add	$8,$0,SYS_GETUID
	trap
	jr	$31


	; int geteuid(void)
geteuid:
	add	$8,$0,SYS_GETUID
	trap
	add	$2,$3,$0
	jr	$31


	; int getgid(void)
getgid:
	add	$8,$0,SYS_GETGID
	trap
	jr	$31


	; int getegid(void)
getegid:
	add	$8,$0,SYS_GETGID
	trap
	add	$2,$3,$0
	jr	$31


	; int setuid(int uid)
setuid:
	add	$8,$0,SYS_SETUID
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int setgid(int gid)
setgid:
	add	$8,$0,SYS_SETGID
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int umask(int complmode)
umask:
	add	$8,$0,SYS_UMASK
	trap
	bne	$8,$0,sysError
	jr	$31


;
; time management
;


	; long _time(long *tp)
_time:
	add	$8,$0,SYS_TIME
	trap
	beq	$4,$0,_time1
	stw	$2,$4,0
_time1:
	jr	$31


	; int ftime(struct timeb *tp)
ftime:
	add	$8,$0,SYS_FTIME
	trap
	jr	$31


	; int stime(long *tp)
stime:
	ldw	$4,$4,0
	add	$8,$0,SYS_STIME
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


	; int times(struct tbuffer *buffer)
times:
	add	$8,$0,SYS_TIMES
	trap
	jr	$31


	; int utime(const char *file, long timep[2])
utime:
	add	$8,$0,SYS_UTIME
	trap
	bne	$8,$0,sysError
	add	$2,$0,0
	jr	$31


;
; common error handling
;


sysError:
	stw	$8,$0,errno		; store error code in errno
	sub	$2,$0,1			; return -1
	jr	$31
