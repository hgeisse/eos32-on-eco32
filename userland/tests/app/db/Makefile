#
# Makefile for the 'db' command
#

BUILD = ../../../../build

all:		db

install:	db
		mkdir -p $(BUILD)/tests/app
		cp db $(BUILD)/tests/app

db:		main.c db.c
		$(BUILD)/bin/lcc -A -o db main.c db.c

clean:
		rm -f *~ db
