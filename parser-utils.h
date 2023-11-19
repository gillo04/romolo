#pragma once
#include "lexer.h"
#include "parser.h"

void ast_strcpy(char** dest, char* src);

void astcpy(Ast** dest, Ast src);

int tokcmp(Token a, Token b);
