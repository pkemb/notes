
CC = gcc
LD = ld
CFLAGS = -I../.. -Wall -std=c99 -D_POSIX_C_SOURCE=199309L -Wno-unused-variable
LDFLAGS = 
LDLIBS =

PROGS += test_signal
PROGS += test_kill
PROGS += test_sleep2
PROGS += test_sigaction
PROGS += test_abort
PROGS += test_system
PROGS += test_sigqueue
PROGS += test_signo

all: ${PROGS} sleep2.o

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

test_sleep2: test_sleep2.o sleep2.o
	$(CC) $(CFLAGS) -o test_sleep2 test_sleep2.o sleep2.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o
