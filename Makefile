CC?=gcc
CFLAGS?=-Wall -g -std=c99
VERSION=0.1
DESTDIR?=
PREFIX?=${DESTDIR}/usr/local
LIBDIR?=${PREFIX}/lib
CFLAGS+=-I.

# graphic backend
GI?=sdl
GI_LIBS=-lSDL -lSDL_ttf -lSDL_image

GI_OBJS=gi_${GI}.o
GI_SRCS=gi_${GI}.c

all: static shared test

config.h:
	cp config.def.h config.h

test: config.h test.o libswk.a
	${CC} test.o -o test libswk.a ${GI_LIBS}

clean:
	rm -f swk.pc swk.mk libswk.a libswk.so test.o swk.o test ${GI_OBJS}

install:
	mkdir -p ${DESTDIR}/${LIBDIR}
	cp libswk.a ${DESTDIR}/${LIBDIR}
	cp libswk.so ${DESTDIR}/${LIBDIR}
	mkdir -p ${DESTDIR}/${LIBDIR}/mk
	cp swk.mk ${DESTDIR}/${LIBDIR}/mk/swk.mk
	mkdir -p ${DESTDIR}/${LIBDIR}/pkgconfig
	cp swk.pc ${DESTDIR}/${LIBDIR}/pkgconfig/swk.pc

static: libswk.a

shared: libswk.so

libswk.so: config.h swk.o ${GI_OBJS}
	${CC} ${CFLAGS} -fPIC -shared swk.c ${GI_SRCS} -o libswk.so

swk.o: config.h

libswk.a: config.h swk.o ${GI_OBJS}
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
	echo Libs: -L${PREFIX}/lib -lswk ${GI_LIBS} >> swk.pc
	echo Libs.private: ${PREFIX}/lib/libswk.a ${GI_LIBS} >> swk.pc
	echo Cflags: -I${PREFIX}/include >> swk.pc
