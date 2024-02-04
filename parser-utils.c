#include "log.h"
#include "parser-utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

void ast_strcpy(char** dest, char* src) {
  *dest = (char*) malloc(strlen(src));
  strcpy(*dest, src);
}

void astcpy(Ast** dest, Ast src) {
  *dest = (Ast*) malloc(sizeof(Ast));
  **dest = src;
}

int tokcmp(Token a, Token b) {
  if (a.type == T_NONE || b.type == T_NONE) {
    return 0;
  }

  if (a.type == T_CONSTANT) {
    return a.val.num == b.val.num && a.type == b.type;
  } else {
    return strcmp(a.val.str, b.val.str) == 0 && a.type == b.type;
  }
}

Ast m_comma_list(int* i, Ast(*match)(int*), int node_type) {
  int j = *i;
  Ast out = {node_type};
  Ast assign = match(&j);

  if (assign.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  int k = 1;
  out.a1.ptr = malloc(sizeof(Ast));
  while (assign.node_type != A_NONE) {
    *i = j;
    out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
    out.a1.ptr[k-1] = assign;
    k++;
    if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, ","})) {
      j++;
      assign = match(&j);
    } else {
      assign.node_type = A_NONE;
    }
  }
  out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
  out.a1.ptr[k-1] = (Ast) {A_NONE};
  return out;
}

Ast m_list(int* i, Ast(*match)(int*), int node_type) {
  int j = *i;
  Ast out = {node_type};
  Ast assign = match(&j);

  if (assign.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  int k = 1;
  out.a1.ptr = malloc(sizeof(Ast));
  while (assign.node_type != A_NONE) {
    *i = j;
    out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
    out.a1.ptr[k-1] = assign;
    k++;
    j++;
  }
  out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
  out.a1.ptr[k-1] = (Ast) {A_NONE};
  return out;
}

void free_ast_stack(Ast* ast) {
  int i = 0;
  while (ast[i].node_type != A_NONE) {
    free_ast(&ast[i], 0);
    i++;
  }
  free_ast(&ast[i], 0);
  free(ast);
}

void free_ast(Ast* ast, int must_free) {
  switch (ast->node_type) {
    case A_NONE:
      break;
    case A_IDENTIFIER:
      free(ast->a1.str);
      break;
    case A_CONSTANT:
      break;
    case A_STRING_LITERAL:
      free(ast->a1.str);
      break;
    case A_MEMBER:
      free(ast->a2.str);
      free_ast(ast->a1.ptr, 1);
      break;
    case A_MEMBER_DEREFERENCE:
      free(ast->a2.str);
      free_ast(ast->a1.ptr, 1);
      break;

    /*
     * Unary operators
     */

    case A_POST_INCREMENT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_POST_DECREMENT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_ARRAY_SUBSCRIPT:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_PRE_INCREMENT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_PRE_DECREMENT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_ADDRESS:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_DEREFERENCE:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_UNARY_PLUS:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_UNARY_MINUS:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_BITWISE_NOT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_LOGIC_NOT:
      free_ast(ast->a1.ptr, 1);
      break;

    /*
     * Binary operators
     */

    case A_MULTIPLICATION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_DIVISION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_MODULO:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_ADDITION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_SUBTRACTION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_LEFT_SHIFT:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_RIGHT_SHIFT:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_GREATER:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_LESS:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_GREATER_EQUAL:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_LESS_EQUAL:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_EQUAL:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_NOT_EQUAL:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_BITWISE_AND:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_BITWISE_XOR:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_BITWISE_OR:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_LOGIC_AND:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_LOGIC_OR:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;

    /*
     * Assignment operators and other 
     */

    case A_CONDITIONAL_EXP:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      free_ast(ast->a3.ptr, 1);
      break;
    case A_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_MULT_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_DIV_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_MOD_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_ADD_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_SUB_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_L_SHIFT_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_R_SHIFT_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_AND_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_XOR_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_OR_ASSIGN:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_COMMA_EXP:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_CAST_EXPRESSION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_FUNCTION_CALL:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_ARGUMENT_EXPRESSION_LIST:
      free_ast_stack(ast->a1.ptr);
      break;

    /*
     * Declarator and declaration 
     */
    case A_TYPEDEF:
    case A_EXTERN:
    case A_STATIC:
    case A_AUTO:
    case A_REGISTER:
    case A_VOID:
    case A_CHAR:
    case A_SHORT:
    case A_INT:
    case A_LONG:
    case A_FLOAT:
    case A_DOUBLE:
    case A_SIGNED:
    case A_UNSIGNED:
    case A_BOOL:
    case A_CONST:
    case A_RESTRIC:
    case A_VIOLATE:
    case A_INLINE:
    case A_NORETURN:
      break;

    case A_DECLARATION_SPECIFIERS:
      if (ast->a1.ptr->node_type == A_STRUCT_SPECIFIER || ast->a1.ptr->node_type == A_UNION_SPECIFIER) {
        free_ast_stack(ast->a1.ptr);
      } else {
        free_ast_stack(ast->a1.ptr);
      }
      break;
    case A_DECLARATION:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_INIT_DECLARATOR:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_INIT_DECLARATOR_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      free_ast(ast->a2.ptr, 1);
      break;
    case A_POINTER:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_DIRECT_DECLARATOR:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_PARAMETER_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_THREE_DOTS:
      break;
    case A_IDENTIFIER_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_PARAMETER_DECLARATION:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_PARAMETER_TYPE_LIST:
      free_ast_stack(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_TYPE_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast_stack(ast->a1.ptr);
      }
      break;
    case A_ARRAY_DIRECT_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_FUNCTION_DIRECT_DECLARATOR:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_ABSTRACT_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_DIRECT_ABSTRACT_DECLARATOR:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_TYPE_NAME:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;

    /*
     * Struct and union declaration
     */
    case A_STRUCT_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_STRUCT_DECLARATOR_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_SPECIFIER_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast_stack(ast->a1.ptr);
      } 
      break;
    case A_STRUCT_DECLARATION:
      free_ast(ast->a1.ptr, 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_STRUCT_DECLARATION_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_STRUCT_SPECIFIER:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      free_ast(ast->a2.ptr, 1);
      break;
    case A_UNION_SPECIFIER:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      free_ast(ast->a2.ptr, 1);
      break;

    /*
     * Enum and typedef specifiers
     */

    case A_ENUMERATOR:
      free(ast->a1.str);
      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;
    case A_ENUMERATOR_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_ENUM_SPECIFIER:
      if (ast->a1.str != 0) {
        free(ast->a1.str);
      }

      if (ast->a2.ptr->node_type != A_NONE) {
        free_ast(ast->a2.ptr, 1);
      }
      break;

    /* 
     * Initializers
     */

    case A_DESIGNATOR_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_INITIALIZER_LIST_ELEMENT:
      if (ast->a1.ptr->node_type != A_NONE) {
        free_ast(ast->a1.ptr, 1);
      }
      free_ast(ast->a2.ptr, 1);
      break;
    case A_INITIALIZER_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_INITIALIZER:
      free_ast(ast->a1.ptr, 1);
      break;

    /*
     * Statements
     */
    case A_LABEL:
      free(ast->a1.str);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_CASE:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_DEFAULT:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_COMPOUND_STATEMENT:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_NULL_STATEMENT:
      break;
    case A_IF:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_IF_ELSE:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      free_ast(ast->a3.ptr, 1);
      break;
    case A_SWITCH:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_WHILE:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_DO_WHILE:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_FOR:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;    
    case A_FOR_CLAUSES:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      free_ast(ast->a3.ptr, 1);
      break; 
    case A_GOTO:
      free(ast->a1.str);
    case A_CONTINUE:
    case A_BREAK:
      break;
    case A_RETURN:
      free_ast(ast->a1.ptr, 1);
      break;
    case A_EXPRESSION_STATEMENT:
      free_ast_stack(ast->a1.ptr);
      break;

    /*
     * External definitions 
     */
    case A_DECLARATION_LIST:
      free_ast_stack(ast->a1.ptr);
      break;
    case A_FUNCTION_PROTOTYPE:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      if (ast->a3.ptr->node_type != A_NONE) {
        free_ast(ast->a3.ptr, 1);
      }
      break;
    case A_FUNCTION_DEFINITION:
      free_ast(ast->a1.ptr, 1);
      free_ast(ast->a2.ptr, 1);
      break;
    case A_TRANSLATION_UNIT:
      free_ast_stack(ast->a1.ptr);
      break;
    default:
      log_msg(ERROR, "error freeing ast\n", -1);
      printf("\tCouldn't recognize type %d\n", ast->node_type);
  }

  if (must_free) {
    free(ast);
  }
}
