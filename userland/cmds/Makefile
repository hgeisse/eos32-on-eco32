#
# Makefile for EOS32 userland commands
#

BUILD = ../../build

DIRS = ar as cat chgrp chmod chown clri cmp cp date dcheck dd \
       deroff df dof du echo ed fsck getty grep hangman icheck \
       init joe kill lcc ld lex ln load login ls man make mkdir \
       mkfs mkmboot mknod mkpart mkpboot mount mv ncheck \
       od passwd ps pr pwd rm rmdir sh shfs shpart stty \
       su sync touch troff umount update wc who yacc yes

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
