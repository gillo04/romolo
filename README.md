# The Romolo C compiler
Romolo is a simple C compiler I'm building to better understand the C language. At the moment it only supports the x86_64 architecture on linux.

# Methodology
This compiler was written referencing directly the C11 standard. I have very little prior compiler theory knowledge, so don't take what I have written as an example of how things should be done.
 
# State of the project
After finishing the parser, I'm going to focus on getting code generation to a decent point, then I will start to think about validating the output of the parser and fixing the infinite memory leaks.

# Building and running
To build the compiler execute the command
    
    make all

To run the compiler on a file, run the command
    
    ./romolo path/to/file.c

This will output an assembly file which you can then assemble and link using gcc with the command

    gcc -m64 path/to/file.s -o path/to/executable 

