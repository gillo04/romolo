#pragma once
#include "parser.h"
#include "data-structures.h"

Block g_unary_op(Ast* ast, char* op);

Block g_binary_op(Ast* ast, char* op);

Block g_binary_logic_op(Ast* ast, char* op);

Block g_assign(Ast* ast, char* op);

// Generates assembly to move src to dest
Block g_mov(Mem_obj* dest, Mem_obj src);

// Generates assembly name of obj
Block g_name(Mem_obj* obj);
