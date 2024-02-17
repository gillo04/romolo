#pragma once
#include "parser.h"
#include "generator.h"

enum {
  D_NONE = 0,
  D_SCALAR,
  D_ARRAY,
  D_FUNCTION
};

int type_sizeof(Type t);

int is_signed(Type t);

// dec is a declaration
int is_function_decl(Ast* dec);

int is_array(Type t);

int is_pointer(Type t);

int is_integer(Type t);

void prune_pointer(Type t);

// Makes a deep copy of ast
Ast* ast_deep_copy(Ast* ast);
