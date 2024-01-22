#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include "parser-declarator.h"
#include "parser-initializer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

Ast m_designator(int* i) {
  int j = *i;
  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "["})) {
    j++;
    Ast exp = m_expression(&j);
    if (exp.node_type == A_NONE) {
      return (Ast) {A_NONE};
    }

    if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "]"})) {
      return (Ast) {A_NONE};
    }
    j++;
    *i = j;
    return exp;
  } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "."})) {
    j++;
    Ast ident = m_identifier(&j);
    if (ident.node_type == A_NONE) {
      return (Ast) {A_NONE};
    }
    *i = j;
    return ident;
  }
  
  return (Ast) {A_NONE};
}

Ast m_designator_list(int* i) {
  int j = *i;
  Ast out = {A_DESIGNATOR_LIST};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    out.a1.ptr[l] = m_designator(&j);
    l++;
  } while (out.a1.ptr[l-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast m_designation(int* i) {
  int j = *i;
  Ast des = m_designator_list(&j);
  if (des.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "="})) {
    return (Ast) {A_NONE};
  }
  j++;
  *i = j;
  return des;
}

Ast m_initializer_list_element(int* i) {
  int j = *i;
  Ast des = m_designation(&j);
  Ast init = m_initializer(&j);

  if (init.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast out = {A_INITIALIZER_LIST_ELEMENT};
  astcpy(&out.a1.ptr, des);
  astcpy(&out.a2.ptr, init);
  *i = j;
  return out;
}

Ast m_initializer_list(int* i) {
  return m_comma_list(i, m_initializer_list_element, A_INITIALIZER_LIST);
}

Ast m_initializer(int* i) {
  int j = *i;
  Ast exp = m_assignment_expression(&j);

  if (exp.node_type != A_NONE) {
    Ast out = {A_INITIALIZER};
    astcpy(&out.a1.ptr, exp);
    *i = j;
    return out;
  }

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "{"})) {
    return (Ast) {A_NONE};
  }
  j++;

  Ast init = m_initializer_list(&j);
  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, ","})) {
    j++;
  }
  if (init.node_type == A_NONE && !tokcmp(toks[j], (Token) {T_PUNCTUATOR, "}"})) {
    return (Ast) {A_NONE};
  }
  j++;

  Ast out = {A_INITIALIZER};
  astcpy(&out.a1.ptr, init);
  *i = j;
  return out;
}
