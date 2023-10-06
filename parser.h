#pragma once
#include "lexer.h"

enum {
  A_NONE,
  A_IDENTIFIER,
  A_CONSTANT,
  A_STRING_LITERAL,
  A_MEMBER,
  A_MEMBER_DEREFERENCE,
  A_POST_INCREMENT,
  A_POST_DECREMENT,
  A_ARRAY_SUBSCRIPT,
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

typedef struct Ast_s Ast;

typedef union {
  Ast* ptr;
  char* str;
  long long num;
} Arg;

struct Ast_s {
  int node_type;
  int exp_type; // Expressio  type
  Arg a1;
  Arg a2;
  Arg a3;
};

Ast parser(Token* tokens);
