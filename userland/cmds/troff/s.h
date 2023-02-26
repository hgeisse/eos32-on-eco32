#include <setjmp.h>
#ifndef __S_H_INCLUDED__
#define __S_H_INCLUDED__

extern jmp_buf sjbuf;

struct s {
	int nargs;
	struct s *pframe;
	filep pip;
	int pnchar;
	int prchar;
	int ppendt;
	int *pap;
	int *pcp;
	int pch0;
	int pch;
	};

#endif
