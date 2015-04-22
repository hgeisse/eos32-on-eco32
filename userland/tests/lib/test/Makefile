all:		assert ctype string

assert:		assert.c
		gcc -Wall -o assert assert.c

ctype:		ctype.c
		gcc -Wall -o ctype ctype.c

string:		string.c
		gcc -Wall -o string string.c

clean:
		rm -f *~ assert ctype string
