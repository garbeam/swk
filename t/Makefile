include ../swk.mk

CFLAGS=-I.. -Wall

all: test calc tlock ui

test: test.o
	${CC} test.o ../libswk.a ${SWKLIBS} -o test

tlock: tlock.o
	${CC} tlock.o ../libswk.a ${SWKLIBS} -o tlock

calc: calc.o
	${CC} calc.o ../libswk.a ${SWKLIBS} -o calc

ui: ui.o
	${CC} ui.o ../libswk.a ${SWKLIBS} -o ui

clean:
	rm -f calc calc.o test test.o ui ui.o tlock tlock.o

fun:
	cd .. && make clean ; make && cd t && ./test
