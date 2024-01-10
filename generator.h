#pragma once
#include "parser.h"
#include "data-structures.h"

// Result of a generation. Aside from the assembly output, it says in which register is stored the result of the operation
typedef struct register_s {
  char* name64;
  char* name32;
  char* name16;
  char* name8;

  char used;
  struct register_s** owner;
} Register;

typedef struct {
  String str;
  Register** result;
} Block;


char* generator(Ast* ast);
