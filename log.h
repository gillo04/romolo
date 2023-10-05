#pragma once
#include "lexer.h"
#include "parser.h"

// Prints the null terminated token array
void print_tokens(Token* t);

// Prints the line in the source code
void print_source_line(char* source, int line);

// Prints the abstract syntax tree with some indentation
void print_ast(Ast* ast, int indent);
