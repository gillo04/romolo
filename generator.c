#include "data-structures.h"
#include "generator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) if (x == 0) { return (Block) {0, 0}; }
#define REG_COUNT 14

Register registers[REG_COUNT+2] = {
  {0, 0},       // Null value 
  {0, "rax"},
  {0, "rbx"},
  {0, "rcx"},
  {0, "rdx"},
  {0, "rsi"},
  {0, "rdi"},
  {0, "r8"},
  {0, "r9"},
  {0, "r10"},
  {0, "r11"},
  {0, "r12"},
  {0, "r13"},
  {0, "r14"},
  {0, "r15"},
  {0, 0}       // Means the value is on the stack
};

Block generate_ast(Ast* ast);

Block generate_ast_stack(Ast* ast) {
  int i = 0;
  String out = {0};
  int last_result = 0;
  while (ast[i].node_type != A_NONE) {
    Block tmp = generate_ast(&ast[i]);
    CHECK(tmp.str);

    i++;
    append_string(&out, tmp.str);
    last_result = tmp.result;
  }
  return (Block) {out.str, last_result};
}

int ralloc() {
  for (int i = 1; i < REG_COUNT+1; i++) {
    if (!registers[i].in_use) {
      registers[i].in_use = 1;
      return i;
    }
  }
  return REG_COUNT+1;
}

void rfree(int r) {
  registers[r].in_use = 0;
}

void rreset() {
  for (int i = 1; i < REG_COUNT+1; i++) {
    registers[i].in_use = 0;
  }
}

// Returns result register
int generate_unary_op(String* out, Ast* ast, char* op) {
  Block s = generate_ast(ast->a1.ptr);
  set_string(out, s.str);
  append_string(out, "\t");
  append_string(out, op);
  append_string(out, registers[s.result].name);
  append_string(out, "\n");
  return s.result;
}

int generate_binary_op(String* out, Ast* ast, char* op) {
  Block l = generate_ast(ast->a1.ptr);
  Block r = generate_ast(ast->a2.ptr);
  set_string(out, l.str);
  append_string(out, r.str);
  append_string(out, "\t");
  append_string(out, op);
  append_string(out, registers[l.result].name);
  append_string(out, ", ");
  append_string(out, registers[r.result].name);
  append_string(out, "\n");
  rfree(r.result);
  return l.result;
}

Block generate_ast(Ast* ast) {
  String out = {0};
  int result = 0;

  int indent = 0;
  switch (ast->node_type) {
    case A_NONE:
      set_string(&out, "");
      break;
    case A_CONSTANT:
      {
        int reg = ralloc();
        set_string(&out, "\tmov ");
        append_string(&out, registers[reg].name);
        append_string(&out, ", ");
        append_int(&out, ast->a1.num);
        append_string(&out, "\n");
        result = reg;
      }
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
      {
        Block s = generate_ast(ast->a1.ptr);
        set_string(&out, s.str);
        result = s.result;
      }
      break;
    case A_UNARY_MINUS:
      result = generate_unary_op(&out, ast, "neg ");
      break;
    case A_BITWISE_NOT:
      result = generate_unary_op(&out, ast, "not ");
      break;
    case A_LOGIC_NOT:
      {
        Block s = generate_ast(ast->a1.ptr);
        set_string(&out, s.str);
        append_string(&out, "\tor ");
        append_string(&out, registers[s.result].name);
        append_string(&out, ", ");
        append_string(&out, registers[s.result].name);
        append_string(&out, "\n\tsete ");
        append_string(&out, registers[s.result].name);
        append_string(&out, "\n");
        result = s.result;
      }
      break;
    case A_MULTIPLICATION:
      result = generate_binary_op(&out, ast, "imul ");
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
      result = generate_binary_op(&out, ast, "add ");
      break;
    case A_SUBTRACTION:
      result = generate_binary_op(&out, ast, "sub ");
      break;
    case A_LEFT_SHIFT:
      result = generate_binary_op(&out, ast, "shl ");
      break;
    case A_RIGHT_SHIFT:
      result = generate_binary_op(&out, ast, "shr ");
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
      {
        Block s = generate_ast_stack(ast->a1.ptr);
        CHECK(s.str);
        out.str = s.str;
        result = s.result;
      }
      break;
    /*case A_DECLARATION:
      printf("DECLARATION < ");
      print_type(&ast->type);
      printf(">\n");
      print_ast_stack(ast->a1.ptr, indent+1);
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
      break;
    case A_INITIALIZER:
      printf("INITIALIZER\n");
      break;
    case A_POINTER:
      printf("POINTER < \n");
      print_type(&ast->type);
      printf(">\n");
      if (ast->a1.ptr->node_type != A_NONE) {
        print_ast(ast->a1.ptr, indent + 1);
      }
      break;
    case A_DIRECT_DECLARATOR:
      printf("DIRECT DECLARATOR\n");
      break;
    case A_PARAMETER_TYPE_LIST:
      printf("PARAMETER TYPE LIST\n");
      print_ast(ast->a1.ptr, indent+1);
      if (ast->a2.ptr->node_type != A_NONE) {
        print_ast(ast->a2.ptr, indent+1);
      }
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
      break;*/
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
      {
        Block s = generate_ast_stack(ast->a1.ptr);
        CHECK(s.str);
        out.str = s.str;
        result = s.result;
      }
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
      print_ast(ast->a2.ptr, indent+1);
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
      {
        Block exp = generate_ast(ast->a1.ptr);
        CHECK(exp.str);
        
        set_string(&out, exp.str);
        append_string(&out, "\tret\n\n");
        result = exp.result;
      }
      break;
    case A_FUNCTION:
      {
        Block body = generate_ast(ast->a2.ptr);
        CHECK(body.str);

        set_string(&out, ast->a1.str);
        append_string(&out, ":\n");
        append_string(&out, body.str);

        rreset();
      }
      break;
    case A_TRANSLATION_UNIT:
      {
        Block s = generate_ast_stack(ast->a1.ptr);
        CHECK(s.str);
        out.str = s.str;
        result = s.result;
      }
      break;
    default:
      printf("Couldn't recognize type %d\n", ast->node_type);
  }
  return (Block) {out.str, result};
}

void init_generator() {
  return;
}

char* generator(Ast* ast) {
  init_generator();
  char* out = generate_ast(ast).str;

  return out;
}
