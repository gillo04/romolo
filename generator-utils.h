#pragma once
#include "parser.h"
#include "data-structures.h"

void free_type(Type t);

Type type_copy(Type t);

Block integer_promotion(Mem_obj* obj);

// Returns the common type to use in operations between a and b
Type choose_common_type(Type a, Type b);

// Converts the type of obj, also generates the needed assembly
Block convert_type(Mem_obj* obj, Type t);

// Generates code for unary arithmetic operation
Block g_unary_op(Ast* ast, char* op);

// Generates code for binary arithmetic operation
Block g_binary_op(Ast* ast, char* op);

// Generates code for binary logic operation
Block g_binary_logic_op(Ast* ast, char* op);

// Generates code for assignment operation
Block g_assign(Ast* ast, char* op);

// Generates assembly to move src to dest
Block g_mov(Mem_obj* dest, Mem_obj src);

// Generates assembly name of obj
Block g_name(Mem_obj* obj);
