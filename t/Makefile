include ../swk.mk

CFLAGS=-I..

all: test ui

test: test.o
	${CC} ${SWKLIBS} test.o -o test ../libswk.a

ui: ui.o
	${CC} ${SWKLIBS} ui.o -o ui ../libswk.a

clean:
	rm -f test test.o ui ui.o
