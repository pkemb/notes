CC=gcc
LD=ld
PROGRAM += hello_x86
PROGRAM += hello_x64
PROGRAM += hello_arm

all: ${PROGRAM}

%: %.c
	${CC} -c -g -fno-builtin $<
	${LD} -static -e nomain $@.o -o $@

clean:
	rm -rf ${PROGRAM} $(addsuffix .o, ${PROGRAM})