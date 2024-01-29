#include <string.h>
#include "memory-manager.h"
#include "parser.h"
#include "log.h"

Register registers[REGISTERS_DIM+2] = {
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

Mem_obj objects[OBJECTS_DIM];

// Variable stack
Variable variables[VARIABLES_DIM];
int var_bp = 0;   // Base pointer
int var_sp = 0;   // Stack pointer

// Hardware stack
int hw_stack[HW_STACK_DIM];   // Keeps track of hardware base pointers
int hw_bp = 0;    // Base pointer
int hw_sp = 0;    // Stack pointer

int tmp_bytes_to_clear = 0;

Function functions[FUNCTIONS_DIM];
int func_sp = 0;

void print_regs() {
  printf("\nRegisters:\n");
  for (int i = 1; i < REGISTERS_DIM + 1; i++) {
    printf("%s\t%d\t%d\n", registers[i].name64, registers[i].used, registers[i].locked);
  }

  printf("\nMemory objects:\n");
  for (int i = 0; i < OBJECTS_DIM; i++) {
    if (objects[i].type == M_REG) {
      printf("REG\t%s", objects[i].loc.reg->name64);
      printf("\t%p\n", objects[i].var);
    } else if (objects[i].type == M_STACK) {
      printf("STACK\t%d", objects[i].loc.stack_off);
      printf("\t%p\n", objects[i].var);
    }
  }
}

void print_var() {
  printf("\nVariables:\n");
  for (int i = 0; i < var_sp; i++) {
    if (variables[i].name != 0) {
      printf("%s\t%d\t%d\n", variables[i].name, variables[i].stack_off, variables[i].size);
    } else {
      printf("var_bp\t%d\n", variables[i].stack_off);
    } 
  }
}

void print_mem() {
  print_regs();
  print_var();
  printf("Stack var: %d %d %d\n", hw_bp, hw_sp, tmp_bytes_to_clear);
}

void init_memory() {
  for (int i = 1; i < REGISTERS_DIM + 1; i++) {
    registers[i].used = 0;
  }

  for (int i = 0; i < OBJECTS_DIM; i++) {
    objects[i].type = M_NONE;
  }

  var_bp = 0;
  var_sp = 0;
  hw_bp = 0;
  hw_sp = 0;
  tmp_bytes_to_clear = 0;
  func_sp = 0;
}

Mem_obj* obj_alloc(Mem_obj obj) {
  for (int i = 0; i < OBJECTS_DIM; i++) {
    if (objects[i].type == M_NONE) {
      objects[i] = obj;
      return &objects[i];
    }
  }
  log_msg(WARN, "objects table is full\n", -1);
  return 0;
}

Block var_push(Variable var) {
  Block out = {0, 0};
  // TODO: Turn into a dynamic stack 
  if (var_sp >= VARIABLES_DIM) {
    log_msg(WARN, "variables stack overflow\n", -1);
    return out;
  }
  hw_sp += var.size;
  var.stack_off = hw_sp;
  variables[var_sp] = var;

  Mem_obj obj = {M_STACK, var.size};
  obj.loc.stack_off = hw_sp;
  obj.var = &variables[var_sp];
  variables[var_sp].obj = obj_alloc(obj);

  var_sp++;
  out.result = variables[var_sp-1].obj;
  append_format(&out.str,
    "\tsub rsp, %d\n",
    var.size);
  return out;
}

void var_pop() {
  if (var_sp <= var_bp) {
    log_msg(WARN, "variables stack underflow\n", -1);
  }
  var_sp --;
  r_free(variables[var_sp].obj);
}

Variable* var_find(char* name) {
  for (int i = var_sp - 1; i >= 0; i--) {
    if (variables[i].name != 0 && strcmp(name, variables[i].name) == 0) {
      return &variables[i];
    }
  }
  log_msg(WARN, "couldn't find variable name\n", -1);
  return 0;
}

void var_push_stack_frame() {
  variables[var_sp] = (Variable) {0, 0, 0, 0, var_bp, 0};
  var_sp ++;
  var_bp = var_sp;
}

Block var_pop_stack_frame() {
  Block out = {0, 0};

  int to_remove = 0;
  for (int i = var_sp - 1; i >= var_bp; i--) {
    to_remove += variables[i].size;
  }
  var_sp = var_bp;
  var_bp = variables[var_bp-1].stack_off;
  var_sp --;

  hw_sp -= to_remove;
  if (to_remove > 0) {
    append_format(&out.str, "\tadd rsp, %d\n", to_remove);
  } else {
    set_string(&out.str, "");
  }
  return out;
}

void r_lock(Mem_obj* obj) {
  if (obj->type == M_REG) {
    obj->loc.reg->locked = 1;
  }
}

void r_unlock(Mem_obj* obj) {
  if (obj->type == M_REG) {
    obj->loc.reg->locked = 0;
  }
}

Block r_alloc(int size) {
  for (int i = 1; i < REGISTERS_DIM + 1; i++) {
    if (!registers[i].used) {
      Mem_obj obj = {M_REG, size};
      obj.loc.reg = &registers[i];
      obj.var = 0;

      registers[i].used = 1;
      registers[i].locked = 0;
      registers[i].owner = obj_alloc(obj);

      Block out = {0, 0};
      set_string(&out.str, "");
      out.result = registers[i].owner;
      return out;
    }
  }
  
  Mem_obj obj = {M_STACK, size};
  hw_sp += size;
  obj.loc.stack_off = hw_sp;
  obj.var = 0;

  Block out = {0, 0};
  append_format(&out.str,
    "\tsub rsp, %d\n",
    size);
  out.result = obj_alloc(obj);
  return out;
}

Block r_free(Mem_obj* obj) {
  Block out = {0, 0};
  if (obj == 0) {
    return out;
  }
  if (obj->type == M_STACK) {
    if (obj->var == 0) {
      tmp_bytes_to_clear += obj->size;
      if (hw_sp == tmp_bytes_to_clear) {
        hw_sp -= obj->loc.stack_off;
        append_format(&out.str,
          "\tadd rsp, %d\n",
          obj->loc.stack_off);
        tmp_bytes_to_clear = 0;
      }
    }
    obj->type = M_NONE;
  } else if (obj->type == M_REG) {
    obj->loc.reg->used = 0;
    obj->loc.reg->locked = 0;
    obj->type = M_NONE;
    set_string(&out.str, "");
  } else {
    log_msg(WARN, "trying to free empty Mem_obj\n", -1);
  }
  out.result = 0;
  return out;
}

Block g_name(Mem_obj* obj) {
  Block out = {0, 0};
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

Block r_move(Mem_obj* obj, int reg) {
  // TODO: use stack if necessary
  Block out = {0, 0};
  if (registers[reg].locked) {
    log_msg(WARN, "trying to move to a locked register\n", -1);
    return out;
  }

  if (reg == obj->loc.reg - registers) {
    set_string(&out.str, "");
    out.result = obj;
    return out; 
  }
  if (registers[reg].used) {
    int new = 0;
    for (int i = 1; i < REGISTERS_DIM + 1; i++) {
      if (!registers[i].used && !registers[i].locked) {
        new = i;
        break;
      }
    }

    if (new == 0) {
      // Move register at reg to the stack
      Mem_obj tmp = *registers[reg].owner;
      hw_sp += registers[reg].owner->size;
      registers[reg].owner->type = M_STACK;
      registers[reg].owner->loc.stack_off = hw_sp;
      append_format(&out.str, 
        "\tsub rsp, %d\n", registers[reg].owner->size);
      append_string(&out.str, g_mov(registers[reg].owner, tmp).str.str);
    } else {
      // Move register at reg to new
      Mem_obj tmp = *registers[reg].owner;
      registers[new].used = 1;
      registers[new].owner = registers[reg].owner;
      registers[new].owner->type = M_REG;
      registers[new].owner->loc.reg = &registers[new];
      out = g_mov(registers[new].owner, tmp);
    }
  }

  // Move obj to reg
  Mem_obj tmp = *obj;
  registers[reg].used = 1;
  registers[reg].owner = obj;
  obj->type = M_REG;
  obj->loc.reg = &registers[reg];
  
  Block mov = g_mov(obj, tmp);
  if (tmp.type == M_REG) {
    tmp.loc.reg->used = 0;
    tmp.loc.reg->locked = 0;
  }
  append_string(&out.str, mov.str.str);
  out.result = obj;
  return out;
}

Block r_load(Mem_obj* obj) {
  Block out = {0, 0};
  if (obj->type == M_STACK) {
    int reg = 0;
    int reg_backup = 0;
    for (int i = 1; i < REGISTERS_DIM + 1; i++) {
      if (!registers[i].used && !registers[i].locked) {
        reg = i;
        break;
      }

      if (!registers[i].locked) {
        reg_backup = i;
      }
    }

    if (reg == 0) {
      if (reg_backup == 0) {
        log_msg(WARN, "all registers are currently locked\n", -1);
      }
      reg = reg_backup;
    }

    out = r_move(obj, reg);

    // Free up stack
    if (obj->var == 0) {
      tmp_bytes_to_clear += obj->size;
      if (tmp_bytes_to_clear == hw_sp) {
        hw_sp -= tmp_bytes_to_clear;
        append_format(&out.str, 
          "\tadd rsp, %d\n", tmp_bytes_to_clear);
        tmp_bytes_to_clear = 0;
      }
    }
  } else {
    set_string(&out.str, "");
  }
  return out;
}

Block r_store(Mem_obj* obj) {
  Block out = {0, 0};
  if (obj->type == M_REG) {
    if (obj->var == 0) {
      log_msg(WARN, "trying to store a memory object that's not a variable\n", -1);
    } else {
      Mem_obj tmp = *obj;
      obj->type = M_STACK;
      obj->loc.reg->used = 0;
      obj->loc.reg->locked = 0;
      obj->loc.stack_off = obj->var->stack_off;
      out = g_mov(obj, tmp);
    }
  } else {
    set_string(&out.str, "");
  }
  return out;
}

Function* func_push(Function func) {
  for (int i = func_sp-1; i >= 0; i--) {
    if (strcmp(functions[i].name, func.name) == 0) {
      return 0;
    }
  }
  functions[func_sp] = func;
  func_sp++;
  return &functions[func_sp - 1];
}

Function* func_find(char* name) {
  for (int i = func_sp - 1; i >= 0; i--) {
    if (functions[i].name != 0 && strcmp(name, functions[i].name) == 0) {
      return &functions[i];
    }
  }
  log_msg(WARN, "couldn't find function name\n", -1);
  return 0;
}

Block align_stack() {
  Block out = {0, 0};
  int align = 16 - hw_sp % 16;
  if (align != 16) {
    append_format(&out.str,
      "\tsub rsp, %d\n",
      align 
    );
    hw_sp -= align;
  } else {
    set_string(&out.str, "");
  }

  return out;
}
