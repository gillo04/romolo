#include "data-structures.h"
#include "generator.h"
#include "memory-manager.h"
#include "verify-utils.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) if (x.str == 0) { return (Block) {0, 0}; }

extern Register registers[REGISTERS_DIM+2];
extern Mem_obj objects[OBJECTS_DIM];
extern Variable variables[VARIABLES_DIM];
extern Function functions[FUNCTIONS_DIM];

int label_stack[LABELS_DIM];
int lab_sp = 0;
int label_count = 0;

void label_push() {
  label_stack[lab_sp] = label_count;
  lab_sp++;
  label_count++;
}

void label_pop() {
  lab_sp--;
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
    op, g_name(s.result).str.str
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
  r_lock(r.result);

  append_format(&out.str,
    "%s"
    "%s"
    "\t%s %s, %s\n",
    l.str.str,
    r.str.str,
    op, g_name(l.result).str.str, g_name(r.result).str.str
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
  r_lock(r.result);

  append_format(&out.str,
    "%s"
    "%s"
    "\tcmp %s, %s\n"
    "\t%s %s\n"
    "\tand %s, 1\n",
    l.str.str,
    r.str.str,
    g_name(l.result).str.str, g_name(r.result).str.str,
    op, l.result->loc.reg->name8,
    g_name(l.result).str.str
  );

  r_free(r.result);
  out.result = l.result;
  return out;
}

Block g_assign(Ast* ast, char* op) {
  Block out = {0, 0};
  Block unary = g_ast(ast->a1.ptr);
  CHECK(unary.str);

  Block exp = g_ast(ast->a2.ptr);
  CHECK(exp.str);
  r_lock(exp.result);

  Block ld = r_load(unary.result);
  Register* reg = unary.result->loc.reg;
  append_format(&out.str,
    "%s"
    "%s"
    "%s"
    "\t%s %s, %s\n",
    unary.str.str, exp.str.str, ld.str.str,
    op, g_name(unary.result).str.str, g_name(exp.result).str.str
  );

  append_string(&out.str, r_store(unary.result).str.str);
  // Make sure the result register is still reserved
  r_move(unary.result, reg - registers);

  r_free(exp.result);
  out.result = unary.result;
  return out;
}

Block g_lvalue(Ast* ast) {
  Block out = {0, 0};

  int indent = 0;
  switch (ast->node_type) {
    case A_NONE:
      set_string(&out.str, "");
      break;
    case A_IDENTIFIER:
      {
        Variable* var = var_find(ast->a1.str);
        if (var != 0) {
          set_string(&out.str, "");
          out.result = obj_alloc((Mem_obj) {
            M_STACK,
            var->size,
            .loc.stack_off = var->stack_off,
            var
          });
        }
      }
      break;
    default:
      printf("Couldn't recognize type %d (in g_lvalue)\n", ast->node_type);
  }

  CHECK(out.str);
  return out;
}

int arg_regs[6] = {6, 5, 4, 3, 7, 8};
Block g_arguments(Ast* ast) {
  int i = 0;
  Block out = {0, 0};
  if (ast == 0) {
    set_string(&out.str, "");
    return out;
  }
  Mem_obj* args[OBJECTS_DIM];
  while (ast[i].node_type != A_NONE) {
    Block tmp = g_ast(&ast[i]);
    CHECK(tmp.str);
    
    Block move = r_move(tmp.result, arg_regs[i]);
    r_lock(tmp.result);
    append_string(&out.str, tmp.str.str);
    append_string(&out.str, move.str.str);

    args[i] = tmp.result;
    i++;
  }

  for (int j = 0; j < i; j++) {
    r_free(args[j]);
  }
  return out;
}

Block g_parameters(Ast* ast) {
  int i = 0;
  Block out = {0, 0};
  if (ast == 0) {
    set_string(&out.str, "");
    return out;
  }
  while (ast[i].node_type != A_NONE) {
    Variable var = {
      ast[i].a2.ptr->a2.ptr->a1.ptr->a1.str,
      ast[i].a1.ptr,
      ast[i].a2.ptr,
      0,
      -1,
      0
    };
    var.size = type_sizeof(var.dec_spec, var.dec);

    Block dec = var_push(var);
    CHECK(dec.str);

    Block move = g_mov(dec.result, (Mem_obj) {
      M_REG,
      var.size,
      .loc.reg = registers + arg_regs[i],
      0
    });
    append_string(&out.str, dec.str.str);
    append_string(&out.str, move.str.str);
    r_free(dec.result);
    i++;
  }

  return out;
}

Block g_ast(Ast* ast) {
  Block out = {0, 0};

  int indent = 0;
  switch (ast->node_type) {
    case A_NONE:
      set_string(&out.str, "");
      break;
    case A_IDENTIFIER:
      {
        Variable* var = var_find(ast->a1.str);
        if (var != 0) {
          out = r_alloc(var->size);
          append_string(&out.str, r_load(out.result).str.str);
          append_string(&out.str, g_mov(out.result, (Mem_obj) {M_STACK, var->size, .loc.stack_off = var->stack_off, var}).str.str);
          out.result->var = var;
        }
      }
      break;
    case A_CONSTANT:
      {
        out = r_alloc(4);

        Block ld = r_load(out.result);
        append_string(&out.str, ld.str.str);
        append_format(&out.str, 
          "\tmov %s, %lld\n",
          g_name(out.result).str.str, ast->a1.num
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
    /*case A_POST_INCREMENT:
      out = g_unary_op(ast, "inc");
      break;
    case A_POST_DECREMENT:
      out = g_unary_op(ast, "dec");
      break;*/
    case A_ARRAY_SUBSCRIPT:
      printf("ARRAY SUBSCRIPT\n");
      print_ast(ast->a1.ptr, indent+1);
      for (int i = 0; i < indent; i++) printf("  ");
      printf("AT INDEX\n");
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_PRE_INCREMENT:
      {
        out = g_unary_op(ast, "inc");
        Register* reg = out.result->loc.reg;
        append_string(&out.str, r_store(out.result).str.str);
        r_move(out.result, reg - registers);
      }
      break;
    case A_PRE_DECREMENT:
      {
        out = g_unary_op(ast, "dec");
        Register* reg = out.result->loc.reg;
        append_string(&out.str, r_store(out.result).str.str);
        r_move(out.result, reg - registers);
      }
      break;
    case A_ADDRESS:
      printf("ADDRESS\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_DEREFERENCE:
      printf("DEREFERENCE\n");
      print_ast(ast->a1.ptr, indent+1);
      break;
    case A_FUNCTION_CALL:
      Function* func = func_find(ast->a1.ptr->a1.str);
      
      // Pass arguments
      out = g_arguments(ast->a2.ptr->a1.ptr);
      CHECK(out.str);

      // Make room for result
      if (func->output_size != 0) {
        Block reserve = r_alloc(func->output_size);
        Block move = r_move(reserve.result, 1);
        out.result = move.result;
        append_string(&out.str, move.str.str);
      }

      append_format(&out.str, 
        "\tcall %s\n",
        func->name
      );
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
        g_name(out.result).str.str, g_name(out.result).str.str,
        out.result->loc.reg->name8
      );
      break;
    case A_MULTIPLICATION:
      out = g_binary_op(ast, "imul");
      break;
    case A_DIVISION:
      {
        Block l = g_ast(ast->a1.ptr);
        CHECK(l.str);

        Block r = g_ast(ast->a2.ptr);
        CHECK(r.str);

        append_format(&out.str,
          "%s%s",
          l.str.str, r.str.str
        );

        // Move l to rax
        append_string(&out.str, r_move(l.result, 1).str.str);
        r_lock(l.result);

        // Allocate rdx
        Block rdx = r_alloc(8);
        append_string(&out.str, rdx.str.str);
        append_string(&out.str, r_move(rdx.result, 4).str.str);
        r_lock(rdx.result);

        // Load r
        append_string(&out.str, r_load(r.result).str.str);
        append_format(&out.str,
          "\txor rdx, rdx\n"
          "\tidiv %s\n",
          g_name(r.result).str.str
        );

        r_free(rdx.result);
        r_free(r.result);
        r_unlock(l.result);
        out.result = l.result;
      }
      break;
    case A_MODULO:
      {
        Block l = g_ast(ast->a1.ptr);
        CHECK(l.str);

        Block r = g_ast(ast->a2.ptr);
        CHECK(r.str);

        append_format(&out.str,
          "%s%s",
          l.str.str, r.str.str
        );

        // Move l to rax
        append_string(&out.str, r_move(l.result, 1).str.str);
        r_lock(l.result);

        // Allocate rdx
        Block rdx = r_alloc(8);
        append_string(&out.str, rdx.str.str);
        append_string(&out.str, r_move(rdx.result, 4).str.str);
        r_lock(rdx.result);

        // Load r
        append_string(&out.str, r_load(r.result).str.str);
        append_format(&out.str,
          "\txor rdx, rdx\n"
          "\tidiv %s\n",
          g_name(r.result).str.str
        );

        r_free(l.result);
        r_free(r.result);
        r_unlock(rdx.result);
        out.result = rdx.result;
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
        r_lock(r.result);

        append_string(&out.str, r_load(l.result).str.str);
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
          g_name(l.result).str.str,
          label_count, label_count + 1, label_count,
          r.str.str,
          g_name(r.result).str.str, l.result->loc.reg->name8,
          label_count + 1,
          g_name(l.result).str.str
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
        r_lock(r.result);

        append_string(&out.str, r_load(l.result).str.str);
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
          g_name(l.result).str.str,
          label_count,  
          g_name(l.result).str.str, label_count + 1, label_count,
          r.str.str,
          g_name(r.result).str.str, l.result->loc.reg->name8,
          g_name(l.result).str.str, label_count + 1
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
          g_name(cond.result).str.str,
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
      {
        Block lvalue = g_lvalue(ast->a1.ptr);
        CHECK(lvalue.str);

        Block exp = g_ast(ast->a2.ptr);
        CHECK(exp.str);

        append_format(&out.str,
          "%s%s", lvalue.str.str, exp.str.str);
        append_string(&out.str, g_mov(lvalue.result, *exp.result).str.str);

        r_free(exp.result);
        out.result = lvalue.result;
      }
      break;
    case A_MULT_ASSIGN:
      out = g_assign(ast, "imul");
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
      out = g_assign(ast, "add");
      break;
    case A_SUB_ASSIGN:
      out = g_assign(ast, "sub");
      break;
    case A_L_SHIFT_ASSIGN:
      out = g_assign(ast, "shl");
      break;
    case A_R_SHIFT_ASSIGN:
      out = g_assign(ast, "shr");
      break;
    case A_AND_ASSIGN:
      out = g_assign(ast, "and");
      break;
    case A_XOR_ASSIGN:
      out = g_assign(ast, "xor");
      break;
    case A_OR_ASSIGN:
      out = g_assign(ast, "or");
      break;
    case A_COMMA_EXP:
      out = g_ast_stack(ast->a1.ptr);
      break;

    /*
     * Declarations
     */
    case A_DECLARATION:
      {
        int i = 0;
        while (ast->a2.ptr->a1.ptr[i].node_type != A_NONE) {
          Variable var = {
            ast->a2.ptr->a1.ptr[i].a1.ptr->a2.ptr->a1.ptr->a1.str,
            ast->a1.ptr,
            ast->a2.ptr->a1.ptr[i].a1.ptr,
            0,
            -1,
            0
          };
          var.size = type_sizeof(var.dec_spec, var.dec);

          Block dec = var_push(var);
          CHECK(dec.str);

          append_string(&out.str, dec.str.str);
          if (ast->a2.ptr->a1.ptr[i].a2.ptr->node_type != A_NONE) {
            Block init = g_ast(ast->a2.ptr->a1.ptr[i].a2.ptr->a1.ptr);
            CHECK(init.str);

            append_string(&out.str, init.str.str);
            append_string(&out.str, g_mov(dec.result, *init.result).str.str);
            r_free(init.result);

            append_string(&out.str, r_store(dec.result).str.str);
            r_free(dec.result);
          }
          append_string(&out.str, "\n");
          i++;
        }
      }
      break;
    
    /*
     * Statements
     */
    case A_LABEL:
      {
        append_format(&out.str,
          "label_%s:\n",
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
      var_push_stack_frame();
      out = g_ast_stack(ast->a1.ptr);
      append_string(&out.str, var_pop_stack_frame().str.str);
      break;
    case A_NULL_STATEMENT:
      set_string(&out.str, "");
      break;
    case A_IF:
      {
        int lab = label_count;
        label_count ++;
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje if_end_%d\n",
          cond.str.str,
          g_name(cond.result).str.str,
          lab 
        );
        r_free(cond.result);

        // Generate true branch
        Block t = g_ast(ast->a2.ptr);
        CHECK(t.str);
        r_free(t.result);
        
        append_format(&out.str,
          "%s"
          "if_end_%d:\n",
          t.str.str, lab 
        );
      }
      break;
    case A_IF_ELSE:
      {
        int lab = label_count;
        label_count ++;
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "%s"
          "\tcmp %s, 0\n"
          "\tje if_else_%d\n",
          cond.str.str,
          g_name(cond.result).str.str,
          lab 
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
          t.str.str, lab,
          lab, f.str.str, lab 
        );
      }
      break;
    case A_SWITCH:
      printf("SWITCH\n");
      print_ast(ast->a1.ptr, indent+1);
      print_ast(ast->a2.ptr, indent+1);
      break;
    case A_WHILE:
      {
        label_push();
        
        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);

        append_format(&out.str,
          "loop_%d:\n"
          "loop_cond_%d:\n"
          "%s\n"
          "\tcmp %s, 0\n"
          "\tje loop_end_%d\n",
          label_stack[lab_sp-1], label_stack[lab_sp-1], cond.str.str,
          g_name(cond.result).str.str, label_stack[lab_sp-1] 
        );
        r_free(cond.result);

        Block stat = g_ast(ast->a2.ptr);
        CHECK(stat.str);
        append_format(&out.str,
          "%s"
          "\tjmp loop_%d\n"
          "loop_end_%d:\n\n",
          stat.str.str, label_stack[lab_sp-1],
          label_stack[lab_sp-1] 
        );
        r_free(stat.result);
        label_pop();
      }
      break;
    case A_DO_WHILE:
      {
        label_push();

        Block stat = g_ast(ast->a2.ptr);
        CHECK(stat.str);
        append_format(&out.str,
          "loop_%d:\n"
          "%s",
          label_stack[lab_sp-1],
          stat.str.str
        );
        r_free(stat.result);

        Block cond = g_ast(ast->a1.ptr);
        CHECK(cond.str);
        append_format(&out.str,
          "loop_cond_%d:\n"
          "%s\n"
          "\tcmp %s, 0\n"
          "\tjne loop_%d\n\n",
          label_stack[lab_sp-1],
          cond.str.str,
          g_name(cond.result).str.str,
          label_stack[lab_sp-1], label_stack[lab_sp-1]
        );
        r_free(cond.result);
        label_pop();
      }
      break;
    case A_FOR:
      {
        label_push();
        var_push_stack_frame();

        Block clause1 = g_ast(ast->a1.ptr->a1.ptr);
        CHECK(clause1.str);
        r_free(clause1.result);

        Block clause2 = g_ast(ast->a1.ptr->a2.ptr);
        CHECK(clause2.str);
        r_free(clause2.result);

        append_format(&out.str,
          "%s\n"
          "loop_%d:\n"
          "loop_cond_%d:\n"
          "%s\n"
          "\tcmp %s, 0\n"
          "\tje loop_end_%d\n",
          clause1.str.str,
          label_stack[lab_sp-1], label_stack[lab_sp-1], clause2.str.str,
          g_name(clause2.result).str.str, label_stack[lab_sp-1]
        );

        Block stat = g_ast(ast->a2.ptr);
        CHECK(stat.str);
        r_free(stat.result);

        Block clause3 = g_ast(ast->a1.ptr->a3.ptr);
        CHECK(clause3.str);
        r_free(clause3.result);

        append_format(&out.str,
          "%s"
          "%s"
          "\tjmp loop_%d\n"
          "loop_end_%d:\n\n",
          stat.str.str, clause3.str.str, label_stack[lab_sp-1],
          label_stack[lab_sp-1]
        );

        append_string(&out.str, var_pop_stack_frame().str.str);
        label_pop();
      }
      break;    
    case A_GOTO:
      append_format(&out.str,
        "\tjmp label_%s\n",
        ast->a1.str
      );
      break;    
    case A_CONTINUE:
      append_format(&out.str,
        "\tjmp loop_cond_%d\n",
        label_stack[lab_sp-1]
      );
      break;
    case A_BREAK:
      append_format(&out.str,
        "\tjmp loop_end_%d\n",
        label_stack[lab_sp-1]
      );
      break;
    case A_RETURN:
      {
        out = g_ast(ast->a1.ptr);
        CHECK(out.str);

        append_format(&out.str, 
          "%s"
          "\tmov rsp, rbp\n"
          "\tpop rbp\n"
          "\tret\n",
          r_move(out.result, 1).str.str
        );
        r_free(out.result);
      }
      break;
    case A_EXPRESSION_STATEMENT:
      out = g_ast_stack(ast->a1.ptr);
      append_string(&out.str, "\n");
      r_free(out.result);
      out.result = 0;
      break;
    case A_FUNCTION_DEFINITION:
      {
        var_push_stack_frame();

        Function func = {
          ast->a1.ptr->a2.ptr->a2.ptr->a1.ptr[0].a1.str,
          ast->a1.ptr->a1.ptr,
          ast->a1.ptr->a2.ptr,
        };
        func.output_size = type_sizeof(func.dec_spec, func.dec);
        func_push(func);

        Block para = g_parameters(ast->a1.ptr->a2.ptr->a2.ptr->a1.ptr[1].a1.ptr->a1.ptr);
        CHECK(para.str);

        Block body = g_ast(ast->a2.ptr);
        CHECK(body.str);

        append_format(&out.str,
          "%s:\n"
          "\tpush rbp\n"
          "\tmov rbp, rsp\n"
          "%s\n"
          "%s\n",
          ast->a1.ptr->a2.ptr->a2.ptr->a1.ptr->a1.str, 
          para.str.str, body.str.str
        );
        var_pop_stack_frame();
      }
      break;
    case A_TRANSLATION_UNIT:
      {
        Block stack = g_ast_stack(ast->a1.ptr);
        append_format(&out.str,
          ".intel_syntax noprefix\n"
          ".global main\n\n"
          ".text\n"
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
  lab_sp = 0;
  label_count = 0;
  init_memory();

  Block out = g_ast(ast);
  return out.str.str;
}
