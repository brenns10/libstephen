# libstephen makefile

CC=gcc
FLAGS=-g
CFLAGS=$(FLAGS) -c -std=c99 -fPIC $(shell if [ -f src/libstephen_conf.h ] ; then echo "-DSMB_CONF" ; fi)
LFLAGS=$(FLAGS)
LIBOBJECTS=obj/linkedlist.o obj/util.o obj/arraylist.o obj/smbunit.o obj/args.o obj/hashtable.o obj/bitfield.o obj/utf8.o
TESTOBJECTS=obj/main.o obj/linkedlisttest.o obj/arraylisttest.o obj/argstest.o obj/hashtabletest.o obj/bitfieldtest.o obj/utf8test.o

.PHONY: all test lib clean testlib documentation

# Main targets

all: lib so test testlib

test: bin/test

testlib: bin/testlib

lib: bin/libstephen.a

so: bin/libstephen.so

docs: src/* src/test/*
	doxygen

clean:
	rm -rf obj/* bin/* doc/*

# Main Binaries

bin/test: $(LIBOBJECTS) $(TESTOBJECTS)
	$(CC) $(LFLAGS) $(LIBOBJECTS) $(TESTOBJECTS) -o bin/test

bin/libstephen.a: $(LIBOBJECTS)
	ar rcs bin/libstephen.a $(LIBOBJECTS)

bin/testlib: bin/libstephen.a $(TESTOBJECTS)
	$(CC) $(LFLAGS) $(TESTOBJECTS) --static -L bin -lstephen -o bin/testlib

bin/libstephen.so: $(LIBOBJECTS)
	$(CC) $(LFLAGS) -shared -Wl,-soname,libstephen.so -o bin/libstephen.so $(LIBOBJECTS)

# Library objects

obj/linkedlist.o: src/linkedlist.c src/libstephen.h
	$(CC) $(CFLAGS) src/linkedlist.c -o obj/linkedlist.o

obj/arraylist.o: src/arraylist.c src/libstephen.h
	$(CC) $(CFLAGS) src/arraylist.c -o obj/arraylist.o

obj/smbunit.o: src/smbunit.c src/libstephen.h
	$(CC) $(CFLAGS) src/smbunit.c -o obj/smbunit.o

obj/common_include.o: src/common_include.c src/libstephen.h
	$(CC) $(CFLAGS) src/common_include.c -o obj/common_include.o

obj/args.o: src/args.c src/libstephen.h
	$(CC) $(CFLAGS) src/args.c -o obj/args.o

obj/hashtable.o: src/hashtable.c src/libstephen.h
	$(CC) $(CFLAGS) src/hashtable.c -o obj/hashtable.o

obj/bitfield.o: src/bitfield.c src/libstephen.h
	$(CC) $(CFLAGS) src/bitfield.c -o obj/bitfield.o

# Test objects

obj/main.o: src/test/main.c src/test/tests.h
	$(CC) $(CFLAGS) src/test/main.c -o obj/main.o

obj/linkedlisttest.o: src/test/linkedlisttest.c src/test/tests.h src/libstephen.h
	$(CC) $(CFLAGS) src/test/linkedlisttest.c -o obj/linkedlisttest.o

obj/arraylisttest.o: src/test/arraylisttest.c src/test/tests.h src/libstephen.h
	$(CC) $(CFLAGS) src/test/arraylisttest.c -o obj/arraylisttest.o

obj/argstest.o: src/test/argstest.c src/libstephen.h
	$(CC) $(CFLAGS) src/test/argstest.c -o obj/argstest.o

obj/hashtabletest.o: src/test/hashtabletest.c src/libstephen.h src/test/tests.h
	$(CC) $(CFLAGS) src/test/hashtabletest.c -o obj/hashtabletest.o

obj/bitfieldtest.o: src/test/bitfieldtest.c src/libstephen.h src/test/tests.h
	$(CC) $(CFLAGS) src/test/bitfieldtest.c -o obj/bitfieldtest.o
