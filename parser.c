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

void astcpy(Ast** dest, Ast src) {
  *dest = (Ast*) malloc(sizeof(Ast));
  **dest = src;
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
  // TODO: implement generics
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

Ast m_postfix_expression(int* i) {
  // TODO: implement function calls, list initializer
  int j = *i;
  Ast out = m_primary_expression(&j);
  if (out.node_type != A_NONE) {
    // Ast post_exp = m_postfix_expression(&j);
    int found_postfix = 1;
    Ast tmp = out;
    while (found_postfix && toks[j].type != T_NONE) {
      if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "."}) && toks[j+1].type == T_IDENTIFIER) {
        tmp.node_type = A_MEMBER;
        astcpy(&tmp.a1.ptr, out);
        ast_strcpy(&tmp.a2.str, toks[j+1].val.str);
        j += 2;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "->"}) && toks[j+1].type == T_IDENTIFIER) {
        tmp.node_type = A_MEMBER_DEREFERENCE;
        astcpy(&tmp.a1.ptr, out);
        ast_strcpy(&tmp.a2.str, toks[j+1].val.str);
        j += 2;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "++"})) {
        tmp.node_type = A_POST_INCREMENT;
        astcpy(&tmp.a1.ptr, out);
        j += 1;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "--"})) {
        tmp.node_type = A_POST_DECREMENT;
        astcpy(&tmp.a1.ptr, out);
        j += 1;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "["})){
        j++;
        Ast exp = m_expression(&j);
        if (exp.node_type != A_NONE && tokcmp(toks[j], (Token) {T_PUNCTUATOR, "]"})) {
          tmp.node_type = A_ARRAY_SUBSCRIPT;
          astcpy(&tmp.a1.ptr, out);
          astcpy(&tmp.a2.ptr, exp);
          j += 1;
        } else {
          j--;
          found_postfix = 0;
        }
      } else {
        found_postfix = 0;
      }
      *i = j;
      out = tmp;
    }
  }

  return out;
}

Ast m_expression(int* i) {
  int j = *i;
  Ast out = m_postfix_expression(&j);
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

