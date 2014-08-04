#-------------------------------------------------------------------------------
# 
# File:         Makefile
#
# Author:       Stephen Brennan
#
# Date Created: Wednesday, 30 July 2014
#
# Description:  libstephen Makefile
#
#-------------------------------------------------------------------------------

# Configuration Variables
CC=gcc
FLAGS=
SMB_CONF=$(shell if [ -f inc/libstephen_conf.h ] ; then echo "-DSMB_CONF" ; fi)
INC=-Iinc/
CFLAGS=$(FLAGS) -c -std=c99 -fPIC $(SMB_CONF) $(INC)
LFLAGS=$(FLAGS)

# Source Files and Directories
LIBSOURCES=$(shell find src/ -type f -name "*.c")
LIBOBJECTS=$(patsubst src/%.c,obj/%.o,$(LIBSOURCES))

TESTSOURCES=$(shell find test/ -type f -name "*.c")
TESTOBJECTS=$(patsubst test/%.c,obj/test/%.o,$(TESTSOURCES))

# Top Level Targets
.PHONY: all test lib clean docs
all: lib test
debug: CFLAGS += -g
debug: all
lib: bin/libstephen.a
test: lib bin/test
docs: src/* test/*
	doxygen
clean:
	rm -rf obj/* bin/* doc/*

# Main Binaries
bin/libstephen.a: $(LIBOBJECTS)
	@mkdir -p $(@D)
	ar rcs bin/libstephen.a $(LIBOBJECTS)

bin/test: $(LIBOBJECTS) $(TESTOBJECTS)
	@mkdir -p $(@D)
	$(CC) $(LFLAGS) $(LIBOBJECTS) $(TESTOBJECTS) -o bin/test

# Library objects
obj/test/%.o: test/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

# Explicit dependencies
src/linkedlist.c: inc/libstephen/ll.h
src/arraylist.c: inc/libstephen/al.h
src/smbunit.c: inc/libstephen/ut.h inc/libstephen/base.h
src/util.c: inc/libstephen/base.h
src/args.c: inc/libstephen/ad.h inc/libstephen/base.h inc/libstephen/ll.h
src/hashtable.c: inc/libstephen/ht.h
src/bitfield.c: inc/libstephen/bf.h inc/libstephen/base.h
src/utf8.c: inc/libstephen/util.h inc/libstephen/base.h

test/main.c: test/tests.h inc/libstephen/base.h
test/linkedlisttest.c: test/tests.h inc/libstephen/ll.h inc/libstephen/ut.h
test/arraylisttest.c: test/tests.h inc/libstephen/al.h inc/libstephen/ut.h
test/argstest.c: inc/libstephen/ad.h inc/libstephen/ll.h inc/libstephen/ut.h
test/hashtabletest.c: inc/libstephen/ht.h test/tests.h inc/libstephen/ut.h
test/bitfieldtest.c: inc/libstephen/bf.h test/tests.h inc/libstephen/ut.h
test/utf8test.c: inc/libstephen/util.h test/tests.h inc/libstephen/ut.h
