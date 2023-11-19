#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token* toks;
char* src;



Ast m_declaration(int* i);

Type m_type_qualifier_list(int* i) {
  int j = *i;
  Type out = {E_NONE};
  Type tmp;
  do {
    tmp = m_type_qualifier(&j);
    if (tmp.node_type != E_NONE) {
      break;
    }

    if (tmp.node_type != E_NONE) {
      tmp.next = (Type*) malloc(sizeof(Type));
      *tmp.next = out;
      out = tmp;
    }
  } while (tmp.node_type != E_NONE);
  // TODO: Check that type follows the rules for type qualifiers

  *i = j;
  return out;
}

Ast m_pointer(int* i) {
  if (!tokcmp(toks[*i], (Token) {T_PUNCTUATOR, "*"})) {
    return (Ast) {A_NONE};
  }
  int j = *i + 1;
  Ast out = {A_POINTER};
  out.type = m_type_qualifier_list(&j);
  astcpy(&out.a1.ptr, m_pointer(i));

  return out;
}

Ast m_parameter_type_list(int* i) {
  return (Ast) {A_NONE};
}

Ast m_parameter_list(int* i) {
  return (Ast) {A_NONE};
}

Ast m_parameter_declaration(int* i) {
  return (Ast) {A_NONE};
}

Ast m_identifier_list(int* i) {
  return (Ast) {A_NONE};
}

Ast m_direct_declarator(int* i) {
  return (Ast) {A_NONE};
}

Ast m_declarator(int* i) {
  Ast out = {A_DECLARATOR};
  astcpy(&out.a1.ptr, m_pointer(i));
  astcpy(&out.a2.ptr, m_direct_declarator(i));
  if (out.a2.ptr->node_type == A_NONE) {
    printf("Declarator without direct declarator at:\n\t");
    print_source_line(src, toks[*i].line);
    printf("\n");
    return (Ast) {A_NONE};
  }

  return out;
}

Ast parser(Token* tokens, char* source) {
  toks = tokens;
  src = source;
  int i = 0;
  Ast out = m_declaration(&i);

  return out;
}

