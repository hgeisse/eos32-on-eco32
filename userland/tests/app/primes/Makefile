#
# Makefile for the 'primes' command
#

BUILD = ../../../../../build

all:		primes

install:	primes
		mkdir -p $(BUILD)/tests/app
		cp primes $(BUILD)/tests/app

primes:		primes.c
		$(BUILD)/bin/lcc -A -o primes primes.c

clean:
		rm -f *~ primes
