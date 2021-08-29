PROGRAM+=cpuid
PROGRAM+=cpuid2_x86
USEGCC=0
all: $(PROGRAM)

cpuid: cpuid.s
ifeq ($(USEGCC),1)
	gcc -o $@ $< -no-pie
else
	as -g -o $@.o $<
	ld -o $@ $@.o -e main
endif

cpuid2_x86: cpuid2_x86.s
ifeq ($(USEGCC),1)
	gcc -o $@ $< -no-pie -g
else
	as -g -o $@.o $<
	ld -o $@ $@.o -e main -lc -dynamic-linker /lib/ld-linux.so.2
endif

.PHONY: clean
clean:
	rm -rf $(PROGRAM) $(addsuffix .o, $(PROGRAM))
