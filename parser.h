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
  A_MODULO,
  A_ADDITION,
  A_SUBTRACTION,
  A_LEFT_SHIFT,
  A_RIGHT_SHIFT,
  A_GRATER,
  A_LESS,
  A_GRATER_EQUAL,
  A_LESS_EQUAL,
  A_EQUAL,
  A_NOT_EQUAL,
  A_BITWISE_AND,
  A_BITWISE_XOR,
  A_BITWISE_OR,
  A_LOGIC_AND,
  A_LOGIC_OR,
  A_CONDITIONAL_EXP,
  A_ASSIGN,
  A_MULT_ASSIGN,
  A_DIV_ASSIGN,
  A_MOD_ASSIGN,
  A_ADD_ASSIGN,
  A_SUB_ASSIGN,
  A_L_SHIFT_ASSIGN,
  A_R_SHIFT_ASSIGN,
  A_AND_ASSIGN,
  A_XOR_ASSIGN,
  A_OR_ASSIGN,
  A_COMMA_EXP,
  A_DECLARATION,
  A_INIT_DECLARATOR,
  A_DECLARATOR,
  A_INITIALIZER,
  A_POINTER,
  A_DIRECT_DECLARATOR,
  A_PARAMETER_TYPE_LIST, 
  A_PARAMETER_LIST,
  A_THREE_DOTS,
  A_IDENTIFIER_LIST,
  A_PARAMETER_DECLARATION,
  A_DIRECT_DECLARATOR_LIST
};

enum {
  E_NONE = 0,

  // Storage class
  E_TYPEDEF,
  E_EXTERN,
  E_STATIC,
  E_AUTO,
  E_REGISTER,

  // Type specifier
  E_VOID,
  E_CHAR,
  E_SHORT,
  E_INT,
  E_LONG,
  E_FLOAT,
  E_DOUBLE,
  E_SIGNED,
  E_UNSIGNED,
  E_BOOL,

  // Typer qualifiers
  E_CONST,
  E_RESTRIC,
  E_VIOLATE,
  
  // Function specifiers
  E_INLINE,
  E_NORETURN,

  // Node category 
  E_RAW,
  E_POINTER,
  E_ARRAY,
  E_STRUCT,
  E_UNION,
  E_DECLARATION_SPECIFIERS
};


typedef struct s_ast Ast;
typedef struct s_type Type;

typedef union {
  Ast* ptr;
  char* str;
  long long num;
} Arg;

struct s_type{
  int node_type;
  int type;
  Type* next;
};

struct s_ast {
  int node_type;
  Type type; // Expression  type
 
  Arg a1;
  Arg a2;
  Arg a3;
};

Ast parser(Token* tokens, char* source);
