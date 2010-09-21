
CC			= gcc
CFLAGS		+= -g -O2
CFLAGS		+= -std=gnu99
DESTDIR		= /usr/local

all				:	ejson_test

ejson_test		:	ejson_test.c libejson.a

libejson.a		:	ejson.o
	libtool -static -o $@ $<

ejson.c	: 	ejson.rl ejson.h
	ragel -G1 -e $<

clean			:
	rm -rf ejson_test libejson.a *.dSYM *.o

install			:
	install libejson.a $(DESTDIR)/lib
	install ejson.h $(DESTDIR)/include
	