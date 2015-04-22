/*
 * chown uid file ...
 */

#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct	passwd	*pwd;
struct	stat	stbuf;
int	uid;
int	status;

int isnumber(char *s);

int main(int argc, char *argv[])
{
	register c;

	if(argc < 3) {
		printf("usage: chown uid file ...\n");
		exit(4);
	}
	if(isnumber(argv[1])) {
		uid = atoi(argv[1]);
		goto cho;
	}
	if((pwd=getpwnam(argv[1])) == NULL) {
		printf("unknown user id: %s\n",argv[1]);
		exit(4);
	}
	uid = pwd->pw_uid;

cho:
	for(c=2; c<argc; c++) {
		stat(argv[c], &stbuf);
		if(chown(argv[c], uid, stbuf.st_gid) < 0) {
			perror(argv[c]);
			status = 1;
		}
	}
	exit(status);
	return 0;
}

int isnumber(char *s)
{
	register c;

	while(c = *s++)
		if(!isdigit(c))
			return(0);
	return(1);
}
