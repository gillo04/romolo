#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include "parser-declarator.h"
#include "parser-statement.h"
#include "data-structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token* toks;
char* src;

Ast m_function_def(int* i) {
  int j = *i;
  if (!tokcmp(toks[j], (Token) {T_KEYWORD, "int"})) {
    return (Ast) {A_NONE};
  }

  j++;
  Ast out = {A_FUNCTION};
  ast_strcpy(&out.a1.str, toks[j].val.str);
  j++;

  if (!(tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ")"}) )) {
    return (Ast) {A_NONE};
  }
  j += 2;

  Ast comp_stat = m_compound_statement(&j);
  astcpy(&out.a2.ptr, comp_stat);

  *i = j;
  return out;
}

Ast m_translation_unit(int* i) {
  int j = *i;

  Ast out = {A_TRANSLATION_UNIT};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int k = 0;

  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (k+1));
    out.a1.ptr[k++] = m_function_def(&j);
  } while (out.a1.ptr[k-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast parser(Token* tokens, char* source) {
  toks = tokens;
  src = source;
  int i = 0;
  Ast out = m_type_name(&i);

  return out;
}

