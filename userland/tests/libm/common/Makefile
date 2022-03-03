BUILD=../../../../build
CC=$(BUILD)/bin/lcc
INCLUDES=-I$(BUILD)/include/ 
#-I../../../include/math/

install: all

all: mtest.o

mtest.o: mtest.h mtest.c
	$(CC) $(INCLUDES) -o mtest.o -c mtest.c

clean:
	rm mtest.o


