#include "memory-manager.h"
#include "parser.h"

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

void print_mem_structs() {
  printf("\nRegisters:\n");
  for (int i = 1; i < REGISTERS_DIM + 1; i++) {
    printf("%s\t%d\t%d\n", registers[i].name64, registers[i].used, registers[i].locked);
  }

  printf("\nMemory objects:\n");
  for (int i = 0; i < OBJECTS_DIM; i++) {
    if (objects[i].type == M_REG) {
      printf("REG\t%s\n", objects[i].loc.reg->name64);
    } else if (objects[i].type == M_STACK) {
      printf("STACK\t%d\n", objects[i].loc.stack_off);
    }
  }

  printf("\nVariables:\n");
  for (int i = 0; i < VARIABLES_DIM; i++) {
    if (variables[i].name != 0) {
      printf("%s\t%d\n", variables[i].name, variables[i].stack_off);
    }
  }
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
}

Mem_obj* obj_alloc(Mem_obj obj) {
  for (int i = 0; i < OBJECTS_DIM; i++) {
    if (objects[i].type == M_NONE) {
      objects[i] = obj;
      return &objects[i];
    }
  }
  printf("Error: objects table is full\n");
  return 0;
}

Mem_obj* var_push(Variable var) {
  // TODO: Turn into a dynamic stack 
  if (var_sp >= VARIABLES_DIM) {
    printf("Error: variables stack overflow\n");
    return 0;
  }
  hw_sp += var.size;
  var.stack_off = hw_sp;
  variables[var_sp] = var;

  Mem_obj obj = {M_STACK, var.size};
  obj.loc.stack_off = hw_sp;
  obj.var = &variables[var_sp];
  variables[var_sp].obj = obj_alloc(obj);

  var_sp++;
  return variables[var_sp-1].obj;
}

void var_pop() {
  if (var_sp <= var_bp) {
    printf("Error: variable stack underflow\n");
  }
  var_sp --;
  r_free(variables[var_sp].obj);
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
  if (obj->type == M_STACK) {
    tmp_bytes_to_clear += obj->size;
    if (hw_sp == tmp_bytes_to_clear) {
      hw_sp -= obj->loc.stack_off;
      obj->type = M_NONE;
      append_format(&out.str,
        "\tadd rsp, %d\n",
        obj->loc.stack_off);
      tmp_bytes_to_clear = 0;
    }
  } else if (obj->type == M_REG) {
    obj->loc.reg->used = 0;
    obj->loc.reg->locked = 0;
    obj->type = M_NONE;
    set_string(&out.str, "");
  } else {
    printf("Error: trying to free empty Mem_obj\n");
  }
  out.result = 0;
  return out;
}

Block g_mov(Mem_obj* dest, Mem_obj src) {
  Block out = {0, 0};
  out.result = dest;

  append_format(&out.str,
    "\tmov ");

  // Generate destination
  if (dest->type == M_REG) {
    switch (dest->size) {
      case 1:
        append_format(&out.str,
          "%s, ", dest->loc.reg->name8);
        break;
      case 2:
        append_format(&out.str,
          "%s, ", dest->loc.reg->name16);
        break;
      case 4:
        append_format(&out.str,
          "%s, ", dest->loc.reg->name32);
        break;
      case 8:
        append_format(&out.str,
          "%s, ", dest->loc.reg->name64);
        break;
      default:
        printf("Error: memory object of invalid size\n");
        return out;
    }
  } else {
    append_format(&out.str,
      "[rbp - %d], ", dest->loc.stack_off);
  }

  // Generate source 
  if (src.type == M_REG) {
    switch (src.size) {
      case 1:
        append_format(&out.str,
          "%s\n", src.loc.reg->name8);
        break;
      case 2:
        append_format(&out.str,
          "%s\n", src.loc.reg->name16);
        break;
      case 4:
        append_format(&out.str,
          "%s\n", src.loc.reg->name32);
        break;
      case 8:
        append_format(&out.str,
          "%s\n", src.loc.reg->name64);
        break;
      default:
        printf("Error: memory object of invalid size\n");
        return out;
    }
  } else {
    append_format(&out.str,
      "[rbp - %d]\n", src.loc.stack_off);
  }

  return out;
}

Block r_move(Mem_obj* obj, int reg) {
  // TODO: use stack if necessary
  Block out = {0, 0};
  if (registers[reg].locked) {
    printf("Error: trying to move to a locked register\n");
    return out;
  }

  if (reg == obj->loc.reg - registers) {
    set_string(&out.str, "");
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
        printf("Error: all registers are currently locked\n");
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
      printf("Error: trying to store a memory object that's not a variable\n");
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

