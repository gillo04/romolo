#pragma once
#include "parser.h"

enum {
  D_NONE = 0,
  D_SCALAR,
  D_ARRAY,
  D_FUNCTION
};

// Accepts declaration specfiers and a declarator
int type_sizeof(Ast* dec_spec, Ast* dec, int ptr_stair);

// Accepts a declaration
int declaration_type(Ast* dec);

