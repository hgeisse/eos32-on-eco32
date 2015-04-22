/*
 * Update the file system every 30 seconds.
 * For cache benefit, open certain system directories.
 */

#include <eos32.h>
#include <stdlib.h>
#include <signal.h>

char *fillst[] = {
	"/bin",
	"/usr",
	"/usr/bin",
	0,
};

void dosync(int sig);

int main(void)
{
	char **f;

	if(fork())
		exit(0);
	close(0);
	close(1);
	close(2);
	for(f = fillst; *f; f++)
		open(*f, 0);
	dosync(SIGALRM);
	for(;;)
		pause();
}

void dosync(int sig)
{
	sync();
	signal(SIGALRM, dosync);
	alarm(30);
}
