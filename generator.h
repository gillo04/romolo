#pragma once
#include "parser.h"
#include "data-structures.h"

#define LABELS_DIM 256

typedef struct mem_obj_s Mem_obj;

// Result of a generation. Aside from the assembly output, it says in which register is stored the result of the operation
typedef struct {
  char* name64;
  char* name32;
  char* name16;
  char* name8;

  char used;
  char locked;
  Mem_obj* owner;
} Register;

enum {
  M_NONE = 0,
  M_REG,
  M_STACK,
};

typedef struct {
  char* name;     // If null, it's a variable stack base pointer
  Ast* dec_spec;  // Declaration specifiers
  Ast* dec;       // Declarator
  int size;
  int stack_off;  // Hardware stack offset
  Mem_obj* obj;
} Variable;

typedef struct {
  char* name;     
  Ast* dec_spec;  // Declaration specifiers
  Ast* dec;       // Declarator
  int output_size;
} Function;

struct mem_obj_s {
  int type;
  int size;
  union { // Location
    Register* reg;
    int stack_off; // Offset from hardware stack base pointer
  } loc;
  Variable* var;
};

typedef struct {
  String str;
  Mem_obj* result;
} Block;


char* generator(Ast* ast);
