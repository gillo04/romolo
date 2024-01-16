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

void print_ast_stack(Ast* ast, int indent) {
  int i = 0;
  while (ast[i].node_type != A_NONE) {
    print_ast(&ast[i], indent);
    i++;
  }
}

void print_ast(Ast* ast, int indent) {
  if (ast->node_type < A_TYPEDEF || ast->node_type > A_NORETURN) {
    for (int i = 0; i < indent; i++) printf("  ");
  }

  switch (ast->node_type) {
    case A_NONE:
      printf("NONE\n");
      break;
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

    /*
     * Unary operators
     */

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

    /*
     * Binary operators
     */

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
    case A_GREATER:
      printf("GREATER\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_LESS:
      printf("LESS\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_GREATER_EQUAL:
      printf("GREATER EQUAL\n");
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

    /*
     * Assignment operators and other 
     */

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
      print_ast_stack(ast->a1.ptr, indent+1);
      break;

    /*
     * Declarator and declaration 
     */
    case A_TYPEDEF:
      printf("TYPEDEF ");
      break;
    case A_EXTERN:
      printf("EXTERN ");
      break;
    case A_STATIC:
      printf("STATIC ");
      break;
    case A_AUTO:
      printf("AUTO ");
      break;
    case A_REGISTER:
      printf("REGISTER ");
      break;
    case A_VOID:
      printf("VOID ");
      break;
    case A_CHAR:
      printf("CHAR ");
      break;
    case A_SHORT:
      printf("SHORT ");
      break;
    case A_INT:
      printf("INT ");
      break;
    case A_LONG:
      printf("LONG ");
      break;
    case A_FLOAT:
      printf("FLOAT ");
      break;
    case A_DOUBLE:
      printf("DOUBLE ");
      break;
    case A_SIGNED:
      printf("SIGNED ");
      break;
    case A_UNSIGNED:
      printf("UNSIGNED ");
      break;
    case A_BOOL:
      printf("BOOL ");
      break;
    case A_CONST:
      printf("CONST ");
      break;
    case A_RESTRIC:
      printf("RESTRICT ");
      break;
    case A_VIOLATE:
      printf("VIOLATE ");
      break;
    case A_INLINE:
      printf("INLINE ");
      break;
    case A_NORETURN:
      printf("NORETURN ");
      break;

    case A_DECLARATION_SPECIFIERS:
      printf("DECLARATION SPECIFIERS\n");
      for (int i = 0; i < indent+1; i++) printf("  ");
      print_ast_stack(ast->a1.ptr, 0);
      printf("\n");
      break;
    case A_DECLARATION:
      printf("DECLARATION\n");
      print_ast_stack(ast->a1.ptr, indent + 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast_stack(ast->a2.ptr, indent + 1);
      }
      break;
    case A_INIT_DECLARATOR:
      printf("INIT DECLARATOR\n");
      print_ast(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_DECLARATOR:
      printf("DECLARATOR\n");
      if (ast->a1.ptr->node_type != A_NONE) {
        print_ast(ast->a1.ptr, indent+1);
      }
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_INITIALIZER:
      printf("INITIALIZER\n");
      break;
    case A_POINTER:
      printf("POINTER\n");
      print_ast(ast->a1.ptr, indent + 1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent + 1);
      }
      break;
    case A_DIRECT_DECLARATOR:
      printf("DIRECT DECLARATOR\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_PARAMETER_LIST:
      printf("PARAMETER LIST\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_THREE_DOTS:
      printf("THREE DOTS\n");
      break;
    case A_IDENTIFIER_LIST:
      printf("IDENTIFIER LIST\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_PARAMETER_DECLARATION:
      printf("PARAMETER DECLARATION\n");
      print_ast(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_PARAMETER_TYPE_LIST:
      printf("PARAMETER TYPE LIST\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_TYPE_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        printf("TYPE QUALIFIER LIST\n");
        for (int i = 0; i < indent+1; i++) printf("  ");
        print_ast_stack(ast->a1.ptr, 0);
        printf("\n");
      } else {
        printf("(empty) TYPE QUALIFIER LIST\n");
      }
      break;
    case A_ARRAY_DIRECT_DECLARATOR:
      printf("ARRAY DIRECT DECLARATOR\n");
      if (ast->a3.num) {
        printf("STATIC\n");
      }
      if (ast->a1.ptr->node_type != A_NONE) {
        print_ast(ast->a1.ptr, indent+1);
      }
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_FUNCTION_DIRECT_DECLARATOR:
      printf("FUNCTION DIRECT DECLARATOR\n");
      print_ast(ast->a1.ptr, indent+1);
      break;

    /*
     * Struct and union declaration
     */
    case A_STRUCT_DECLARATOR:
      printf("STRUCT DECLARATOR\n");
      print_ast(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_STRUCT_DECLARATOR_LIST:
      printf("STRUCT DECLARATOR LIST\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_SPECIFIER_QUALIFIER_LIST:
      if (ast->a1.ptr->node_type != A_NONE) {
        printf("SPECIFIER QUALIFIER LIST\n");
        for (int i = 0; i < indent+1; i++) printf("  ");
        print_ast_stack(ast->a1.ptr, 0);
        printf("\n");
      } else {
        printf("(empty) SPECIFIER QUALIFIER LIST\n");
      }
      break;
    case A_STRUCT_DECLARATION:
      printf("STRUCT DECLARATION\n");
      print_ast(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
      break;
    case A_STRUCT_DECLARATION_LIST:
      printf("STRUCT DECLARATION LIST\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_STRUCT_SPECIFIER:
      printf("STRUCT SPECIFIER\n");
      if (ast->a1.ptr->node_type != A_NONE) {
        print_ast(ast->a1.ptr, indent+1);
      }
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_UNION_SPECIFIER:
      printf("UNION SPECIFIER\n");
      if (ast->a1.ptr->node_type != A_NONE) {
        print_ast(ast->a1.ptr, indent+1);
      }
      print_ast(ast->a2.ptr, indent+1);
      break;

    /*
     * Statements
     */
    case A_LABEL:
      printf("LABEL %s\n", ast->a1.str);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_CASE:
      printf("CASE\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_DEFAULT:
      printf("DEFAULT\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_COMPOUND_STATEMENT:
      printf("COMPOUND STATEMENT\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_NULL_STATEMENT:
      printf("NULL STATEMENT\n");
      break;
    case A_IF:
      printf("IF\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_IF_ELSE:
      printf("IF\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("ELSE\n");
      print_ast(ast->a3.ptr, indent+1);
      break;
    case A_SWITCH:
      printf("SWITCH\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_WHILE:
      printf("WHILE\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_DO_WHILE:
      printf("DO\n");
      print_ast(ast->a1.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("WHILE\n");
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_FOR:
      printf("FOR\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;    
    case A_FOR_CLAUSES:
      printf("FOR CLAUSES\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      print_ast(ast->a3.ptr, indent+1);
      break; 
    case A_GOTO:
      printf("GOTO %s\n", ast->a1.str);
      break;    
    case A_CONTINUE:
      printf("CONTINUE\n");
      break;
    case A_BREAK:
      printf("BREAK\n");
      break;
    case A_RETURN:
      printf("RETURN\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_EXPRESSION_STATEMENT:
      printf("EXPRESSION STATEMENT\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    case A_FUNCTION:
      printf("FUNCTION %s\n", ast->a1.str);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_TRANSLATION_UNIT:
      printf("TRANSLATION UNIT\n");
      print_ast_stack(ast->a1.ptr, indent+1);
      break;
    default:
      printf("Couldn't recognize type %d\n", ast->node_type);
  }
}

