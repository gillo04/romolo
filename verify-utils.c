#include "log.h" 
#include "parser.h" 
#include "verify-utils.h"
#include <stdio.h>

int type_sizeof(Ast* dec_spec, Ast* dec) {
  int size = 0;
  // Count declaration specifiers
  int c_char = 0, c_short = 0, c_int = 0, c_long = 0;
  int i = 0;
  while (dec_spec->a1.ptr[i].node_type != A_NONE) {
    switch(dec_spec->a1.ptr[i].node_type) {
      case A_CHAR:
        c_char++;
        break;
      case A_SHORT:
        c_short++;
        break;
      case A_INT:
        c_int++;
        break;
      case A_LONG:
        c_long++;
        break;
    }
    i++;
  }

  if (c_char == 1) {
    size = 1;
  } else if (c_short == 1) {
    size = 2;
  } else if (c_long == 1 || c_long == 2) {
    size = 8;
  } else if (c_int == 1) {
    size = 4;
  }

  // Multiply by array sizes
  i = 1;
  while (dec->a2.ptr->a1.ptr[i].node_type != A_NONE) {
    if (dec->a2.ptr->a1.ptr[i].node_type == A_ARRAY_DIRECT_DECLARATOR) {
      size *= dec->a2.ptr->a1.ptr[i].a2.ptr->a1.num;
    } else {
      return -1;
    }
    i++;
  }


  return size;
}
