.PHONY: all t clean install

-include config.mk

VERSION=0.1
# graphic backend
GI?=sdl
ifeq (${GI},sdl)
GI_LIBS=-lSDL -lSDL_ttf -lSDL_image
else
ifeq (${GI},x11)
GI_LIBS=-lX11 -ldraw
endif
endif

GI_OBJS=gi_${GI}.o
GI_SRCS=gi_${GI}.c

all: config.mk static shared t

x: config.mk
	make clean ; make GI=x11 && cd t ; ./test

s: config.mk
	make clean ; make GI=sdl && cd t ; ./test

t: config.mk
	cd t && ${MAKE} all

config.h:
	cp config.def.h config.h

config.mk: config.h
	cp config.def.mk config.mk

clean:
	echo >swk.mk
	cd t && ${MAKE} clean
	rm -f swk.pc swk.mk libswk.a libswk.so swk.o ${GI_OBJS}

install:
	mkdir -p ${DESTDIR}/${INCDIR}
	cp swk.h ${DESTDIR}/${INCDIR}
	mkdir -p ${DESTDIR}/${LIBDIR}
	cp libswk.a ${DESTDIR}/${LIBDIR}
	cp libswk.so ${DESTDIR}/${LIBDIR}
	mkdir -p ${DESTDIR}/${LIBDIR}/mk
	cp swk.mk ${DESTDIR}/${LIBDIR}/mk/swk.mk
	mkdir -p ${DESTDIR}/${LIBDIR}/pkgconfig
	cp swk.pc ${DESTDIR}/${LIBDIR}/pkgconfig/swk.pc

deinstall uninstall:
	rm -f ${DESTDIR}/${INCDIR}/swk.h
	rm -f ${DESTDIR}/${LIBDIR}/libswk.a
	rm -f ${DESTDIR}/${LIBDIR}/libswk.so
	rm -f ${DESTDIR}/${LIBDIR}/mk/swk.mk
	rm -f ${DESTDIR}/${LIBDIR}/pkgconfig/swk.pc

static: libswk.a

shared: libswk.so

libswk.so: config.mk swk.o ${GI_OBJS}
	${CC} ${CFLAGS} -fPIC -shared swk.c ${GI_SRCS} -o libswk.so

swk.o: config.mk

libswk.a: config.mk swk.o ${GI_OBJS}
	rm -f libswk.a
	ar qcvf libswk.a swk.o ${GI_OBJS}
	echo SWKINCS+=-I${PREFIX}/include > swk.mk
	echo SWKLIB+=${PREFIX}/lib/libswk.a >> swk.mk
	echo SWKLIBS+=${GI_LIBS} >> swk.mk
	echo prefix=${PREFIX} > swk.pc
	echo libdir=${LIBDIR} >> swk.pc
	echo >> swk.pc
	echo Name: swk >> swk.pc
	echo Version: ${VERSION} >> swk.pc
	echo Description: simple widget kit >> swk.pc
	echo Libs: -L${PREFIX}/lib -lswk ${GI_LIBS} >> swk.pc
	echo Libs.private: ${PREFIX}/lib/libswk.a ${GI_LIBS} >> swk.pc
	echo Cflags: -I${PREFIX}/include >> swk.pc
