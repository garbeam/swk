CC?=gcc
DESTDIR?=
PREFIX?=${DESTDIR}/usr/local
LIBDIR?=${PREFIX}/lib

all: static test

test.o:
	${CC} -I. test.c -c -o test.o

test: test.o
	${CC} test.o -o test libswk.a

clean:
	rm -f libswk.a test.o swk.o test

install:
	cp ${DESTDIR}/${LIBDIR}

static: libswk.a

libswk.a: swk.o
	rm -f libswk.a
	ar qcvf libswk.a swk.o
