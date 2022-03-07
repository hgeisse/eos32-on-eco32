/*
 * who
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <eos32.h>

struct utmp utmp;
struct passwd *pw;

void putline(void);

int main(int argc, char **argv)
{
	char *tp, *s;
	FILE *fi;

	s = "/etc/utmp";
	if(argc == 2)
		s = argv[1];
	if (argc==3) {
		tp = ttyname(0);
		if (tp)
			tp = strchr(tp+1, '/') + 1;
		else {	/* no tty - use best guess from passwd file */
			pw = getpwuid(getuid());
			strcpy(utmp.ut_name, pw?pw->pw_name: "?");
			strcpy(utmp.ut_line, "tty??");
			time(&utmp.ut_time);
			putline();
			exit(0);
		}
	}
	if ((fi = fopen(s, "r")) == NULL) {
		puts("who: cannot open utmp");
		exit(1);
	}
	while (fread((char *)&utmp, sizeof(utmp), 1, fi) == 1) {
		if(argc==3) {
			if (strcmp(utmp.ut_line, tp))
				continue;
#ifdef interdata
			printf("(Interdata) ");
#endif
			putline();
			exit(0);
		}
		if(utmp.ut_name[0] == '\0' && argc==1)
			continue;
		putline();
	}
	return 0;
}

void putline(void)
{
	char *cbuf;

	printf("%-8.8s %-8.8s", utmp.ut_name, utmp.ut_line);
	cbuf = ctime(&utmp.ut_time);
	printf("%.12s\n", cbuf+4);
}
