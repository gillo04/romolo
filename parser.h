#pragma once
#include "lexer.h"

enum {
  A_NONE,
  PROGRAM,
  FUNCTION,       // name, info (-> type, arguments), block
  BLOCK,          // ptr
  CONST_INT,      // num
  RETURN,         // ptr
};

typedef union {
  void* ptr;
  char* str;
  long long num;
} Arg;

typedef struct {
  int type;
  Arg a1;
  Arg a2;
  Arg a3;
} Ast;

Ast* parser(Token* tokens);
