#include "data-structures.h"
#include "generator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) if (x.str == 0) { return (Block) {0, 0}; }
#define REG_COUNT 14
#define USED_REGS_COUNT 256

int label_count = 0;

Register registers[REG_COUNT+2] = {
  {0},       // Null value 
  {"rax", "eax", "ax", "al"},
  {"rbx", "ebx", "bx", "bl"},
  {"rcx", "ecx", "cx", "cl"},
  {"rdx", "edx", "dx", "dl"},
  {"rsi", "esi", "si", "sil"},
  {"rdi", "edi", "di", "dil"},
  {"r8", "r8d", "r8w", "r8b"},
  {"r9", "r9d", "r9w", "r9b"},
  {"r10", "r10d", "r10w", "r10b"},
  {"r11", "r11d", "r11w", "r11b"},
  {"r12", "r12d", "r12w", "r12b"},
  {"r13", "r13d", "r13w", "r13b"},
  {"r14", "r14d", "r14w", "r14b"},
  {"r15", "r15d", "r15w", "r15b"},
  {0}       // Means the value is on the stack
};

void print_regs() {
  for (int i = 1; i < REG_COUNT+1; i++) {
    printf("%s: %d\n", registers[i].name64, registers[i].used);
  }
  printf("\n");
}

Register* used_regs[USED_REGS_COUNT];

Register** insert_reg(int reg) {
  Register** out = 0;
  for (int j = 0; j < USED_REGS_COUNT; j++) {
    if (!used_regs[j]) {
      used_regs[j] = &registers[reg];
      out = &used_regs[j];
      registers[reg].owner = out;
      break;
    }
  }
  if (registers[reg].owner == 0) {
    printf("Error: exceeded maximum number of allocated values\n");
    return out;
  }
  registers[reg].used = 1;
  return out;
}

int get_unused_reg() {
  for (int i = 1; i < REG_COUNT+1; i++) {
    if (!registers[i].used) {
      return i;
    }
  }
  return 0;
}

Register** r_alloc() {
  int reg = get_unused_reg();
  if (reg != 0) {
    return insert_reg(reg);
  }

  printf("Error: stack is not yet handled\n");
  return 0;
}

void r_free(Register** r) {
  if (r == 0) {
    return;
  }

  if (*r == 0) {
    printf("Passed pointer to ghost register\n");
    return;
  }
  (*r)->used = 0;
  *r = 0;
}

void r_reset() {
  for (int i = 1; i < REG_COUNT+1; i++) {
    registers[i].used = 0;
    registers[i].owner = 0;
  }

  for (int i = 1; i < USED_REGS_COUNT; i++) {
    used_regs[i] = 0;
  }
}

// Relocates the value at register
Register** r_realloc(int request, Block* correction) {
  Block out = {0, 0};
  set_string(&out.str, "");
  if (registers[request].used) {
    int new_loc = get_unused_reg();
    append_format(&out.str, 
      "\tmov %s, %s\n",
      registers[new_loc].name64, registers[request].name64
    );
    registers[new_loc].owner = registers[request].owner;
    registers[new_loc].used = 1;
    *registers[request].owner = &registers[new_loc];
  }
  
  *correction = out;
  return insert_reg(request);
}

Block r_move_to(Register** src, Register* dest) {
  Block out = {0, 0};

  if (*src == dest) {
    set_string(&out.str, "");
    out.result = src;
    return out;
  }

  if (dest->used) {
    printf("Error: trying to move to an already used register\n");
    return out;
  }

  append_format(&out.str, 
    "\tmov %s, %s\n",
    dest->name64, (*src)->name64
  );
  *src = dest;
  dest->used = 1;
  dest->owner = src;

  out.result = src;
  return out;
}

Block g_ast(Ast* ast);

Block g_ast_stack(Ast* ast) {
  int i = 0;
  Block out = {0, 0};
  while (ast[i].node_type != A_NONE) {
    if (i != 0) {
      r_free(out.result);
    }
    Block tmp = g_ast(&ast[i]);
    CHECK(tmp.str);

    i++;
    append_string(&out.str, tmp.str.str);
    out.result = tmp.result;
  }
  return out;
}

Block g_unary_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block s = g_ast(ast->a1.ptr);
  CHECK(s.str);

  append_format(&out.str,
    "%s"
    "\t%s %s\n",
    s.str.str,
    op, (*s.result)->name64
  );

  out.result = s.result;
  return out;
}

Block g_binary_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block l = g_ast(ast->a1.ptr);
  CHECK(l.str);
  Block r = g_ast(ast->a2.ptr);
  CHECK(r.str);

  append_format(&out.str,
    "%s"
    "%s"
    "\t%s %s, %s\n",
    l.str.str,
    r.str.str,
    op, (*l.result)->name64, (*r.result)->name64
  );

  r_free(r.result);
  out.result = l.result;
  return out;
}

Block g_binary_logic_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block l = g_ast(ast->a1.ptr);
  CHECK(l.str);
  Block r = g_ast(ast->a2.ptr);
  CHECK(r.str);

  append_format(&out.str,
    "%s"
    "%s"
    "\tcmp %s, %s\n"
    "\t%s %s\n"
    "\tand %s, 1\n",
    l.str.str,
    r.str.str,
    (*l.result)->name64, (*r.result)->name64,
    op, (*l.result)->name8,
    (*l.result)->name64
  );

  r_free(r.result);
  out.result = l.result;
  return out;
}

Block g_ast(Ast* ast) {
  Block out = {0, 0};

  int indent = 0;
  switch (ast->node_type) {
    case A_NONE:
      set_string(&out.str, "");
      break;
    case A_CONSTANT:
      {
        Register** reg = r_alloc();
        append_format(&out.str, 
          "\tmov %s, %lld\n"
          , (*reg)->name64, ast->a1.num
        );
        out.result = reg;
      }
      break;
    /*case A_STRING_LITERAL:
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
      break;*/

    case A_UNARY_PLUS:
      out = g_ast(ast->a1.ptr);
      break;
    case A_UNARY_MINUS:
      out = g_unary_op(ast, "neg");
      break;
    case A_BITWISE_NOT:
      out = g_unary_op(ast, "not");
      break;
    case A_LOGIC_NOT:
      out = g_ast(ast->a1.ptr);
      append_format(&out.str,
        "\tor %s, %s\n"
        "\tsete %s\n",
        (*out.result)->name64, (*out.result)->name64, (*out.result)->name64
      );
      break;
    case A_MULTIPLICATION:
      out = g_binary_op(ast, "imul");
      break;
    case A_DIVISION:
      {
        Block l = g_ast(ast->a1.ptr);
        Block r = g_ast(ast->a2.ptr);
        append_format(&out.str,
          "%s%s",
          l.str.str, r.str.str
        );

        Block realloc;
        Register** rax = r_realloc(1, &realloc);
        append_string(&out.str, realloc.str.str);

        Register** rdx = r_realloc(4, &realloc);
        append_string(&out.str, realloc.str.str);

        append_format(&out.str,
          "\txor rdx, rdx\n"
          "\tmov rax, %s\n"
          "\tidiv %s\n",
          (*l.result)->name64, (*r.result)->name64
        );

        r_free(rdx);
        r_free(l.result);
        r_free(r.result);
        out.result = rax;
      }
      break;
    case A_MODULO:
      {
        Block l = g_ast(ast->a1.ptr);
        Block r = g_ast(ast->a2.ptr);
        append_format(&out.str,
          "%s%s",
          l.str.str, r.str.str
        );

        Block realloc;
        Register** rax = r_realloc(1, &realloc);
        append_string(&out.str, realloc.str.str);

        Register** rdx = r_realloc(4, &realloc);
        append_string(&out.str, realloc.str.str);

        append_format(&out.str,
          "\txor rdx, rdx\n"
          "\tmov rax, %s\n"
          "\tidiv %s\n",
          (*l.result)->name64, (*r.result)->name64
        );

        r_free(rax);
        r_free(l.result);
        r_free(r.result);
        out.result = rdx;
      }
      break;
    case A_ADDITION:
      out = g_binary_op(ast, "add");
      break;
    case A_SUBTRACTION:
      out = g_binary_op(ast, "sub");
      break;
    case A_LEFT_SHIFT:
      out = g_binary_op(ast, "shl");
      break;
    case A_RIGHT_SHIFT:
      out = g_binary_op(ast, "shr");
      break;
    case A_GREATER:
      out = g_binary_logic_op(ast, "setg");
      break;
    case A_LESS:
      out = g_binary_logic_op(ast, "setl");
      break;
    case A_GREATER_EQUAL:
      out = g_binary_logic_op(ast, "setge");
      break;
    case A_LESS_EQUAL:
      out = g_binary_logic_op(ast, "setle");
      break;
    case A_EQUAL:
      out = g_binary_logic_op(ast, "sete");
      break;
    case A_NOT_EQUAL:
      out = g_binary_logic_op(ast, "setne");
      break;
    case A_BITWISE_AND:
      out = g_binary_op(ast, "and");
      break;
    case A_BITWISE_XOR:
      out = g_binary_op(ast, "xor");
      break;
    case A_BITWISE_OR:
      out = g_binary_op(ast, "or");
      break;
    case A_LOGIC_AND:
      {
        Block l = g_ast(ast->a1.ptr);
        CHECK(l.str);
        Block r = g_ast(ast->a2.ptr);
        CHECK(r.str);

        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tjne logic_and_%d\n"
          "\tjmp logic_and_%d\n"
          "logic_and_%d:\n"
          "%s"
          "\tcmp %s, 0\n"
          "\tsetne %s\n"
          "logic_and_%d:\n"
          "\tand %s, 1\n",
          l.str.str,
          (*l.result)->name64,
          label_count, label_count + 1, label_count,
          r.str.str,
          (*r.result)->name64, (*l.result)->name8, 
          label_count + 1,
          (*l.result)->name64
        );

        label_count += 2;

        r_free(r.result);
        out.result = l.result;
      }
      break;
    case A_LOGIC_OR:
      {
        Block l = g_ast(ast->a1.ptr);
        CHECK(l.str);
        Block r = g_ast(ast->a2.ptr);
        CHECK(r.str);

        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje logic_or_%d\n"
          "\tmov %s, 1\n"
          "\tjmp logic_or_%d\n"
          "logic_or_%d:\n"
          "%s"
          "\tcmp %s, 0\n"
          "\tsetne %s\n"
          "\tand %s, 1\n"
          "logic_or_%d:\n",
          l.str.str,
          (*l.result)->name64,
          label_count,  
          (*l.result)->name64, label_count + 1, label_count,
          r.str.str,
          (*r.result)->name64, (*l.result)->name8, 
          (*l.result)->name64, label_count + 1
        );

        label_count += 2;

        r_free(r.result);
        out.result = l.result;
      }
      break;
    case A_CONDITIONAL_EXP:
      {
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje cond_exp_else_%d\n",
          cond.str.str,
          (*cond.result)->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);

        // Free up result register but remember it
        Register* result = *t.result;
        r_free(t.result);

        // Generate false branch
        Block f = g_ast(ast->a3.ptr);
        CHECK(f.str);

        // Make sure false result is in the right register
        Block f_move_to = r_move_to(f.result, result);

        append_format(&out.str,
          "%s"
          "\tjmp cond_exp_end_%d\n"
          "cond_exp_else_%d:\n"
          "%s"
          "%s"
          "cond_exp_end_%d:\n",
          t.str.str, label_count,
          label_count, f.str.str, f_move_to.str.str, label_count 
        );

        label_count ++;
        out.result = f.result;
      }
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
      out = g_ast_stack(ast->a1.ptr);
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
      {
        append_format(&out.str,
          "%s:\n",
          ast->a1.str
        );
        Block stat = g_ast(ast->a2.ptr);
        CHECK(stat.str);

        append_string(&out.str, stat.str.str);
        out.result = stat.result;
      }
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
      out = g_ast_stack(ast->a1.ptr);
      break;
    case A_NULL_STATEMENT:
      printf("NULL STATEMENT\n");
      break;
    case A_IF:
      {
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje if_end_%d\n",
          cond.str.str,
          (*cond.result)->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);
        
        append_format(&out.str,
          "%s"
          "if_end_%d:\n",
          t.str.str, label_count
        );

        label_count ++;
        out.result = t.result;
      }
      break;
    case A_IF_ELSE:
      {
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje if_else_%d\n",
          cond.str.str,
          (*cond.result)->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);

        // Free up result register but remember it
        Register* result = *t.result;
        r_free(t.result);

        // Generate false branch
        Block f = g_ast(ast->a3.ptr);
        CHECK(f.str);

        // Make sure false result is in the right register
        Block f_move_to = r_move_to(f.result, result);

        append_format(&out.str,
          "%s"
          "\tjmp if_end_%d\n"
          "if_else_%d:\n"
          "%s"
          "%s"
          "if_end_%d:\n",
          t.str.str, label_count,
          label_count, f.str.str, f_move_to.str.str, label_count 
        );

        label_count ++;
        out.result = f.result;
      }
      break;
    case A_SWITCH:
      printf("SWITCH\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_WHILE:
      {
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        printf("ok\n");
        append_format(&out.str,
          "while_%d:\n"
          "%s"
          "\tcmp %s, 0\n"
          "\tje while_end_%d\n",
          label_count, cond.str.str,
          (*cond.result)->name64, label_count
        );
        r_free(cond.result);

        Block stat = g_ast(ast->a2.ptr);
        CHECK(stat.str);
        append_format(&out.str,
          "%s"
          "\tjmp while_%d\n"
          "while_end_%d:\n",
          stat.str.str, label_count,
          label_count
        );
        out.result = stat.result;
      }
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
      append_format(&out.str,
        "\tjmp %s\n",
        ast->a1.str
      );
      break;    
    case A_CONTINUE:
      printf("CONTINUE\n");
      break;
    case A_BREAK:
      printf("BREAK\n");
      break;
    case A_RETURN:
      {
        out = g_ast(ast->a1.ptr);
        CHECK(out.str);
        Block realloc = {0, 0};
        if (*(out.result) - registers != 1) {
          Register** rax = r_realloc(1, &realloc);
          append_format(&out.str, 
            "%s"
            "\tmov rax, %s\n",
            realloc.str.str, (*out.result)->name64
          );
        }

        append_format(&out.str, 
          "\tret\n"
        );
      }
      break;
    case A_EXPRESSION_STATEMENT:
      out = g_ast_stack(ast->a1.ptr);
      break;
    /*case A_FUNCTION:
      {
        Block body = g_ast(ast->a2.ptr);
        CHECK(body.str);

        append_format(&out.str,
          "%s:\n"
          "%s\n",
          ast->a1.str, body.str.str
        );

        r_reset();
      }
      break;*/
    case A_TRANSLATION_UNIT:
      {
        Block stack = g_ast_stack(ast->a1.ptr);
        append_format(&out.str,
          ".intel_syntax noprefix\n"
          ".global main\n"
          ".text\n\n"
          "%s",
          stack.str.str
        );
      }
      break;
    default:
      printf("Couldn't recognize type %d\n", ast->node_type);
  }
  CHECK(out.str);
  return out;
}

void init_generator() {
  label_count = 0;
  r_reset();
  return;
}

char* generator(Ast* ast) {
  init_generator();
  Block out = g_ast(ast);

  return out.str.str;
}
