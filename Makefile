example_name = fibonacci

run: all
	./romolo examples/$(example_name).c examples/$(example_name).s
	nasm -f elf64 examples/$(example_name).s -o examples/$(example_name).o
	ld -e main examples/$(example_name).o -o examples/$(example_name)
	./examples/$(example_name) || echo $$?


all:
	gcc -o romolo *.c
