/*
 * trap.c -- traps and other exceptions
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
#include "../include/proc.h"
#include "../include/reg.h"
#include "../include/seg.h"

#include "start.h"
#include "machdep.h"
#include "klib.h"
#include "slp.h"
#include "sig.h"
#include "trap.h"
#include "clock.h"


#define USER		0x100


Bool debugSyscall = FALSE;
Bool debugTrap = FALSE;

/*
 * Offsets of the user's registers relative
 * to the saved r0. See reg.h
 */
char regloc[33] = {
  R0,  R1,  R2,  R3,  R4,  R5,  R6,  R7,
  R8,  R9,  R10, R11, R12, R13, R14, R15,
  R16, R17, R18, R19, R20, R21, R22, R23,
  R24, R25, R26, R27, R28, R29, R30, R31,
  RPS
};


/**************************************************************/


/*
 * This routine implements the system call interface.
 */
static void syscall(void) {
  int ncall;
  int nargs;
  int i;

  /* get arguments */
  ncall = u.u_ar0[R8];
  if (ncall < 0 || ncall >= 64) {
    u.u_error = EINVAL;
    return;
  }
  nargs = sysent[ncall].sy_narg;
  for (i = 0; i < nargs; i++) {
    u.u_arg[i] = u.u_ar0[R4 + i];
  }
  u.u_ap = u.u_arg;
  u.u_error = 0;
  /* execute syscall service routine */
  if (debugSyscall) {
    printf("-----  syscall enter (pid = %d)  -----\n", u.u_procp->p_pid);
    printf("arguments: call = %d (%d args)\n", ncall, nargs);
    for (i = 0; i < nargs; i++) {
      printf("           arg%d = 0x%08X\n", i + 1, u.u_arg[i]);
    }
    printf("\n");
  }
  if (save(u.u_qsav)) {
    if (u.u_error == 0) {
      u.u_error = EINTR;
    }
  } else {
    (*sysent[ncall].sy_call)();
  }
  if (debugSyscall) {
    printf("-----  syscall leave (pid = %d)  -----\n", u.u_procp->p_pid);
    if (ncall == 59 && u.u_error == 0) {
      printf("no results - this was a successful exec\n");
    } else {
      printf("results: error = 0x%08X\n", u.u_error);
      printf("         rval1 = 0x%08X\n", u.u_r.r_val.r_val1);
      printf("         rval2 = 0x%08X\n", u.u_r.r_val.r_val2);
    }
    printf("\n");
  }
  if ((ncall != 49 && ncall != 59) || u.u_error != 0) {
    /* store results */
    u.u_ar0[R8] = u.u_error;
    u.u_ar0[R2] = u.u_r.r_val.r_val1;
    u.u_ar0[R3] = u.u_r.r_val.r_val2;
    /* skip trap instruction */
    u.u_ar0[PC] += 4;
  }
}


/**************************************************************/


/*
 * Initialize exception handling.
 */
void trapInit(void) {
  setISR(EXC_BUS_TIMEOUT, trapISR);
  setISR(EXC_ILL_INSTRCT, trapISR);
  setISR(EXC_PRV_INSTRCT, trapISR);
  setISR(EXC_DIVIDE, trapISR);
  setISR(EXC_TRAP, trapISR);
  setISR(EXC_TLB_MISS, trapISR);
  setISR(EXC_TLB_WRITE, trapISR);
  setISR(EXC_TLB_INVALID, trapISR);
  setISR(EXC_ILL_ADDRESS, trapISR);
  setISR(EXC_PRV_ADDRESS, trapISR);
}


/*
 * Trap interrupt service routine, which is called
 * for other exceptions as well. It handles also the
 * pseudo exception to reschedule the CPU.
 */
void trapISR(int tn, InterruptContext *icp) {
  time_t syst;

  /* set process priority, re-enable interrupts */
  spl0();
  enableInt();
  /* some preparations */
  syst = u.u_stime;
  u.u_fpsaved = 0;
  if (USERMODE(icp)) {
    tn += USER;
  }
  u.u_ar0 = (int *) &icp->ic_reg[0];
  /* branch according to exception number and mode */
  if (debugTrap) {
    printf("-----  pid = %d: trap %d in %s mode  -----\n",
           u.u_procp->p_pid,
           tn >= USER ? tn - USER : tn,
           tn >= USER ? "user" : "kernel");
  }
  switch (tn) {
    case USER + EXC_ILL_INSTRCT:
      psignal(u.u_procp, SIGINS);
      break;
    case USER + EXC_PRV_INSTRCT:
      psignal(u.u_procp, SIGINS);
      break;
    case USER + EXC_DIVIDE:
      psignal(u.u_procp, SIGFPT);
      break;
    case USER + EXC_TRAP:
      syscall();
      break;
    case USER + EXC_TLB_WRITE:
      psignal(u.u_procp, SIGSEG);
      break;
    case USER + EXC_TLB_INVALID:
      /* grow stack automatically */
      if (!grow(u.u_ar0[R29])) {
        psignal(u.u_procp, SIGSEG);
      }
      break;
    case USER + EXC_ILL_ADDRESS:
      psignal(u.u_procp, SIGBUS);
      break;
    case USER + EXC_PRV_ADDRESS:
      psignal(u.u_procp, SIGSEG);
      break;
    case USER + 32:
      /* rescheduling pseudo exception */
      break;
    default:
      /* exception not expected */
      if (tn < USER) {
        printf("exception %d in kernel mode\n", tn);
      } else {
        printf("exception %d in user mode\n", tn - USER);
      }
      printf("pid = %d, PC = 0x%08X\n", u.u_procp->p_pid, u.u_ar0[PC]);
      panic("unexpected exception");
      break;
  }
  if (issig()) {
    psig();
  }
  curpri = setpri(u.u_procp);
  if (runrun) {
    qswtch();
  }
  if (u.u_prof.pr_scale) {
    addupc((caddr_t) u.u_ar0[PC], &u.u_prof, u.u_stime - syst);
  }
  if (u.u_fpsaved) {
    restfp(&u.u_fps);
  }
}
