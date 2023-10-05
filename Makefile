run: all
	./romolo examples/arithmetic.c

all:
	gcc -o romolo *.c
