#pragma once
#include "parser.h"
#include "data-structures.h"

// Result of a generation. Aside from the assembly output, it says in which register is stored the result of the operation
typedef struct {
  char* name64;
  char* name32;
  char* name16;
  char* name8;

  char used;
  Mem_obj* owner;
} Register;

enum {
  M_NONE,
  M_REG,
  M_STACK,
};

typedef struct {
  char* name;
  Ast* dec_spec;  // Declaration specifiers
  Ast* dec;       // Declarator
  int stack_off;  // Hardware stack offset
} Variable;

typedef struct {
  int type;
  int size;
  union loc { // Location
    Register* reg;
    int stack_off; // Offset from (variable) stack base pointer
  };
  Variable* var;
} Mem_obj;

typedef struct {
  String str;
  Mem_obj* result;
} Block;


char* generator(Ast* ast);
