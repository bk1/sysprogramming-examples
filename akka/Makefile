# automatically generated makefile by ../scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-lpthread -litsky


all: c/closure

clean:
	rm -f lib/libitsky.a  c/closure

c/closure: c/closure.c lib/libitsky.a include/itskylib.h
	gcc $(CFLAGS) c/closure.c $(LIBS) -o c/closure



lib/libitsky.a:
	ar crs lib/libitsky.a
