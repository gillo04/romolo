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

Ast m_binary_expression(int* i, Ast (*prev_exp)(), char* strings[], int* types) {
  int j = *i;
  Ast out = prev_exp(&j); 
  if (out.node_type != A_NONE) {
    Ast right, op = out;
    int k, found_op = 1;
    while (found_op && toks[j].type != T_NONE && toks[j+1].type != T_NONE) {
      k = j+1;
      right = prev_exp(&k); 

      if (right.node_type != A_NONE) {
        int l = 0, found_match = 0;
        while (strings[l] != 0) {
          if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, strings[l]})) {
            op.node_type = types[l];
            astcpy(&op.a1.ptr, out);
            astcpy(&op.a2.ptr, right);
            j = k;
            found_match = 1;
            break;
          } else {
            l++;
          }
        }

        if (!found_match) {
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

char* mult_strs[] = {"*", "/", "%"};
int mult_types[] = {A_MULTIPLICATION, A_DIVISION, A_MODULO};
Ast m_multiplicative_expression(int* i) {
  Ast out = m_binary_expression(i, m_unary_expression, mult_strs, mult_types);
  return out;
}

char* add_strs[] = {"+", "-"};
int add_types[] = {A_ADDITION, A_SUBTRACTION};
Ast m_additive_expression(int* i) {
  Ast out = m_binary_expression(i, m_multiplicative_expression, add_strs, add_types);
  return out;
}

char* shift_strs[] = {"<<", ">>"};
int shift_types[] = {A_LEFT_SHIFT, A_RIGHT_SHIFT};
Ast m_shift_expression(int* i) {
  Ast out = m_binary_expression(i, m_additive_expression, shift_strs, shift_types);
  return out;
}

char* rel_strs[] = {">", "<", ">=", "<="};
int rel_types[] = {A_GRATER, A_LESS, A_GRATER_EQUAL, A_LESS_EQUAL};
Ast m_relational_expression(int* i) {
  Ast out = m_binary_expression(i, m_shift_expression, rel_strs, rel_types);
  return out;
}

char* eq_strs[] = {"==", "!="};
int eq_types[] = {A_EQUAL, A_NOT_EQUAL};
Ast m_equality_expression(int* i) {
  Ast out = m_binary_expression(i, m_relational_expression, eq_strs, eq_types);
  return out;
}

char* b_and_strs[] = {"&"};
int b_and_types[] = {A_BITWISE_AND};
Ast m_bitwise_and_expression(int* i) {
  Ast out = m_binary_expression(i, m_equality_expression, b_and_strs, b_and_types);
  return out;
}

char* b_xor_strs[] = {"^"};
int b_xor_types[] = {A_BITWISE_XOR};
Ast m_bitwise_xor_expression(int* i) {
  Ast out = m_binary_expression(i, m_bitwise_and_expression, b_xor_strs, b_xor_types);
  return out;
}

char* b_or_strs[] = {"|"};
int b_or_types[] = {A_BITWISE_OR};
Ast m_bitwise_or_expression(int* i) {
  Ast out = m_binary_expression(i, m_bitwise_xor_expression, b_or_strs, b_or_types);
  return out;
}

char* l_and_strs[] = {"&&"};
int l_and_types[] = {A_LOGIC_AND};
Ast m_logic_and_expression(int* i) {
  Ast out = m_binary_expression(i, m_bitwise_or_expression, l_and_strs, l_and_types);
  return out;
}

char* l_or_strs[] = {"||"};
int l_or_types[] = {A_LOGIC_OR};
Ast m_logic_or_expression(int* i) {
  Ast out = m_binary_expression(i, m_logic_and_expression, l_or_strs, l_or_types);
  return out;
}

Ast m_expression(int* i) {
  int j = *i;
  Ast out = m_additive_expression(&j);
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

