# The Romolo C compiler
Romolo is a simple C compiler I'm building to better understand the C language. At the moment it only supports the x86_64 architecture on linux.

# Methodology
This compiler was written referencing directly the C11 standard. I have very little prior compiler theory knowledge, so don't take what I have written as an example of how things should be done.
 
# State of the project
At the moment I'm working on the parser. Before implementing declarator parsing I'm going to start work on the code generator, to at least have some basic programs compiling.

# Building and running
To build the compiler execute the command
    
    make all

To run the compiler on a file, run the command
    
    ./romolo path/to/file.c

This will output an assembly file which you can then assemble and link using gcc with the command

    gcc -m64 path/to/file.s -o path/to/executable 

