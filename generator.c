#include "data-structures.h"
#include "generator.h"
#include "memory-manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) if (x.str == 0) { return (Block) {0, 0}; }

int label_count = 0;

extern Register registers[REGISTERS_DIM+2];
extern Mem_obj objects[OBJECTS_DIM];
extern Variable variables[VARIABLES_DIM];

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
    op, s.result->loc.reg->name64
  );

  out.result = s.result;
  return out;
}

Block g_binary_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block l = g_ast(ast->a1.ptr);
  CHECK(l.str);
  r_lock(l.result);

  Block r = g_ast(ast->a2.ptr);
  CHECK(r.str);

  append_format(&out.str,
    "%s"
    "%s"
    "\t%s %s, %s\n",
    l.str.str,
    r.str.str,
    op, l.result->loc.reg->name64, r.result->loc.reg->name64
  );

  r_free(r.result);
  r_unlock(l.result);
  out.result = l.result;
  return out;
}

Block g_binary_logic_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block l = g_ast(ast->a1.ptr);
  CHECK(l.str);
  r_lock(l.result);

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
    l.result->loc.reg->name64, r.result->loc.reg->name64,
    op, l.result->loc.reg->name8,
    l.result->loc.reg->name64
  );

  r_free(r.result);
  r_unlock(l.result);
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
        out = r_alloc(8);
        append_string(&out.str, r_load(out.result).str.str);
        append_format(&out.str, 
          "\tmov %s, %lld\n"
          , out.result->loc.reg->name64, ast->a1.num
        );
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
      CHECK(out.str);
      append_format(&out.str,
        "\tor %s, %s\n"
        "\tsete %s\n",
        out.result->loc.reg->name64, out.result->loc.reg->name64,
        out.result->loc.reg->name64
      );
      break;
    case A_MULTIPLICATION:
      out = g_binary_op(ast, "imul");
      break;
    /*case A_DIVISION:
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
      break;*/
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
        r_lock(l.result);

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
          l.result->loc.reg->name64,
          label_count, label_count + 1, label_count,
          r.str.str,
          r.result->loc.reg->name64, l.result->loc.reg->name8, 
          label_count + 1,
          l.result->loc.reg->name64
        );

        label_count += 2;

        r_free(r.result);
        r_unlock(l.result);
        out.result = l.result;
      }
      break;
    case A_LOGIC_OR:
      {
        Block l = g_ast(ast->a1.ptr);
        CHECK(l.str);
        r_lock(l.result);

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
          l.result->loc.reg->name64,
          label_count,  
          l.result->loc.reg->name64, label_count + 1, label_count,
          r.str.str,
          r.result->loc.reg->name64, l.result->loc.reg->name8, 
          l.result->loc.reg->name64, label_count + 1
        );

        label_count += 2;

        r_free(r.result);
        r_unlock(l.result);
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
          cond.result->loc.reg->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);

        // Free up result register but remember it
        Register* result = t.result->loc.reg;
        r_free(t.result);

        // Generate false branch
        Block f = g_ast(ast->a3.ptr);
        CHECK(f.str);

        // Make sure false result is in the right register
        Block f_move_to = r_move(f.result, result - registers);

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
    
    /*
     * Statements
     */
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
          cond.result->loc.reg->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);
        r_free(t.result);
        
        append_format(&out.str,
          "%s"
          "if_end_%d:\n",
          t.str.str, label_count
        );

        label_count ++;
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
          cond.result->loc.reg->name64,
          label_count
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);
        r_free(t.result);

        // Generate false branch
        Block f = g_ast(ast->a3.ptr);
        CHECK(f.str);
        r_free(f.result);

        append_format(&out.str,
          "%s"
          "\tjmp if_end_%d\n"
          "if_else_%d:\n"
          "%s"
          "if_end_%d:\n",
          t.str.str, label_count,
          label_count, f.str.str, label_count 
        );

        label_count ++;
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

        append_format(&out.str,
          "while_%d:\n"
          "%s"
          "\tcmp %s, 0\n"
          "\tje while_end_%d\n",
          label_count, cond.str.str,
          cond.result->loc.reg->name64, label_count
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
        r_free(stat.result);
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

        append_format(&out.str, 
          "%s"
          "\tret\n",
          r_move(out.result, 1).str.str
        );
      }
      break;
    case A_EXPRESSION_STATEMENT:
      out = g_ast_stack(ast->a1.ptr);
      break;
    case A_FUNCTION_DEFINITION:
      {
        if (ast->a2.ptr->node_type != A_NONE) {
          Block body = g_ast(ast->a2.ptr);
          CHECK(body.str);

          append_format(&out.str,
            "%s:\n"
            "%s\n",
            ast->a1.ptr->a2.ptr->a2.ptr->a1.ptr->a1.str, body.str.str
          );
          // TODO: pop stack frame
        }
      }
      break;
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

char* generator(Ast* ast) {
  label_count = 0;
  init_memory();

  Block out = g_ast(ast);
  return out.str.str;
}
