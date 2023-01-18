#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <eos32sys.h>


/* SETTINGS */
#define DICT "/usr/dict/words"
#define EDICT "/crp/dict/web2"
#define MAXERR 7
#define MINSCORE 0
#define MINLEN 7
#define DEBUG 0

/* PROTOTYPES */
void setup(void);
double frand(void);
void startnew(void);
void stateout(void);
void getguess(void);
void wordout(void);
void youwon(void);
void fatal(char *s);
void getword(void);
void pscore(void);
void debug(char *s);

/* GLOBAL VARIABLES */
char *dictfile;
int alive,lost;
FILE *dict;
long int dictlen;
float errors=0, words=0;
long int freq[] = 
	{	42066,	9228,	24412,	14500,	55162,
		6098,	11992,	12648,	48241,	639,
		2944,	33351,	15545,	35618,	36211,
		16033,	937,	36686,	34957,	37544,
		17621,	5453,	3028,	1556,	12875,
		1743
	};
char word[26],alph[26],realword[26];

/* FUNCTIONS */
void main(int argc, char **argv) {
	if(argc == 1) {
		debug("main dict file");
		dictfile = DICT;
	} else if(*argv[1] == '-') {
		debug("edict replace dict file");
		dictfile = EDICT;
	} else {
		debug("user dict file");
		dictfile = argv[1];
	}
	debug("calling setup");
	setup();
	debug("going to the main loop");
	while(1){	
		debug("new game started");
		startnew();
		while(alive>0)
		{	
			debug("state out");
			stateout();
			debug("get guess");
			getguess();
		}
		words++;
		if(lost) {
			wordout();
		} else {
			youwon();
		}
	}
}

void debug(char *s) {
	if(DEBUG) printf("DEBUG: %s\n", s);
}

void setup(void) {	
	struct stat statb;
	srand(time(NULL));
	if(DEBUG) printf("setup: first rand call = %d\n", rand());
	if((dict = fopen(dictfile, "r")) == NULL) fatal("no dictionary");
	if(stat(dictfile, &statb) < 0) fatal("can't stat");
	dictlen = statb.st_size;
	if(DEBUG) printf("setup: dictlen = %d\n", dictlen);
}

double frand(void) {
	double ret = rand()/32768.;
	if(DEBUG) printf("frand: frand called = %f\n", ret);
	return ret;
}

void startnew(void) {	
	int i;
	long int pos;
	char buf[128];
	for(i = 0; i < 26; i++) {
		word[i] = alph[i] = realword[i] = 0;
	}
	pos = frand() * dictlen;
	fseek(dict, pos, 0);
	fscanf(dict, "%s\n", buf);
	debug("startnew: calling getword()");
	getword();
	alive = MAXERR;
	lost = 0;
}

void stateout(void) {	
	int i;
	printf("guesses: ");
	for(i=0;i<26;i++)
		if(alph[i]!=0) putchar(alph[i]);
	printf(" word: %s ",word);
	printf("errors: %d/%d\n",MAXERR-alive,MAXERR);
}

void getguess(void) {
	char gbuf[128],c;
	int ok=0,i;
	loop:
	printf("guess: ");
	if(gets(gbuf)==NULL)
	{	putchar('\n');
		exit(0);
	}
	if((c=gbuf[0])<'a' || c>'z')
	{	printf("lower case\n");
		goto loop;
	}
	if(alph[c-'a']!=0)
	{	printf("you guessed that\n");
		goto loop;
	}
	else alph[c-'a']=c;
	for(i=0;realword[i]!=0;i++)
		if(realword[i]==c)
		{	word[i]=c;
			ok=1;
		}
	if(ok==0)
	{	alive--;
		errors=errors+1;
		if(alive<=0) lost=1;
		return;
	}
	for(i=0;word[i]!=0;i++)
		if(word[i]=='.') return;
	alive=0;
	lost=0;
	return;
}

void wordout(void) {
	errors=errors+2;
	printf("the answer was %s, you blew it\n",realword);
}

void youwon(void) {
	printf("you win, the word is %s\n",realword);
}

void fatal(char *s) {
	fprintf(stderr,"%s\n",s);
	exit(1);
}

void getword(void) {	
	char wbuf[128], c;
	int i, j;
	debug("getword: entry point");
	loop:
	if(fgets(wbuf, sizeof(wbuf), dict) == NULL) {
		/* if end of file is reached, roll over */
		fseek(dict, 0L, 0);
		goto loop;
	}
	debug("getword: next from dict scanned to buffer");
	if(DEBUG) printf("\tword: %s\n", wbuf);
	c = wbuf[0];
	if(c > 'z' || c < 'a') goto loop;
	debug("getword: first char of buffer is a letter");
	/* remove '-' from string */
	for(i = j = 0; wbuf[j] != 0; i++, j++) {
		if(wbuf[j] == '-') j++;
		wbuf[i] = wbuf[j];
	}
	wbuf[--i] = 0; /* zero-terminate */
	if(DEBUG) printf("\tword: %s\n", wbuf);
	if(i < MINLEN) goto loop;
	debug("getword: word is long enough");
	for(j = 0; j < i; j++) {
		c = wbuf[j];
		if(c < 'a' || c > 'z') {
			printf("DEBUG: INVALID CHARACTER FOUND: %c (%d)\n", c, c);
			goto loop;
		}
	}
	debug("getword: word doesn't contain a non-alphabetical char");
	pscore();
	strcpy(realword, wbuf);
	for(j=0;j<i;word[j++]='.');
}

void pscore(void) {
	if(words != 0) printf("(%4.2f/%.0f) ", errors / words, words);
}
