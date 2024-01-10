run: all
	./romolo examples/arithmetic.c examples/arithmetic.s

all:
	gcc -o romolo *.c
