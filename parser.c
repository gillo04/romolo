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

Ast parser(Token* tokens, char* source) {
  toks = tokens;
  src = source;
  int i = 0;
  Ast out = m_statement(&i);

  return out;
}

