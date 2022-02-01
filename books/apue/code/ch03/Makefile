
CC = gcc
CFLAGS =
LDFLAGS = 
LDLIBS =

PROGS += test_dup2_1
PROGS += test_dup2_2

all: ${PROGS}

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o test.txt

