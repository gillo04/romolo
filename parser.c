#include "log.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token* toks;

void ast_strcpy(char** dest, char* src) {
  *dest = (char*) malloc(strlen(src));
  strcpy(*dest, src);
}

int tokcmp(Token a, Token b) {
  if (a.type == T_CONSTANT) {
    return a.val.num == b.val.num && a.type == b.type;
  } else {
    return strcmp(a.val.str, b.val.str) == 0 && a.type == b.type;
  }
}

Ast m_expression(int* i);

Ast m_primary_expression(int* i) {
  int j = *i;
  Ast out = {A_NONE};
  if (toks[j].type == T_IDENTIFIER) {
    out.node_type = A_IDENTIFIER;
    out.exp_type = E_NONE;
    ast_strcpy(&out.a1.str, toks[j].val.str);
    *i = j+1;
  } else if (toks[j].type == T_CONSTANT) {
    out.node_type = A_CONSTANT;
    out.exp_type = E_NONE;
    out.a1.num = toks[j].val.num;
    *i = j+1;
  } else if (toks[j].type == T_STRING_LITERAL) {
    out.node_type = A_STRING_LITERAL;
    out.exp_type = E_NONE;
    ast_strcpy(&out.a1.str, toks[j].val.str);
    *i = j+1;
  } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("})) {
    j++;
    out = m_expression(&j);
    if (out.node_type != A_NONE && tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"})) {
      out.exp_type = E_NONE;
      *i = j+1;
    } else {
      out.node_type = A_NONE;
    }
  }

  return out;
}

Ast m_expression(int* i) {
  int j = *i;
  Ast out = m_primary_expression(&j);
  if (out.node_type != A_NONE) {
    *i = j;
    return out;
  }
  return (Ast) {A_NONE};
}

Ast parser(Token* tokens) {
  toks = tokens;
  int i = 0;
  Ast out = m_expression(&i);

  return out;
}

