# automatically generated makefile by ../../scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-lpthread -lrt 


all: closure

clean:
	rm -f lib/libitsky.a  closure

closure: closure.c 
	gcc $(CFLAGS) closure.c $(LIBS) -o closure

