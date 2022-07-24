#include <sys/off.h>
#include <sys/blk.h>
#include <sys/tim.h>
#include <sys/ino.h>
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>

daddr_t	blkno	= 1;
char	*dargv[] = {
	0,
	"/dev/idediskp0",
	"/dev/idediskp2",
	"/dev/idediskp3",
	0
};


struct	filsys sblock;

int	fi;

void dfree(char *file);
daddr_t alloc(void);
void bread(daddr_t bno, char *buf, int cnt);

int main(int argc, char **argv)
{
	int i;

	if(argc <= 1) {
		for(argc = 1; dargv[argc]; argc++);
		argv = dargv;
	}

	for(i=1; i<argc; i++) {
		dfree(argv[i]);
	}

	return 0;
}

void dfree(char *file)
{
	daddr_t i;

	fi = open(file, 0);
	if(fi < 0) {
		fprintf(stderr,"cannot open %s\n", file);
		return;
	}
	sync();
	bread(1L, (char *)&sblock, sizeof(sblock));
	i = 0;
	while(alloc())
		i++;
	printf("%s %ld\n", file, i);
	close(fi);
}

daddr_t alloc(void)
{
	int i;
	daddr_t b;
	struct fblk buf;

	i = --sblock.s_nfree;
	if(i<0 || i>=NICFREE) {
		printf("bad free count, b=%ld\n", blkno);
		return(0);
	}
	b = sblock.s_free[i];
	if(b == 0)
		return(0);
	if(b<sblock.s_isize+2 || b>=sblock.s_fsize) {
		printf("bad free block (%ld)\n", b);
		return(0);
	}
	if(sblock.s_nfree <= 0) {
		bread(b, (char *)&buf, sizeof(buf));
		blkno = b;
		sblock.s_nfree = buf.df_nfree;
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.df_free[i];
	}
	return(b);
}

void bread(daddr_t bno, char *buf, int cnt)
{
	int n;
	extern errno;

	lseek(fi, bno<<BSHIFT, 0);
	if((n=read(fi, buf, cnt)) != cnt) {
		printf("read error %ld\n", bno);
		printf("count = %d; errno = %d\n", n, errno);
		exit(0);
	}
}
