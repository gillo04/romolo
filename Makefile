example_name = arithmetic

run: all
	./romolo examples/$(example_name).c examples/$(example_name).s
	gcc -m64 ./examples/$(example_name).s -o ./examples/$(example_name)
	./examples/$(example_name) || echo $$?


all:
	gcc -o romolo *.c
