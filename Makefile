# libstephen makefile

CC=gcc
FLAGS=
CFLAGS=$(FLAGS) -c -std=c99
LFLAGS=$(FLAGS)
LIBOBJECTS=obj/linkedlist.o obj/common_include.o
TESTOBJECTS=obj/main.o obj/linkedlisttest.o

.PHONY: all test lib clean testlib

# Main targets

all: lib test testlib

test: bin/test

testlib: bin/testlib

lib: bin/libstephen.a

clean:
	rm -rf obj/* bin/*

# Main Binaries

bin/test: $(LIBOBJECTS) $(TESTOBJECTS)
	$(CC) $(LFLAGS) $(LIBOBJECTS) $(TESTOBJECTS) -o bin/test

bin/libstephen.a: $(LIBOBJECTS)
	ar rcs bin/libstephen.a $(LIBOBJECTS)

bin/testlib: bin/libstephen.a $(TESTOBJECTS)
	$(CC) $(LFLAGS) $(TESTOBJECTS) -L bin -lstephen -o bin/testlib

# Library objects

obj/linkedlist.o: linkedlist/linkedlist.c linkedlist/linkedlist.h generic.h
	$(CC) $(CFLAGS) linkedlist/linkedlist.c -o obj/linkedlist.o

obj/common_include.o: common_include.c common_include.h
	$(CC) $(CFLAGS) common_include.c -o obj/common_include.o

# Test objects

obj/main.o: test/main.c test/tests.h
	$(CC) $(CFLAGS) test/main.c -o obj/main.o

obj/linkedlisttest.o: test/linkedlisttest.c test/tests.h linkedlist/linkedlist.h common_include.h
	$(CC) $(CFLAGS) test/linkedlisttest.c -o obj/linkedlisttest.o
