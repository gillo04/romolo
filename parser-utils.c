#include "log.h"
#include "parser-utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

void ast_strcpy(char** dest, char* src) {
  *dest = (char*) malloc(strlen(src));
  strcpy(*dest, src);
}

void astcpy(Ast** dest, Ast src) {
  *dest = (Ast*) malloc(sizeof(Ast));
  **dest = src;
}

int tokcmp(Token a, Token b) {
  if (a.type == T_NONE || b.type == T_NONE) {
    return 0;
  }

  if (a.type == T_CONSTANT) {
    return a.val.num == b.val.num && a.type == b.type;
  } else {
    return strcmp(a.val.str, b.val.str) == 0 && a.type == b.type;
  }
}

Ast m_comma_list(int* i, Ast(*match)(int*), int node_type) {
  int j = *i;
  Ast out = {node_type};
  Ast assign = match(&j);

  if (assign.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  int k = 1;
  out.a1.ptr = malloc(sizeof(Ast));
  while (assign.node_type != A_NONE) {
    *i = j;
    out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
    out.a1.ptr[k-1] = assign;
    k++;
    if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, ","})) {
      j++;
      assign = match(&j);
    } else {
      assign.node_type = A_NONE;
    }
  }
  out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
  out.a1.ptr[k-1] = (Ast) {A_NONE};
  return out;
}

Ast m_list(int* i, Ast(*match)(int*), int node_type) {
  int j = *i;
  Ast out = {node_type};
  Ast assign = match(&j);

  if (assign.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  int k = 1;
  out.a1.ptr = malloc(sizeof(Ast));
  while (assign.node_type != A_NONE) {
    *i = j;
    out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
    out.a1.ptr[k-1] = assign;
    k++;
    j++;
  }
  out.a1.ptr = realloc(out.a1.ptr, sizeof(Ast) * k);
  out.a1.ptr[k-1] = (Ast) {A_NONE};
  return out;
}
