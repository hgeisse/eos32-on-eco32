/*
** make directory
*/

#include	<eos32.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<signal.h>

int	Errors = 0;

void mkdir(char *d);

int main(int argc, char *argv[])
{

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if(argc < 2) {
		fprintf(stderr, "mkdir: arg count\n");
		exit(1);
	}
	while(--argc)
		mkdir(*++argv);
	exit(Errors!=0);
	return 0;
}

void mkdir(char *d)
{
	char pname[128], dname[128];
	register i, slash = 0;

	pname[0] = '\0';
	for(i = 0; d[i]; ++i)
		if(d[i] == '/')
			slash = i + 1;
	if(slash)
		strncpy(pname, d, slash);
	strcpy(pname+slash, ".");
	if (access(pname, 02)) {
		fprintf(stderr,"mkdir: cannot access %s\n", pname);
		++Errors;
		return;
	}
	if ((mknod(d, S_IFDIR | 0777, 0)) < 0) {
		fprintf(stderr,"mkdir: cannot make directory %s\n", d);
		++Errors;
		return;
	}
	chown(d, getuid(), getgid());
	strcpy(dname, d);
	strcat(dname, "/.");
	if((link(d, dname)) < 0) {
		fprintf(stderr, "mkdir: cannot link %s\n", dname);
		unlink(d);
		++Errors;
		return;
	}
	strcat(dname, ".");
	if((link(pname, dname)) < 0) {
		fprintf(stderr, "mkdir: cannot link %s\n",dname);
		dname[strlen(dname)] = '\0';
		unlink(dname);
		unlink(d);
		++Errors;
	}
}
