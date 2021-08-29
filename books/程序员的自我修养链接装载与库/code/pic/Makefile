CC = gcc
CFLAGS = -shared -fPIC

all: pic.so

pic.so: pic.c
	${CC} ${CFLAGS} -o $@ $<

.PHONY: clean
clean:
	rm -rf pic.so