# libstephen makefile

CC=gcc
FLAGS=-g
CFLAGS=$(FLAGS) -c -std=c99
LFLAGS=$(FLAGS)
LIBOBJECTS=obj/linkedlist.o obj/common_include.o obj/arraylist.o obj/smbunit.o obj/args.o
TESTOBJECTS=obj/main.o obj/linkedlisttest.o obj/arraylisttest.o obj/argstest.o

.PHONY: all test lib clean testlib documentation

# Main targets

all: lib test testlib

test: bin/test

testlib: bin/testlib

lib: bin/libstephen.a

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
	$(CC) $(LFLAGS) $(TESTOBJECTS) -L bin -lstephen -o bin/testlib

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

# Test objects

obj/main.o: src/test/main.c src/test/tests.h
	$(CC) $(CFLAGS) src/test/main.c -o obj/main.o

obj/linkedlisttest.o: src/test/linkedlisttest.c src/test/tests.h src/libstephen.h
	$(CC) $(CFLAGS) src/test/linkedlisttest.c -o obj/linkedlisttest.o

obj/arraylisttest.o: src/test/arraylisttest.c src/test/tests.h src/libstephen.h
	$(CC) $(CFLAGS) src/test/arraylisttest.c -o obj/arraylisttest.o

obj/argstest.o: src/test/argstest.c src/libstephen.h
	$(CC) $(CFLAGS) src/test/argstest.c -o obj/argstest.o
