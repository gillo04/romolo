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
  A_PRE_INCREMENT,
  A_PRE_DECREMENT,
  A_ADDRESS,
  A_DEREFERENCE,
  A_UNARY_PLUS,
  A_UNARY_MINUS,
  A_BITWISE_NOT,
  A_LOGIC_NOT,
  A_MULTIPLICATION,
  A_DIVISION,
  A_MODULO
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
  E_ULONG_LONG,

  E_RAW,
  E_POINTER,
  E_ARRAY,
  E_STRUCT,
  E_UNION
};

typedef struct {
  int class;    // raw, pointer, array, ...
  int value;    // int, unsigned int, ...
  union {
    int len;      // array length
    char* name;   // struct/union name
  } info;
} Type;

typedef struct Ast_s Ast;

typedef union {
  Ast* ptr;
  char* str;
  long long num;
} Arg;

struct Ast_s {
  int node_type;
  Type type; // Expression  type
 
  Arg a1;
  Arg a2;
  Arg a3;
};

Ast parser(Token* tokens);
