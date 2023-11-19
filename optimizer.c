#include <stdlib.h>
#include <stdio.h>
#include "optimizer.h"
#include "log.h"

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
    case A_GRATER:
      ast->a1.num = ptr1->a1.num > ptr2->a1.num;
      break;
    case A_LESS:
      ast->a1.num = ptr1->a1.num < ptr2->a1.num;
      break;
    case A_GRATER_EQUAL:
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
        while (ast->a1.ptr[i].node_type != A_NONE) {
          optimizer(&ast->a1.ptr[i]);
          i++;
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
    case A_GRATER:
    case A_LESS:
    case A_GRATER_EQUAL:
    case A_LESS_EQUAL:
    case A_EQUAL:
    case A_NOT_EQUAL:
    case A_BITWISE_AND:
    case A_BITWISE_XOR:
    case A_BITWISE_OR:
    case A_LOGIC_AND:
    case A_LOGIC_OR:
      return solve_constant_binary(ast);
    default:
      return 0;
  }
}

void optimizer(Ast* ast) {
  solve_constant_exp(ast);
}
