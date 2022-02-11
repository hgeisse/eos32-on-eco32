/*
 * main.c -- start the ball rolling
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
#include "../include/mount.h"
#include "../include/map.h"
#include "../include/proc.h"
#include "../include/ino.h"
#include "../include/inode.h"
#include "../include/seg.h"
#include "../include/conf.h"
#include "../include/buf.h"
#include "../include/tty.h"

#include "start.h"
#include "machdep.h"
#include "trap.h"
#include "klib.h"
#include "slp.h"
#include "sig.h"
#include "prim.h"
#include "bio.h"
#include "idedsk.h"
#include "serdsk.h"
#include "sdcdsk.h"
#include "ramdsk.h"
#include "pipe.h"
#include "rdwri.h"
#include "fio.h"
#include "nami.h"
#include "subr.h"
#include "alloc.h"
#include "iget.h"
#include "malloc.h"
#include "ureg.h"
#include "xtest.h"
#include "c.h"


#define VERSION		"0.27"


Bool debugStartup = FALSE;

struct inode *rootdir;


/*
 * This is the 'icode', the initial code for
 * process 1, executed in user mode.
 */
static unsigned char icode[] = {
  #include "icode/icode.dump"
};

static int szicode = sizeof(icode);


/*
 * Control the amount of debugging output.
 */
void setDebugOutput(void) {

  /* main.c */
  debugStartup = FALSE;

  /* klib.c */
  debugCopyFrame = FALSE;
  debugClearFrame = FALSE;
  debugCopyIn = FALSE;
  debugCopyOut = FALSE;
  debugFubyte = FALSE;
  debugFuword = FALSE;
  debugSubyte = FALSE;
  debugSuword = FALSE;

  /* pipe.c */
  debugPlock = FALSE;
  debugPrele = FALSE;

  /* rdwri.c */
  debugReadi = FALSE;
  debugWritei = FALSE;
  debugIomove = FALSE;

  /* subr.c */
  debugBmap = FALSE;
  debugBcopy = FALSE;

  /* slp.c */
  debugSleep = FALSE;
  debugWakeup = FALSE;
  debugSetpri = FALSE;
  debugSched = FALSE;
  debugSwtch = FALSE;
  debugNewproc = FALSE;
  debugExpand = FALSE;

  /* sig.c */
  debugPsignal = FALSE;
  debugGrow = FALSE;

  /* ureg.c */
  debugPagingParams = FALSE;
  debugPageTableMiss = FALSE;
  debugEstabur = FALSE;
  debugSureg = FALSE;
  debugCheckAddrs = FALSE;

  /* malloc.c */
  debugMalloc = FALSE;

  /* trap.c */
  debugSyscall = FALSE;
  debugTrap = FALSE;

  /* fio.c*/
  debugGetf = FALSE;
  debugClosef = FALSE;
  debugOpeni = FALSE;
  debugAccess = FALSE;
  debugOwner = FALSE;
  debugSuser = FALSE;
  debugUfalloc = FALSE;
  debugFalloc = FALSE;

  /* nami.c */
  debugNamei = FALSE;

  /* alloc.c */
  debugIinit = FALSE;
  debugAlloc = FALSE;
  debugFree = FALSE;
  debugIalloc = FALSE;
  debugIfree = FALSE;
  debugUpdate = FALSE;

  /* iget.c */
  debugIget = FALSE;
  debugIput = FALSE;
  debugIupdat = FALSE;
  debugItrunc = FALSE;
  debugMaknode = FALSE;
  debugWdir = FALSE;

  /* prim.c */
  debugCinit = FALSE;

  /* bio.c */
  debugBinit = FALSE;
  debugBread = FALSE;
  debugBreada = FALSE;
  debugBwrite = FALSE;
  debugBdwrite = FALSE;
  debugBawrite = FALSE;
  debugBrelse = FALSE;
  debugGetblk = FALSE;
  debugGeteblk = FALSE;
  debugIodone = FALSE;
  debugClrbuf = FALSE;
  debugSwap = FALSE;
  debugBflush = FALSE;
  debugPhysio = FALSE;

  /* idedsk.c */
  debugIdeDisk = FALSE;

  /* serdsk.c */
  debugSerDisk = FALSE;

  /* sdcdsk.c */
  debugSdcDisk = FALSE;

  /* ramdsk.c */
  debugRamDisk = FALSE;

}


#if KRNL_EXC_TST == 16
void kernelExceptionTest(void) {
  unsigned int *a;
  unsigned int w;

  a = (unsigned int *) 0xF0600000;
  w = *a;
}
#endif


#if KRNL_EXC_TST == 17
void kernelExceptionTest(void) {
  unsigned int *a;

  a = (unsigned int *) kernelExceptionTest;
  *a = 0x1E << 26;
  kernelExceptionTest();
}
#endif


#if KRNL_EXC_TST == 18
void kernelExceptionTest(void) {
  panic("cannot test exception 18");
}
#endif


#if KRNL_EXC_TST == 19
void kernelExceptionTest(void) {
  int i;

  i = 15;
  i = i / (i - i);
}
#endif


#if KRNL_EXC_TST == 20
void kernelExceptionTest(void) {
  unsigned int *a;

  a = (unsigned int *) kernelExceptionTest;
  *a = 0x2E << 26;
  kernelExceptionTest();
}
#endif


#if KRNL_EXC_TST == 21
void kernelExceptionTest(void) {
  panic("cannot test exception 21");
}
#endif


#if KRNL_EXC_TST == 22
void kernelExceptionTest(void) {
  unsigned int *a;

  setTLB(17, 0x80000000, 0x00000001);
  a = (unsigned int *) 0x80000000;
  *a = 0;
}
#endif


#if KRNL_EXC_TST == 23
void kernelExceptionTest(void) {
  unsigned int *a;
  unsigned int w;

  setTLB(17, 0x80000000, 0x00000002);
  a = (unsigned int *) 0x80000000;
  w = *a;
}
#endif


#if KRNL_EXC_TST == 24
void kernelExceptionTest(void) {
  unsigned int *a;
  unsigned int w;

  a = (unsigned int *) 0xF0000002;
  w = *a;
}
#endif


#if KRNL_EXC_TST == 25
void kernelExceptionTest(void) {
  panic("cannot test exception 25");
}
#endif


/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */
void main(void) {
  int numFrames;	/* total memory size as number of frames */
  int startFree;	/* frame number of first free frame */
  int numFree;		/* free memory given to user processes */
  int numUnused;	/* memory that will not be used */
  int maxMem;		/* max number of frames per process */
  int numSwap;		/* number of blocks on swap device */

  /* control where kernel output should appear */
  outputToLogfile = OUTPUT_TO_LOGFILE;
  outputToConsole = OUTPUT_TO_CONSOLE;
  outputToTerminal = OUTPUT_TO_TERMINAL;

  /* set amount of debugging output */
  setDebugOutput();

  /* say hello */
  printf("\n");
  printf("EOS32 Version %s starting...\n", VERSION);
  printf("\n");

  /* initialize interrupts */
  initInterrupts();

  /* determine memory size */
  numFrames = getMemorySize();
  startFree = ADDR2FRAME(allocStart);
  if (numFrames > MEM_USE) {
    numFree = MEM_USE - startFree;
    numUnused = numFrames - MEM_USE;
  } else {
    numFree = numFrames - startFree;
    numUnused = 0;
  }
  maxMem = (MAXMEM < numFree ? MAXMEM : numFree);
  printf("Memory size   = %6d page frames * 4 Kbytes\n", numFrames);
  printf("       kernel = %6d page frames * 4 Kbytes\n", startFree);
  printf("       free   = %6d page frames * 4 Kbytes\n", numFree);
  printf("       unused = %6d page frames * 4 Kbytes\n", numUnused);
  printf("  per process = %6d page frames * 4 Kbytes\n", maxMem);
  printf("\n");

  /* free all of core */
  mfree(coremap, numFree, startFree);

  /* set up system process (process 0) */
  proc[0].p_addr = ADDR2FRAME(proc0uarea);
  proc[0].p_size = U_PAGES;
  proc[0].p_stat = SRUN;
  proc[0].p_flag = SLOAD|SSYS;
  proc[0].p_nice = NZERO;
  u.u_procp = &proc[0];
  u.u_cmask = CMASK;
  if (debugStartup) {
    showProcTable("proc table just after setting up proc 0");
  }

  /* initialize exception handling */
  trapInit();

  /* initialize paging */
  initPaging(maxMem);

  /* initialize clock */
  clockInit();

  /* set root, swap, and pipe devices */
  if (bootdsk == 0) {
    rootdev = makedev(0, 0);
    swapdev = makedev(0, 1);
    pipedev = makedev(0, 0);
  } else
  if (bootdsk == 1) {
    rootdev = makedev(1, 0);
    swapdev = makedev(1, 1);
    pipedev = makedev(1, 0);
  } else
  if (bootdsk == 2) {
    rootdev = makedev(6, 0);
    swapdev = makedev(6, 1);
    pipedev = makedev(6, 0);
  } else {
    printf("unknown boot disk, assuming disk 0\n");
    rootdev = makedev(0, 0);
    swapdev = makedev(0, 1);
    pipedev = makedev(0, 0);
  }

  /* determine swap size */
  numSwap = bdevsw[major(swapdev)].d_size(swapdev);
  if (numSwap == 0) {
    panic("no swap space");
  }

  /* free all of swap space */
  mfree(swapmap, numSwap, 0);

  /* initialize character buffers */
  cinit();

  /* initialize buffer cache */
  binit();

  /* initialize root file system */
  iinit();

  /* set up known inodes */
  rootdir = iget(rootdev, (ino_t) ROOTINO);
  rootdir->i_flag &= ~ILOCK;
  u.u_cdir = iget(rootdev, (ino_t) ROOTINO);
  u.u_cdir->i_flag &= ~ILOCK;
  u.u_rdir = NULL;

#if KRNL_EXC_TST >= 16 && KRNL_EXC_TST <= 25
  /* test kernel exception */
  kernelExceptionTest();
#endif

  /* make init process */
  if (newproc()) {
    /* this is executed by process 1 */
    if (debugStartup) {
      showProcTable("proc table when proc 1 comes to life");
    }
    expand(U_PAGES + BYTES2PAGES(szicode));
    estabur(0, BYTES2PAGES(szicode), 0, RO);
#if USER_EXC_TST >= 16 && USER_EXC_TST <= 25
  #if USER_EXC_TST == 20
    /* this is a syscall test */
    strcpy((char *) &icode[0x24], "/tst/sc??");
    icode[0x2B] = '0' + SYSCALL_TST / 10;
    icode[0x2C] = '0' + SYSCALL_TST % 10;
  #else
    /* this is a user mode exception test (but not a syscall) */
    strcpy((char *) &icode[0x24], "/tst/ux??");
    icode[0x2B] = '0' + USER_EXC_TST / 10;
    icode[0x2C] = '0' + USER_EXC_TST % 10;
  #endif
#else
  #if APPL_PRG_TST == 1
    /* start a simple shell */
    strcpy((char *) &icode[0x24], "/tst/init");
  #endif
#endif
    copyOut((caddr_t) icode, (caddr_t) 0, szicode);
    syncCaches();
    /* return goes to location 0 of user init code just copied out */
    return;
  }

  /* process 0 enters swapping loop */
  if (debugStartup) {
    showProcTable("proc table before entering the swapper");
  }
  sched();

  /* never reached */
  panic("end of main() reached");
}
