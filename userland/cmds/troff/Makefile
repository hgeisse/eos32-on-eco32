#
# main Makefile for 'troff' command
# attention! there are two subordinary Makefiles (nmake and tmake)
#

BUILD=../../../build

all:	nroff troff

nroff:
	make -f nmake

troff:
	make -f tmake


install:	troff nroff
		mkdir -p $(BUILD)/cmds
		cp troff $(BUILD)/cmds
		cp nroff $(BUILD)/cmds
		cd font; make install

clean:
	rm *.o
	rm troff
	rm nroff