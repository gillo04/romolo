#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include "parser-declarator.h"
#include "parser-statement.h"
#include "parser-initializer.h"
#include "data-structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token* toks;
char* src;
int error_flag;

Ast m_declaration_list(int* i) {
  int j = *i;
  Ast out = {A_DECLARATION_LIST};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    out.a1.ptr[l] = m_declaration(&j);
    l++;
  } while (out.a1.ptr[l-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast m_function_prototype(int* i) {
  int j = *i;
  Ast dec_spec = m_declaration_specifier_list(&j);
  if (dec_spec.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast dec = m_declarator(&j);
  if (dec.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast dec_list = m_declaration_list(&j);
  Ast out = {A_FUNCTION_PROTOTYPE};
  astcpy(&out.a1.ptr, dec_spec);
  astcpy(&out.a2.ptr, dec);
  astcpy(&out.a3.ptr, dec_list);
  *i = j;
  return out;
}

Ast m_function_definition(int* i) {
  int j = *i;
  Ast prot = m_function_prototype(&j);
  if (prot.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast stat = m_compound_statement(&j);
  if (stat.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast out = {A_FUNCTION_DEFINITION};
  astcpy(&out.a1.ptr, prot);
  astcpy(&out.a2.ptr, stat);

  *i = j;
  return out;
}

Ast m_external_declaration(int* i) {
  int j = *i;
  Ast dec = m_declaration(&j);
  if (dec.node_type != A_NONE) {
    *i = j;
    return dec;
  }

  Ast func = m_function_definition(&j);
  *i = j;
  return func;
}

Ast m_translation_unit(int* i) {
  int j = *i;
  Ast out = {A_TRANSLATION_UNIT};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    out.a1.ptr[l] = m_external_declaration(&j);
    l++;
  } while (out.a1.ptr[l-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast parser(Token* tokens, char* source) {
  toks = tokens;
  src = source;
  error_flag = 0;

  int i = 0;
  Ast out = m_translation_unit(&i);

  if (error_flag) {
    return (Ast) {A_NONE};
  }
  return out;
}

