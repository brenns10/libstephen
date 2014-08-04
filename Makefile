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
src/linkedlist.c: inc/libstephen.h
src/arraylist.c: inc/libstephen.h
src/smbunit.c: inc/libstephen.h
src/util.c: inc/libstephen.h
src/args.c: inc/libstephen.h
src/hashtable.c: inc/libstephen.h
src/bitfield.c: inc/libstephen.h
src/utf8.c: inc/libstephen.h

src/test/main.c: src/test/tests.h
src/test/linkedlisttest.c: src/test/tests.h inc/libstephen.h
src/test/arraylisttest.c: src/test/tests.h inc/libstephen.h
src/test/argstest.c: inc/libstephen.h
src/test/hashtabletest.c: inc/libstephen.h src/test/tests.h
src/test/bitfieldtest.c: inc/libstephen.h src/test/tests.h
src/test/utf8test.c: inc/libstephen.h src/test/tests.h
