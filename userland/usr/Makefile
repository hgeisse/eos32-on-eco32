#
# Makefile for EOS32 usr part
#


# ATTENTION!
# this is a "fake" Makefile.
# it's only purpose is to copy the usr folder to the build directory

BUILD = ../../build


.PHONY:		install

install:
		cp -r --dereference . $(BUILD)/usr
		rm $(BUILD)/usr/Makefile