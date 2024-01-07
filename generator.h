#pragma once
#include "parser.h"

// Result of a generation. Aside from the assembly output, it says in which register is stored the result of the operation
typedef struct {
  char* str;
  int result;
} Block;

typedef struct {
  char in_use;
  char* name;
} Register;

char* generator(Ast* ast);
