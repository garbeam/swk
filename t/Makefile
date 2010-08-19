include ../swk.mk

CFLAGS=-I.. -Wall

all: test calc tlock ui

test: test.o
	${CC} ${SWKLIBS} test.o -o test ../libswk.a

tlock: tlock.o
	${CC} ${SWKLIBS} tlock.o -o tlock ../libswk.a

calc: calc.o
	${CC} ${SWKLIBS} calc.o -o calc ../libswk.a

ui: ui.o
	${CC} ${SWKLIBS} ui.o -o ui ../libswk.a

clean:
	rm -f calc calc.o test test.o ui ui.o tlock tlock.o

fun:
	cd .. && make clean ; make && cd t && ./test
