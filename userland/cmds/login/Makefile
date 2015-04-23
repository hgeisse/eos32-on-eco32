#
# Makefile for 'login' command
#

BUILD = ../../../build

all:		login

install:	login
		mkdir -p $(BUILD)/cmds
		cp login $(BUILD)/cmds

login:		login.c
		$(BUILD)/bin/lcc -A -o login login.c

clean:
		rm -f *~ login
