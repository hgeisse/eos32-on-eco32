/*
 * xtest.h -- extended test configuration
 */


#ifndef _XTEST_H_
#define _XTEST_H_


/*
 * Set KRNL_EXC_TST to any value in the range 16..25 to
 * check if a kernel mode exception with this number is
 * handled properly. Note that exceptions 18 (privileged
 * instruction) and 25 (privileged address) cannot happen
 * in kernel mode. Exception 21 (TLB miss) has its own
 * handler already installed and thus cannot be tested.
 *
 * Set KRNL_EXC_TST to 0 to disable this test.
 */
#define KRNL_EXC_TST	0


/*
 * If KRNL_EXC_TST is set to 0, i.e. switched off, you
 * may set USER_EXC_TST to any value in the range 16..25
 * to check if a user mode exception with this number is
 * handled properly. Note that exception 21 (TLB miss)
 * has its own handler already installed and thus cannot
 * be tested.
 *
 * If you want to test a system call (with USER_EXC_TST
 * set to 20, i.e. trap instruction), you must specify
 * the system call number by setting SYSCALL_TST to the
 * appropriate value, which must be in the range 0..63.
 *
 * Set USER_EXC_TST to 0 to disable this test.
 */
#define USER_EXC_TST	0
#define SYSCALL_TST	0


/*
 * If USER_EXC_TST is set to 0, i.e. switched off, you
 * may set APPL_PRG_TST to 1 to start a simple shell
 * which is capable of running some application test
 * programs.
 *
 * Set APPL_PRG_TST to 0 to disable this test.
 */
#define APPL_PRG_TST	0


#endif /* _XTEST_H_ */
