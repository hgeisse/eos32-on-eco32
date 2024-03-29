/*
 * clri filsys inumber ...
 */

#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>

#define ISIZE	(sizeof(struct dinode))
#define	NI	(BSIZE/ISIZE)

struct	ino
{
	char	junk[ISIZE];
};

struct	ino	buf[NI];
int	status;

int isnumber(char *s);

int main(int argc, char *argv[])
{
	int i, f;
	unsigned n;
	int j, k;
	long off;

	if(argc < 3) {
		printf("usage: clri filsys inumber ...\n");
		exit(4);
	}
	f = open(argv[1], 2);
	if(f < 0) {
		printf("cannot open %s\n", argv[1]);
		exit(4);
	}
	for(i=2; i<argc; i++) {
		if(!isnumber(argv[i])) {
			printf("%s: is not a number\n", argv[i]);
			status = 1;
			continue;
		}
		n = atoi(argv[i]);
		if(n == 0) {
			printf("%s: is zero\n", argv[i]);
			status = 1;
			continue;
		}
		off = itod(n) * (long) BSIZE;
		lseek(f, off, 0);
		if(read(f, (char *)buf, BSIZE) != BSIZE) {
			printf("%s: read error\n", argv[i]);
			status = 1;
		}
	}
	if(status)
		exit(status);
	for(i=2; i<argc; i++) {
		n = atoi(argv[i]);
		printf("clearing %u\n", n);
		off = itod(n) * (long) BSIZE;
		lseek(f, off, 0);
		read(f, (char *)buf, BSIZE);
		j = itoo(n);
		for(k=0; k<ISIZE; k++)
			buf[j].junk[k] = 0;
		lseek(f, off, 0);
		write(f, (char *)buf, BSIZE);
	}
	exit(status);
	return 0;
}

int isnumber(char *s)
{
	int c;

	while(c = *s++)
		if(c < '0' || c > '9')
			return(0);
	return(1);
}
