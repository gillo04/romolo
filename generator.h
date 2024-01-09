#pragma once
#include "parser.h"
#include "data-structures.h"

// Result of a generation. Aside from the assembly output, it says in which register is stored the result of the operation
typedef struct register_s {
  char used;
  char* name;
} Register;

typedef struct {
  String str;
  int result;
} Block;


char* generator(Ast* ast);
