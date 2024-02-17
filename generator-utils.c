#include "parser-utils.h"
#include "generator.h"
#include "generator-utils.h"
#include "memory-manager.h"
#include "verify-utils.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Register registers[REGISTERS_DIM+2];
extern Mem_obj objects[OBJECTS_DIM];
extern Variable variables[VARIABLES_DIM];
extern Function functions[FUNCTIONS_DIM];
extern int func_sp;

void free_type(Type t) {
  return;
  if (t.dec_spec != 0) {
    print_ast(t.dec_spec, 0);
    free_ast(t.dec_spec, 1);
  }
  if (t.dec != 0) {
    // print_ast(t.dec, 0);
    free_ast(t.dec, 1);
  }
}

Type type_copy(Type t) {
  Type out = {
    ast_deep_copy(t.dec_spec),
    ast_deep_copy(t.dec)
  };
  return out;
}

Block integer_promotion(Mem_obj* obj) {
  Block out = {0, 0};
  if (!is_integer(obj->t)) {
    log_msg(WARN, "cannot perform integer promotion\n", -1);
    return out;
  }

  int size = type_sizeof(obj->t);
  int sign = is_signed(obj->t);
}

Type choose_common_type(Type a, Type b) {

}

// Converts the type of obj, also generates the needed assembly
Block convert_type(Mem_obj* obj, Type t);

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

void fix_size(Mem_obj* l, Mem_obj* r) {
  if (l->size > r->size) {
    l->size = r->size;
  } else if (l->size < r->size) {
    r->size = l->size;
  }
}

Block g_binary_op(Ast* ast, char* op) {
  Block out = {0, 0};
  Block l = g_ast(ast->a1.ptr);
  CHECK(l.str);

  Block r = g_ast(ast->a2.ptr);
  CHECK(r.str);
  r_lock(r.result);

  // Drop to smalest
  fix_size(l.result, r.result);
  append_format(&out.str,
    "%s"
    "%s"
    "\t%s %s, %s\n",
    l.str.str,
    r.str.str,
    op, g_name(l.result).str.str, g_name(r.result).str.str
  );

  // Chose type
  // apply_conversion(&out.result, l.result, r.result);
  /*out.result->dec_spec = ast_deep_copy(var->dec_spec);
  out.result->dec = ast_deep_copy(var->dec);*/

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

  fix_size(l.result, r.result);
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

Block g_name(Mem_obj* obj) {
  Block out = {0, 0};
  if (obj->type == M_NONE) {
    set_string(&out.str, "");
    log_msg(WARN, "trying to get the name of an invalid Mem_obj\n", -1);
    return out;
  }

  if (obj->type == M_REG) {
    switch (obj->size) {
      case 1:
        append_string(&out.str, obj->loc.reg->name8);
        break;
      case 2:
        append_string(&out.str, obj->loc.reg->name16);
        break;
      case 4:
        append_string(&out.str, obj->loc.reg->name32);
        break;
      case 8:
        append_string(&out.str, obj->loc.reg->name64);
        break;
      default:
        log_msg(WARN, "memory object of invalid size\n", -1);
        return out;
    }
  } else {
    switch (obj->size) {
      case 1:
        append_string(&out.str, "byte ");
        break;
      case 2:
        append_string(&out.str, "word ");
        break;
      case 4:
        append_string(&out.str, "dword ");
        break;
      case 8:
        append_string(&out.str, "qword ");
        break;
      default:
        log_msg(WARN, "memory object of invalid size\n", -1);
        return out;
    }

    append_format(&out.str,
      "[rbp - %d]", obj->loc.stack_off);
  }
  out.result = obj;
  return out;
}

Block g_mov(Mem_obj* dest, Mem_obj src) {
  Block out = {0, 0};
  out.result = dest;

  Block d_name = g_name(dest);

  if (dest->size == src.size) {
    Block s_name = g_name(&src);
    append_format(&out.str,
      "\tmov %s, %s\n", d_name.str.str, s_name.str.str);
  } else if (dest->size < src.size) {
    log_msg(WARN, "truncating value\n", -1);
    src.size = dest->size;
    Block s_name = g_name(&src);
    append_format(&out.str,
      "\tmov %s, %s\n", d_name.str.str, s_name.str.str);
  } else {
    // TODO: account for signedness 
    Block s_name = g_name(&src);
    append_format(&out.str,
      "\tmovzx %s, %s\n", d_name.str.str, s_name.str.str);
  }

  return out;
}
