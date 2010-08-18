CC?=gcc
CFLAGS?=-Wall -g -std=c99
DESTDIR?=
PREFIX?=${DESTDIR}/usr/local
INCDIR?=${PREFIX}/include
LIBDIR?=${PREFIX}/lib
CFLAGS+=-I.
