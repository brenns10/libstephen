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
INC=-Iinc/
CFLAGS=$(FLAGS) -c -std=c99 -fPIC $(SMB_CONF) $(INC)
LFLAGS=$(FLAGS)
DIR_GUARD=@mkdir -p $(@D)

# Build configurations.
CFG=release
ifeq ($(CFG),debug)
FLAGS += -g -DDEBUG -DSMB_DEBUG
endif
ifneq ($(CFG),debug)
ifneq ($(CFG),release)
	@echo "Invalid configuration "$(CFG)" specified."
	@echo "You must specify a configuration when running make, e.g."
	@echo "  make CFG=debug"
	@echo "Choices are 'release' and 'debug'."
	@exit 1
endif
endif

# Source Files and Directories
LIBSOURCES=$(shell find src/ -type f -name "*.c")
LIBOBJECTS=$(patsubst src/%.c,obj/$(CFG)/%.o,$(LIBSOURCES))

TESTSOURCES=$(shell find test/ -type f -name "*.c")
TESTOBJECTS=$(patsubst test/%.c,obj/$(CFG)/test/%.o,$(TESTSOURCES))

# Top Level Targets
.PHONY: all test lib clean docs

all: lib test

lib: bin/$(CFG)/libstephen.a

test: lib bin/$(CFG)/test

clean:
	rm -rf obj/$(CFG)/* bin/$(CFG)/* doc/*

docs: src/* test/*
	doxygen

bin/$(CFG)/libstephen.a: $(LIBOBJECTS)
	$(DIR_GUARD)
	ar rcs bin/$(CFG)/libstephen.a $(LIBOBJECTS)

bin/$(CFG)/test: $(LIBOBJECTS) $(TESTOBJECTS)
	$(DIR_GUARD)
	$(CC) $(LFLAGS) $(LIBOBJECTS) $(TESTOBJECTS) -o bin/$(CFG)/test

obj/$(CFG)/test/%.o: test/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $< -o $@

obj/$(CFG)/%.o: src/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $< -o $@

# Explicit dependencies
src/linkedlist.c: inc/libstephen/ll.h
src/arraylist.c: inc/libstephen/al.h
src/smbunit.c: inc/libstephen/ut.h inc/libstephen/base.h
src/util.c: inc/libstephen/base.h
src/args.c: inc/libstephen/ad.h inc/libstephen/base.h inc/libstephen/ll.h \
            inc/libstephen/list.h
src/hashtable.c: inc/libstephen/ht.h
src/bitfield.c: inc/libstephen/bf.h inc/libstephen/base.h
src/utf8.c: inc/libstephen/util.h inc/libstephen/base.h
src/iter.c: inc/libstephen/base.h inc/libstephen/list.h

test/main.c: test/tests.h inc/libstephen/base.h
test/linkedlisttest.c: test/tests.h inc/libstephen/ll.h inc/libstephen/ut.h
test/arraylisttest.c: test/tests.h inc/libstephen/al.h inc/libstephen/ut.h
test/argstest.c: inc/libstephen/ad.h inc/libstephen/ll.h inc/libstephen/ut.h
test/hashtabletest.c: inc/libstephen/ht.h test/tests.h inc/libstephen/ut.h
test/bitfieldtest.c: inc/libstephen/bf.h test/tests.h inc/libstephen/ut.h
test/utf8test.c: inc/libstephen/util.h test/tests.h inc/libstephen/ut.h
test/itertest.c: test/tests.h inc/libstephen/list.h inc/libstephen/ll.h \
                 inc/libstephen/al.h inc/libstephen/ut.h
