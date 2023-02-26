#
# Makefile for all fonts
# this file is normally called by another Makefile
#

BUILD=../../../../build
CC=$(BUILD)/bin/lcc
AS=$(BUILD)/bin/as


all:	ftB.o ftBC.o ftC.o ftCE.o ftCI.o ftCK.o ftCS.o ftCW.o ftG.o ftGI.o ftGM.o ftGR.o
all:	ftI.o ftL.o ftLI.o ftPA.o ftPB.o ftPI.o ftR.o ftS.o ftSB.o ftSI.o ftSM.o ftUD.o ftXM.o


install: 	all
	mkdir -p $(BUILD)/usr/lib/font
	for i in *.o; do cp $$i $(BUILD)/usr/lib/font/`basename $$i .o`; done

.c.o:
	$(CC) -c $<; 


mkfont: mkfont.c mkfont1.c -o mkfont
	$(CC) mkfont.c