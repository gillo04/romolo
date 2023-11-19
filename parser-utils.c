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
