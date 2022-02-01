
CC = gcc
CFLAGS = -I../.. -Wall -Wno-div-by-zero
LDFLAGS = 
LDLIBS =

PROGS += test_getpid
PROGS += test_fork
PROGS += test_vfork
PROGS += test_wait
PROGS += test_waitpid
PROGS += test_waitid
PROGS += test_exec
PROGS += test_system
PROGS += test_getlogin
PROGS += test_nice

all: ${PROGS}

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o
