#
# Makefile for EOS32 userland commands
#

BUILD = ../../build

DIRS = ar as cat chgrp chmod chown clri cmp cp date \
       dcheck df dof du echo ed getty grep icheck \
       init joe kill lcc ld lex ln load login ls mkdir \
       mkfs mkmboot mknod mkpart mkpboot mount ncheck \
       od passwd ps pwd rm rmdir sh shfs shpart stty \
       su sync touch umount update wc who yacc yes

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
