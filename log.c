#include <stdio.h>
#include "log.h"
#include "lexer.h"
#include "parser.h"

void print_tokens(Token* t) {
  while (t->type != T_NONE) {
    printf("%d\t", t->line);
    if (t->type == T_CONSTANT) {
      printf("CONSTANT\t%lld\n", t->val.num);
    } else {
      switch(t->type) {
        case T_KEYWORD:
          printf("KEYWORD\t\t%s\n", t->val.str);
          break;
        case T_IDENTIFIER:
          printf("IDENTIFIER\t%s\n", t->val.str);
          break;
        case T_STRING_LITERAL:
          printf("STRING LITERAL\t%s\n", t->val.str);
          break;
        case T_PUNCTUATOR:
          printf("PUNCTUATOR\t%s\n", t->val.str);
          break;
        default:
          printf("Unrecognized token\n");
          break;
      }
    }
    t++;
  }
}

void print_source_line(char* source, int line) {
  while(source[line] != 0 && source[line] != '\n') {
    putchar(source[line++]);
  }
}

void print_ast(Ast* ast, int indent) {
  for (int i = 0; i < indent; i++) printf("  ");

  switch (ast->node_type) {
    case A_IDENTIFIER:
      printf("IDENTIFIER %s\n", ast->a1.str);
      break;
    case A_CONSTANT:
      printf("CONSTANT %lld\n", ast->a1.num);
      break;
    case A_STRING_LITERAL:
      printf("STRING LITERAL \"%s\"\n", ast->a1.str);
      break;
    case A_MEMBER:
      printf("MEMBER %s\n", ast->a2.str);
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_MEMBER_DEREFERENCE:
      printf("MEMBER DEREFERENCE %s\n", ast->a2.str);
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_POST_INCREMENT:
      printf("POST INCREMENT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_POST_DECREMENT:
      printf("POST DECREMENT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_ARRAY_SUBSCRIPT:
      printf("ARRAY SUBSCRIPT\n");
      print_ast(ast->a1.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("AT INDEX\n");
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_PRE_INCREMENT:
      printf("PRE INCREMENT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_PRE_DECREMENT:
      printf("PRE DECREMENT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_ADDRESS:
      printf("ADDRESS\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_DEREFERENCE:
      printf("DEREFERENCE\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_UNARY_PLUS:
      printf("UNARY PLUS\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_UNARY_MINUS:
      printf("UNARY MINUS\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_BITWISE_NOT:
      printf("BITWISE NOT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_LOGIC_NOT:
      printf("LOGIC NOT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_MULTIPLICATION:
      printf("MULTIPLICATION\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_DIVISION:
      printf("DIVISION\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_MODULO:
      printf("MODULO\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_ADDITION:
      printf("ADDITION\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_SUBTRACTION:
      printf("SUBTRACTION\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LEFT_SHIFT:
      printf("LEFT SHIFT\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_RIGHT_SHIFT:
      printf("RIGHT SHIFT\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_GRATER:
      printf("GRATER\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LESS:
      printf("LESS\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_GRATER_EQUAL:
      printf("GRATER EQUAL\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LESS_EQUAL:
      printf("LESS EQUAL\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_EQUAL:
      printf("EQUAL\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_NOT_EQUAL:
      printf("NOT EQUAL\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_BITWISE_AND:
      printf("BITWISE AND\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_BITWISE_XOR:
      printf("BITWISE XOR\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_BITWISE_OR:
      printf("BITWISE OR\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LOGIC_AND:
      printf("LOGIC AND\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LOGIC_OR:
      printf("LOGIC OR\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_CONDITIONAL_EXP:
      printf("CONDITIONAL EXPRESSION\n");
      print_ast(ast->a1.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("THEN\n");
      print_ast(ast->a2.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("ELSE\n");
      print_ast(ast->a3.ptr, indent+1);
      break;
    case A_ASSIGN:
      printf("ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_MULT_ASSIGN:
      printf("MULTIPLY ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_DIV_ASSIGN:
      printf("DIVISION ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_MOD_ASSIGN:
      printf("MODULO ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_ADD_ASSIGN:
      printf("ADD ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_SUB_ASSIGN:
      printf("SUB ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_L_SHIFT_ASSIGN:
      printf("LEFT SHIFT ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_R_SHIFT_ASSIGN:
      printf("RIGHT SHIFT ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_AND_ASSIGN:
      printf("AND ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_XOR_ASSIGN:
      printf("XOR ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_OR_ASSIGN:
      printf("OR ASSIGN\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_COMMA_EXP:
      printf("COMMA EXPRESSION\n");
      int i = 0;
      while (ast->a1.ptr[i].node_type != A_NONE) {
        print_ast(&ast->a1.ptr[i], indent+1);
        i++;
      }
      break;
    case A_DECLARATION:
      printf("DECLARATION < ");
      print_type(&ast->type);
      printf(">\n");
      break;
    default:
      printf("Couldn't recognize type %d\n", ast->node_type);
  }
}

void print_type(Type* t) {
  if (t == 0 || t->node_type == E_NONE) {
    return;
  }

  switch (t->type) {
    case E_TYPEDEF:
      printf("TYPEDEF ");
      print_type(t->next);
      break;
    case E_EXTERN:
      printf("EXTERN ");
      print_type(t->next);
      break;
    case E_STATIC:
      printf("STATIC ");
      print_type(t->next);
      break;
    case E_AUTO:
      printf("AUTO ");
      print_type(t->next);
      break;
    case E_REGISTER:
      printf("REGISTER ");
      print_type(t->next);
      break;
    case E_VOID:
      printf("VOID ");
      print_type(t->next);
      break;
    case E_CHAR:
      printf("CHAR ");
      print_type(t->next);
      break;
    case E_SHORT:
      printf("SHORT ");
      print_type(t->next);
      break;
    case E_INT:
      printf("INT ");
      print_type(t->next);
      break;
    case E_LONG:
      printf("LONG ");
      print_type(t->next);
      break;
    case E_FLOAT:
      printf("FLOAT ");
      print_type(t->next);
      break;
    case E_DOUBLE:
      printf("DOUBLE ");
      print_type(t->next);
      break;
    case E_SIGNED:
      printf("SIGNED ");
      print_type(t->next);
      break;
    case E_UNSIGNED:
      printf("UNSIGNED ");
      print_type(t->next);
      break;
    case E_BOOL:
      printf("BOOL ");
      print_type(t->next);
      break;
    case E_CONST:
      printf("CONST ");
      print_type(t->next);
      break;
    case E_RESTRIC:
      printf("RESTRICT ");
      print_type(t->next);
      break;
    case E_VIOLATE:
      printf("VIOLATE ");
      print_type(t->next);
      break;
    case E_INLINE:
      printf("INLINE ");
      print_type(t->next);
      break;
    case E_NORETURN:
      printf("NORETURN ");
      print_type(t->next);
      break;
    default:
      printf("Couldn't recognize type %d\n", t->node_type);
  }
}

