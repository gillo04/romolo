#pragma once
#include "lexer.h"

enum {
  A_NONE,
  A_IDENTIFIER,
  A_CONSTANT,
  A_STRING_LITERAL,
};

enum {
  E_NONE = 0,
  E_CHAR,
  E_UCHAR,
  E_SHORT,
  E_USHORT,
  E_INT,
  E_UINT,
  E_LONG,
  E_ULONG,
  E_LONG_LONG,
  E_ULONG_LONG
};

typedef union {
  void* ptr;
  char* str;
  long long num;
} Arg;

typedef struct {
  int node_type;
  int exp_type; // Expressio  type
  Arg a1;
  Arg a2;
  Arg a3;
} Ast;

Ast parser(Token* tokens);
