CC?=gcc
CFLAGS?=-Wall -g
DESTDIR?=
PREFIX?=${DESTDIR}/usr/local
LIBDIR?=${PREFIX}/lib

# graphic backend
GI?=sdl
GI_LIBS=-lSDL

GI_OBJS=gi_${GI}.o draw.o

all: static test

test.o:
	${CC} -I. test.c -c -o test.o

test: test.o libswk.a
	${CC} test.o -o test libswk.a ${GI_LIBS}

clean:
	rm -f libswk.a test.o swk.o test ${GI_OBJS}

install:
	cp libswk.a ${DESTDIR}/${LIBDIR}
	# TODO: create pkgconfig?

static: libswk.a

libswk.a: swk.o ${GI_OBJS}
	rm -f libswk.a
	ar qcvf libswk.a swk.o ${GI_OBJS}
