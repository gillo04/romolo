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

  // Unary operators 
  A_PRE_INCREMENT,
  A_PRE_DECREMENT,
  A_ADDRESS,
  A_DEREFERENCE,
  A_UNARY_PLUS,
  A_UNARY_MINUS,
  A_BITWISE_NOT,
  A_LOGIC_NOT,

  // Binary operators 
  A_MULTIPLICATION,
  A_DIVISION,
  A_MODULO,
  A_ADDITION,
  A_SUBTRACTION,
  A_LEFT_SHIFT,
  A_RIGHT_SHIFT,
  A_GREATER,
  A_LESS,
  A_GREATER_EQUAL,
  A_LESS_EQUAL,
  A_EQUAL,
  A_NOT_EQUAL,
  A_BITWISE_AND,
  A_BITWISE_XOR,
  A_BITWISE_OR,
  A_LOGIC_AND,
  A_LOGIC_OR,

  // Assignment operators and other 
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

  // Statements
  A_LABEL,
  A_CASE,
  A_DEFAULT,
  A_COMPOUND_STATEMENT,
  A_NULL_STATEMENT,
  A_IF,
  A_IF_ELSE,
  A_SWITCH,
  A_WHILE,
  A_DO_WHILE,
  A_FOR,
  A_FOR_CLAUSES,
  A_GOTO,
  A_CONTINUE,
  A_BREAK,
  A_RETURN,
  A_EXPRESSION_STATEMENT,

  A_FUNCTION,
  A_TRANSLATION_UNIT,

  // Declaration specifiers 
  A_TYPEDEF,
  A_EXTERN,
  A_STATIC,
  A_AUTO,
  A_REGISTER,
  A_VOID,
  A_CHAR,
  A_SHORT,
  A_INT,
  A_LONG,
  A_FLOAT,
  A_DOUBLE,
  A_SIGNED,
  A_UNSIGNED,
  A_BOOL,
  A_CONST,
  A_RESTRIC,
  A_VIOLATE,
  A_INLINE,
  A_NORETURN,

  // Declaration and declarator
  A_DECLARATION_SPECIFIERS,
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
  A_DIRECT_DECLARATOR_LIST,
  A_TYPE_QUALIFIER_LIST,
  A_ARRAY_DIRECT_DECLARATOR,
  A_FUNCTION_DIRECT_DECLARATOR
};


typedef struct s_ast Ast;
typedef struct s_type Type;

typedef union {
  Ast* ptr;
  char* str;
  long long num;
} Arg;

/*struct s_type{
  int node_type;
  int type;
  Type* next;
}; */

struct s_ast {
  int node_type;
  // Type type; // Expression  type
 
  Arg a1;
  Arg a2;
  Arg a3;
};

Ast parser(Token* tokens, char* source);
