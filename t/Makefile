include ../swk.mk

CFLAGS=-I.. -Wall

all: test tlock ui

test: test.o
	${CC} ${SWKLIBS} test.o -o test ../libswk.a

tlock: tlock.o
	${CC} ${SWKLIBS} tlock.o -o tlock ../libswk.a

ui: ui.o
	${CC} ${SWKLIBS} ui.o -o ui ../libswk.a

clean:
	rm -f test test.o ui ui.o tlock tlock.o

fun:
	cd .. && make clean ; make && cd t && ./test
