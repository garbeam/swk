CC?=gcc
CFLAGS?=-Wall -g -std=c99
VERSION=0.1
DESTDIR?=
PREFIX?=${DESTDIR}/usr/local
LIBDIR?=${PREFIX}/lib
CFLAGS+=-I.

# graphic backend
GI?=sdl
GI_LIBS=-lSDL -lSDL_ttf

GI_OBJS=gi_${GI}.o

all: static test

test: test.o libswk.a
	${CC} test.o -o test libswk.a ${GI_LIBS}

clean:
	rm -f swk.pc swk.mk libswk.a test.o swk.o test ${GI_OBJS}

install:
	mkdir -p ${DESTDIR}/${LIBDIR}
	cp libswk.a ${DESTDIR}/${LIBDIR}
	mkdir -p ${DESTDIR}/${LIBDIR}/mk
	cp swk.mk ${DESTDIR}/${LIBDIR}/mk/swk.mk
	mkdir -p ${DESTDIR}/${LIBDIR}/pkgconfig
	cp swk.pc ${DESTDIR}/${LIBDIR}/pkgconfig/swk.pc

static: libswk.a

libswk.a: swk.o ${GI_OBJS}
	rm -f libswk.a
	ar qcvf libswk.a swk.o ${GI_OBJS}
	echo CFLAGS+=-I${PREFIX}/include > swk.mk
	echo LDFLAGS+=${PREFIX}/lib/libswk.a ${GI_LIBS} >> swk.mk
	echo prefix=${PREFIX} > swk.pc
	echo libdir=${LIBDIR} >> swk.pc
	echo >> swk.pc
	echo Name: swk >> swk.pc
	echo Version: ${VERSION} >> swk.pc
	echo Description: simple widget kit >> swk.pc
	echo Libs: ${PREFIX}/lib/libswk.a ${GI_LIBS} >> swk.pc
	echo Cflags: -I${PREFIX}/include >> swk.pc
