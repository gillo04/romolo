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

// The "m_" stands for "match"
Ast m_expression(int* i);

Ast m_primary_expression(int* i) {
  // TODO: implement generics
  int j = *i;
  Ast out = {A_NONE};
  if (toks[j].type == T_IDENTIFIER) {
    out.node_type = A_IDENTIFIER;
    out.type = (Type) {E_NONE, E_NONE};
    ast_strcpy(&out.a1.str, toks[j].val.str);
    *i = j+1;
  } else if (toks[j].type == T_CONSTANT) {
    out.node_type = A_CONSTANT;
    out.type = (Type) {E_RAW, E_INT};
    out.a1.num = toks[j].val.num;
    *i = j+1;
  } else if (toks[j].type == T_STRING_LITERAL) {
    out.node_type = A_STRING_LITERAL;
    out.type = (Type) {E_ARRAY, E_CHAR};
    ast_strcpy(&out.a1.str, toks[j].val.str);
    *i = j+1;
  } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("})) {
    j++;
    out = m_expression(&j);
    if (out.node_type != A_NONE && tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"})) {
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

Ast m_unary_expression(int* i) {
  // TODO: implement sizeof and _Alignof
  int j = *i;
  Ast out = m_postfix_expression(&j);

  if (out.node_type == A_NONE) {
    int k = j+1;
    out = m_unary_expression(&k);
    if (out.node_type != A_NONE) {
      Ast tmp = {A_NONE};
      if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "++"})) {
        tmp.node_type = A_PRE_INCREMENT;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "--"})) {
        tmp.node_type = A_PRE_DECREMENT;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "&"})) {
        tmp.node_type = A_ADDRESS;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "*"})) {
        tmp.node_type = A_DEREFERENCE;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "+"})) {
        tmp.node_type = A_UNARY_PLUS;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "-"})) {
        tmp.node_type = A_UNARY_MINUS;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "~"})) {
        tmp.node_type = A_BITWISE_NOT;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "!"})) {
        tmp.node_type = A_LOGIC_NOT;
        astcpy(&tmp.a1.ptr, out);
        *i = k;
      }
      out = tmp;
    }
  } else {
    *i = j;
  }

  return out;
}

Ast m_multiplicative_expression(int* i) {
  int j = *i;
  Ast out = m_unary_expression(&j); // Replace with cast expression once implemented
  if (out.node_type != A_NONE) {
    Ast right, op = out;
    int k, found_op = 1;
    while (found_op && toks[j].type != T_NONE && toks[j+1].type != T_NONE) {
      k = j+1;
      right = m_unary_expression(&k);

      if (right.node_type != A_NONE) {
        if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "*"})) {
          op.node_type = A_MULTIPLICATION;
          astcpy(&op.a1.ptr, out);
          astcpy(&op.a2.ptr, right);
          j = k;
        } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "/"})) {
          op.node_type = A_DIVISION;
          astcpy(&op.a1.ptr, out);
          astcpy(&op.a2.ptr, right);
          j = k;
        } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "%"})) {
          op.node_type = A_MODULO;
          astcpy(&op.a1.ptr, out);
          astcpy(&op.a2.ptr, right);
          j = k;
        } else {
          found_op = 0;
        }
        out = op;
      } else {
        found_op = 0;
      }
    }
    *i = j;

  }

  return out;
}

Ast m_expression(int* i) {
  int j = *i;
  Ast out = m_multiplicative_expression(&j);
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

