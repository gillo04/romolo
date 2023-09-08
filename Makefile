run: all
	./romolo examples/lexing-test.c

all:
	gcc -o romolo *.c
