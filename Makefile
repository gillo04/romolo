example_name = test

run: all
	./romolo examples/$(example_name).c examples/$(example_name).s
	nasm -f elf64 examples/$(example_name).s -o examples/$(example_name).o
	ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc examples/$(example_name).o -o examples/$(example_name)
	rm examples/$(example_name).o
	./examples/$(example_name) || echo $$?


all:
	gcc -o romolo *.c

valgrind:
	gcc -g -Og -o romolo *.c
	valgrind ./romolo examples/$(example_name).c examples/$(example_name).s
	#valgrind --leak-check=full ./romolo examples/$(example_name).c examples/$(example_name).s
