#include "log.h" 
#include "parser.h" 
#include "verify-utils.h"
#include <stdio.h>
#include <stdlib.h>

int type_sizeof(Ast* dec_spec, Ast* dec, int ptr_stair) {
  int size = 0;

  // Descend pointer list
  Ast* cur = dec->a1.ptr;
  for (int i = 0; i < ptr_stair && cur->node_type != A_NONE; i++) {
    cur = cur->a2.ptr;
  }

  if (cur->node_type == A_NONE) {
    // Count declaration specifiers
    int c_char = 0, c_short = 0, c_int = 0, c_long = 0;
    int i = 0;
    while (dec_spec->a1.ptr[i].node_type != A_NONE) {
      switch(dec_spec->a1.ptr[i].node_type) {
        case A_CHAR:
          c_char++;
          break;
        case A_SHORT:
          c_short++;
          break;
        case A_INT:
          c_int++;
          break;
        case A_LONG:
          c_long++;
          break;
      }
      i++;
    }

    if (c_char == 1) {
      size = 1;
    } else if (c_short == 1) {
      size = 2;
    } else if (c_long == 1 || c_long == 2) {
      size = 8;
    } else if (c_int == 1) {
      size = 4;
    }
  } else {
    // Pointer size
    size = 8;
  }

  // Multiply by array sizes
  int i = 1;
  while (dec->a2.ptr->a1.ptr[i].node_type != A_NONE) {
    if (dec->a2.ptr->a1.ptr[i].node_type == A_ARRAY_DIRECT_DECLARATOR) {
      size *= dec->a2.ptr->a1.ptr[i].a2.ptr->a1.num;
    }
    i++;
  }


  return size;
}

int declaration_type(Ast* dec) {
  Ast* dd = &dec->a2.ptr->a1.ptr->a1.ptr->a2.ptr->a1.ptr[1];
  if (dd->node_type == A_NONE) {
    return D_SCALAR;
  } else if (dd->node_type == A_ARRAY_DIRECT_DECLARATOR) {
    return D_ARRAY;
  } else if (dd->node_type == A_FUNCTION_DIRECT_DECLARATOR) {
    return D_FUNCTION;
  }

  return D_NONE;
}

Ast* ast_stack_deep_copy(Ast* ast) {
  Ast* out = (Ast*) malloc(sizeof(Ast));
  int i = 0;
  while (ast[i].node_type != A_NONE) {
    Ast* tmp = ast_deep_copy(&ast[i]);
    out[i] = *tmp;
    free(tmp);

    i++;
    out = (Ast*) realloc(out, sizeof(Ast) * (i+1));
  }
  Ast* tmp = ast_deep_copy(&ast[i]);
  out[i] = *tmp;
  free(tmp);
  return out;
}

Ast* ast_deep_copy(Ast* ast) {
  Ast* out = (Ast*) malloc(sizeof(Ast));
  out->node_type = ast->node_type;
  switch (ast->node_type) {
    case A_NONE:
      break;
    case A_IDENTIFIER:
      ast_strcpy(&out->a1.str, ast->a1.str);
      break;
    case A_CONSTANT:
      break;
    case A_STRING_LITERAL:
      ast_strcpy(&out->a1.str, ast->a1.str);
      break;
    case A_MEMBER:
      ast_strcpy(&out->a2.str, ast->a2.str);
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_MEMBER_DEREFERENCE:
      ast_strcpy(&out->a2.str, ast->a2.str);
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;

    /*
     * Unary operators
     */

    case A_POST_INCREMENT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_POST_DECREMENT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_ARRAY_SUBSCRIPT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_PRE_INCREMENT:
    case A_PRE_DECREMENT:
    case A_ADDRESS:
    case A_DEREFERENCE:
    case A_UNARY_PLUS:
    case A_UNARY_MINUS:
    case A_BITWISE_NOT:
    case A_LOGIC_NOT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;

    /*
     * Binary operators
     */

    case A_MULTIPLICATION:
    case A_DIVISION:
    case A_MODULO:
    case A_ADDITION:
    case A_SUBTRACTION:
    case A_LEFT_SHIFT:
    case A_RIGHT_SHIFT:
    case A_GREATER:
    case A_LESS:
    case A_GREATER_EQUAL:
    case A_LESS_EQUAL:
    case A_EQUAL:
    case A_NOT_EQUAL:
    case A_BITWISE_AND:
    case A_BITWISE_XOR:
    case A_BITWISE_OR:
    case A_LOGIC_AND:
    case A_LOGIC_OR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;

    /*
     * Assignment operators and other 
     */

    case A_CONDITIONAL_EXP:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      out->a3.ptr = ast_deep_copy(ast->a3.ptr);
      break;
    case A_ASSIGN:
    case A_MULT_ASSIGN:
    case A_DIV_ASSIGN:
    case A_MOD_ASSIGN:
    case A_ADD_ASSIGN:
    case A_SUB_ASSIGN:
    case A_L_SHIFT_ASSIGN:
    case A_R_SHIFT_ASSIGN:
    case A_AND_ASSIGN:
    case A_XOR_ASSIGN:
    case A_OR_ASSIGN:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_COMMA_EXP:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_CAST_EXPRESSION:
    case A_FUNCTION_CALL:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_ARGUMENT_EXPRESSION_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
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
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_DECLARATION:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_INIT_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_INIT_DECLARATOR_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_POINTER:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_DIRECT_DECLARATOR:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_PARAMETER_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_THREE_DOTS:
      break;
    case A_IDENTIFIER_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_PARAMETER_DECLARATION:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_PARAMETER_TYPE_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_TYPE_QUALIFIER_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_ARRAY_DIRECT_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_FUNCTION_DIRECT_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_ABSTRACT_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_DIRECT_ABSTRACT_DECLARATOR:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_TYPE_NAME:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;

    /*
     * Struct and union declaration
     */
    case A_STRUCT_DECLARATOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_STRUCT_DECLARATOR_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_SPECIFIER_QUALIFIER_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_STRUCT_DECLARATION:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_STRUCT_DECLARATION_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_STRUCT_SPECIFIER:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_UNION_SPECIFIER:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;

    /*
     * Enum and typedef specifiers
     */

    case A_ENUMERATOR:
      ast_strcpy(&out->a1.str, ast->a1.str);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_ENUMERATOR_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_ENUM_SPECIFIER:
      if (ast->a1.str != 0) {
        ast_strcpy(&out->a1.str, ast->a1.str);
      }

      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;

    /* 
     * Initializers
     */

    case A_DESIGNATOR_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_INITIALIZER_LIST_ELEMENT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_INITIALIZER_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_INITIALIZER:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;

    /*
     * Statements
     */
    case A_LABEL:
      ast_strcpy(&out->a1.str, ast->a1.str);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_CASE:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_DEFAULT:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_COMPOUND_STATEMENT:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_NULL_STATEMENT:
      break;
    case A_IF:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_IF_ELSE:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      out->a3.ptr = ast_deep_copy(ast->a3.ptr);
      break;
    case A_SWITCH:
    case A_WHILE:
    case A_DO_WHILE:
    case A_FOR:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;    
    case A_FOR_CLAUSES:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      out->a3.ptr = ast_deep_copy(ast->a3.ptr);
      break; 
    case A_GOTO:
      ast_strcpy(&out->a1.str, ast->a1.str);
    case A_CONTINUE:
    case A_BREAK:
      break;
    case A_RETURN:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      break;
    case A_EXPRESSION_STATEMENT:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;

    /*
     * External definitions 
     */
    case A_DECLARATION_LIST:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    case A_FUNCTION_PROTOTYPE:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      out->a3.ptr = ast_deep_copy(ast->a3.ptr);
      break;
    case A_FUNCTION_DEFINITION:
      out->a1.ptr = ast_deep_copy(ast->a1.ptr);
      out->a2.ptr = ast_deep_copy(ast->a2.ptr);
      break;
    case A_TRANSLATION_UNIT:
      out->a1.ptr = ast_stack_deep_copy(ast->a1.ptr);
      break;
    default:
      log_msg(ERROR, "error copying ast\n", -1);
      printf("\tCouldn't recognize type %d\n", ast->node_type);
  }

  return out;
}
