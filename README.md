# The Romolo C compiler
Romolo is a simple C compiler I'm building to better understand the C language. At the moment it only supports the x86_64 architecture on linux.

# Methodology
This compiler was written referencing directly the C11 standard. I have very little prior compiler theory knowledge, so don't take what I have written as an example of how things should be done.
 
# State of the project
I'm working on code generation. Right now the priority is getting a decent declaration generator. After this, I will start to think about validating the output of the parser and fixing the infinite memory leaks.

# Building and running
To build the compiler execute the command
    
    make all

To run the compiler on a file, run the command
    
    ./romolo path/to/source.c path/to/output.s

This will output an assembly file which you can then assemble and link using gcc with the commands

	nasm -f elf64 path/to/output.s -o path/to/object.o
	ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc -e main path/to/object.o -o path/to/executable
    # Optional
	rm path/to/object.o

