#pragma once
#include "lexer.h"
#include "parser.h"

enum {
  ERROR = 1,
  WARN,
  INFO
};

// Initializes some global variables used by the logger
void init_logger(char* src);

// Prints the null terminated token array
void print_tokens(Token* t);

// Prints the line in the source code
void print_source_line(int line);

// Prints colored error message
void log_msg(int type, char* msg, int line);

// Prints the abstract syntax tree with some indentation
void print_ast(Ast* ast, int indent);
