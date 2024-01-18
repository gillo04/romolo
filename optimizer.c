#include <stdlib.h>
#include <stdio.h>
#include "optimizer.h"
#include "log.h"

void solve_constant_stack(Ast* ast) {
  int i = 0;
  while (ast[i].node_type != A_NONE) {
    solve_constant_exp(&ast[i]);
    i++;
  }
}

int solve_constant_unary(Ast* ast) {
  Ast* ptr1 = ast->a1.ptr;
  if (solve_constant_exp(ptr1)) {
    switch (ast->node_type) {
    case A_UNARY_PLUS:
      ast->a1.num = ptr1->a1.num;
      break;
    case A_UNARY_MINUS:
      ast->a1.num = -ptr1->a1.num;
      break;
    case A_BITWISE_NOT:
      ast->a1.num = ~ptr1->a1.num;
      break;
    case A_LOGIC_NOT:
      ast->a1.num = !ptr1->a1.num;
      break;
    default:
      return 0;
    }

    ast->node_type = A_CONSTANT;
    free (ptr1);
    return 1;
  } else {
    return 0;
  }
}

int solve_constant_binary(Ast* ast) {
  Ast* ptr1 = ast->a1.ptr;
  Ast* ptr2 = ast->a2.ptr;
  int solve1 = solve_constant_exp(ptr1);
  int solve2 = solve_constant_exp(ptr2);
  if (solve1 && solve2) {
    switch (ast->node_type) {
    case A_MULTIPLICATION:
      ast->a1.num = ptr1->a1.num * ptr2->a1.num;
      break;
    case A_DIVISION:
      ast->a1.num = ptr1->a1.num / ptr2->a1.num;
      break;
    case A_MODULO:
      ast->a1.num = ptr1->a1.num % ptr2->a1.num;
      break;
    case A_ADDITION:
      ast->a1.num = ptr1->a1.num + ptr2->a1.num;
      break;
    case A_SUBTRACTION:
      ast->a1.num = ptr1->a1.num - ptr2->a1.num;
      break;
    case A_LEFT_SHIFT:
      ast->a1.num = ptr1->a1.num << ptr2->a1.num;
      break;
    case A_RIGHT_SHIFT:
      ast->a1.num = ptr1->a1.num >> ptr2->a1.num;
      break;
    case A_GREATER:
      ast->a1.num = ptr1->a1.num > ptr2->a1.num;
      break;
    case A_LESS:
      ast->a1.num = ptr1->a1.num < ptr2->a1.num;
      break;
    case A_GREATER_EQUAL:
      ast->a1.num = ptr1->a1.num >= ptr2->a1.num;
      break;
    case A_LESS_EQUAL:
      ast->a1.num = ptr1->a1.num <= ptr2->a1.num;
      break;
    case A_EQUAL:
      ast->a1.num = ptr1->a1.num == ptr2->a1.num;
      break;
    case A_NOT_EQUAL:
      ast->a1.num = ptr1->a1.num != ptr2->a1.num;
      break;
    case A_BITWISE_AND:
      ast->a1.num = ptr1->a1.num & ptr2->a1.num;
      break;
    case A_BITWISE_XOR:
      ast->a1.num = ptr1->a1.num ^ ptr2->a1.num;
      break;
    case A_BITWISE_OR:
      ast->a1.num = ptr1->a1.num | ptr2->a1.num;
      break;
    case A_LOGIC_AND:
      ast->a1.num = ptr1->a1.num && ptr2->a1.num;
      break;
    case A_LOGIC_OR:
      ast->a1.num = ptr1->a1.num || ptr2->a1.num;
      break;
    default:
      return 0;
    }

    ast->node_type = A_CONSTANT;
    free (ptr1);
    free (ptr2);
    return 1;
  } else {
    return 0;
  }
}

int solve_constant_exp(Ast* ast) {
  Ast *ptr1, *ptr2, *ptr3;
  switch (ast->node_type) {
    case A_CONSTANT:
      return 1;
    case A_UNARY_PLUS:
    case A_UNARY_MINUS:
    case A_BITWISE_NOT:
    case A_LOGIC_NOT:
      return solve_constant_unary(ast);
    case A_CONDITIONAL_EXP:
      ptr1 = ast->a1.ptr;
      ptr2 = ast->a2.ptr;
      ptr3 = ast->a3.ptr;
      if (solve_constant_exp(ptr1)) {
        solve_constant_exp(ptr2);
        solve_constant_exp(ptr3);
        *ast = ptr1->a1.num ? *ptr2 : *ptr3;
        free (ptr1);
        free (ptr2);
        free (ptr3);
        return 1;
      }
      return 0;
    case A_COMMA_EXP:
      {
        int i = 0;
        int last;
        while (ast->a1.ptr[i].node_type != A_NONE) {
          last = solve_constant_exp(&ast->a1.ptr[i]);
          i++;
        }
        if (i == 1 && last == 1) {
          ptr1 = ast->a1.ptr;
          ast->a1.num = ast->a1.ptr->a1.num;
          ast->node_type = A_CONSTANT;
          free(ptr1);
          return 1;
        }
      }
      return 0;
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
      return solve_constant_binary(ast);

    case A_CAST_EXPRESSION:
      // TODO: Handle cast expressions
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_FUNCTION_CALL:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_ARGUMENT_EXPRESSION_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;

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
      return 0;

    case A_DECLARATION_SPECIFIERS:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_DECLARATION:
      solve_constant_stack(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_stack(ast->a2.ptr);
      }
      return 0;
    case A_INIT_DECLARATOR:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_INIT_DECLARATOR_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_POINTER:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_DIRECT_DECLARATOR:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_PARAMETER_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_THREE_DOTS:
      return 0;
    case A_IDENTIFIER_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_PARAMETER_DECLARATION:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_PARAMETER_TYPE_LIST:
      solve_constant_stack(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_TYPE_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_stack(ast->a1.ptr);
      }
      return 0;
    case A_ARRAY_DIRECT_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_FUNCTION_DIRECT_DECLARATOR:
      solve_constant_exp(ast->a1.ptr);
      return 0;
    case A_ABSTRACT_DECLARATOR:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_DIRECT_ABSTRACT_DECLARATOR:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_TYPE_NAME:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;

    /*
     * Struct and union declaration
     */
    case A_STRUCT_DECLARATOR:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_STRUCT_DECLARATOR_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_SPECIFIER_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_stack(ast->a1.ptr);
      }
      return 0;
    case A_STRUCT_DECLARATION:
      solve_constant_exp(ast->a1.ptr);
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_STRUCT_DECLARATION_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_STRUCT_SPECIFIER:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_UNION_SPECIFIER:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      solve_constant_exp(ast->a2.ptr);
      return 0;

    /*
     * Enum and typedef specifiers
     */

    case A_ENUMERATOR:
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;
    case A_ENUMERATOR_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_ENUM_SPECIFIER:
      if (ast->a2.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a2.ptr);
      }
      return 0;

    /* 
     * Initializers
     */

    case A_DESIGNATOR_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_INITIALIZER_LIST_ELEMENT:
      if (ast->a1.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a1.ptr);
      }
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_INITIALIZER_LIST:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_INITIALIZER:
      solve_constant_exp(ast->a1.ptr);
      return 0;

    /*
     * Statements
     */
    case A_LABEL:
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_CASE:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_DEFAULT:
      solve_constant_exp(ast->a1.ptr);
      return 0;
    case A_COMPOUND_STATEMENT:
      solve_constant_stack(ast->a1.ptr);
      return 0;
    case A_NULL_STATEMENT:
      return 0;
    case A_IF:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_IF_ELSE:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      solve_constant_exp(ast->a3.ptr);
      return 0;
    case A_SWITCH:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_WHILE:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_DO_WHILE:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;
    case A_FOR:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      return 0;    
    case A_FOR_CLAUSES:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      solve_constant_exp(ast->a3.ptr);
      return 0; 
    case A_GOTO:
    case A_CONTINUE:
    case A_BREAK:
      return 0;
    case A_RETURN:
      solve_constant_exp(ast->a1.ptr);
      return 0;
    case A_EXPRESSION_STATEMENT:
      solve_constant_stack(ast->a1.ptr);
      return 0;

    /*
     * External definitions 
     */
    case A_DECLARATION_LIST:
      solve_constant_stack(ast->a1.ptr);
      break;
    case A_FUNCTION_PROTOTYPE:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      if (ast->a3.ptr->node_type != A_NONE) {
        solve_constant_exp(ast->a3.ptr);
      }
      break;
    case A_FUNCTION_DEFINITION:
      solve_constant_exp(ast->a1.ptr);
      solve_constant_exp(ast->a2.ptr);
      break;
    case A_TRANSLATION_UNIT:
      solve_constant_stack(ast->a1.ptr);
      break;
    default:
      return 0;
  }
}

void optimizer(Ast* ast) {
  solve_constant_exp(ast);
}
