
CC = gcc
LD = ld
CFLAGS = -I../.. -Wall -std=c99
LDFLAGS = 
LDLIBS =

PROGS += 

all: ${PROGS} sleep2.o

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o
