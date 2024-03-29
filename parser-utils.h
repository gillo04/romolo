#pragma once
#include "lexer.h"
#include "parser.h"

void ast_strcpy(char** dest, char* src);

void astcpy(Ast** dest, Ast src);

int tokcmp(Token a, Token b);

Ast m_comma_list(int* i, Ast(*match)(int*), int node_type);

void free_ast(Ast* ast, int must_free);
