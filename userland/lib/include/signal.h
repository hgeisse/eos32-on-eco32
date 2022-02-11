/*
 * signal.h -- signals
 */


#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#define NSIG 17 /* taken from v7 */

#define SIGHUP		1	/* hangup [term] */
#define SIGINT		2	/* interrupt [term] */
#define SIGQUIT		3	/* quit [core] */
#define SIGILL		4	/* illegal struction [core] */
#define SIGTRAP		5	/* trace trap [core] */
#define SIGABRT		6	/* abort signal [core] */
#define SIGEMT		7	/* EMT instruction [core] */
#define SIGFPE		8	/* floating point exception [core] */
#define SIGKILL		9	/* kill (can't be caught or ignored) [term] */
#define SIGBUS		10	/* bus error [core] */
#define SIGSEGV		11	/* segmentation violation [core] */
#define SIGSYS		12	/* bad argument to system call [core] */
#define SIGPIPE		13	/* write on a pipe without a reader [term] */
#define SIGALRM		14	/* alarm clock [term] */
#define SIGTERM		15	/* software termination signal [term] */

#define SIG_ERR		((void (*)(int)) -1)
#define SIG_DFL		((void (*)(int)) 0)
#define SIG_IGN		((void (*)(int)) 1)


void (*signal(int sig, void (*handler)(int)))(int);
int raise(int sig);


#endif /* _SIGNAL_H_ */
