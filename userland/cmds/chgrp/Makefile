#
# Makefile for 'chgrp' command
#

BUILD = ../../../../build

all:		chgrp

install:	chgrp
		mkdir -p $(BUILD)/cmds
		cp chgrp $(BUILD)/cmds

chgrp:		chgrp.c
		$(BUILD)/bin/lcc -A -o chgrp chgrp.c

clean:
		rm -f *~ chgrp
