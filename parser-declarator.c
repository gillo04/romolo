#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include "parser-declarator.h"
#include "data-structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


extern Token* toks;
extern char* src;

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

Ast m_parameter_list(int* i) {
  return m_comma_list(i, m_parameter_declaration, A_PARAMETER_LIST);
}

Ast m_parameter_type_list(int* i) {
  Ast out = m_parameter_list(i);
  if (out.node_type != A_NONE) {
    out.node_type = A_PARAMETER_TYPE_LIST;
    if (tokcmp(toks[*i], (Token) {T_PUNCTUATOR, ","})
        && tokcmp(toks[*i+1], (Token) {T_PUNCTUATOR, "..."})) {
      *i += 2;
      astcpy(&out.a2.ptr, (Ast) {A_THREE_DOTS});
    } else {
      astcpy(&out.a2.ptr, (Ast) {A_NONE});
    }
  }
  return out;
}

Ast m_abstract_declarator(int* i) {
  return (Ast) {A_NONE};
}

Ast m_parameter_declaration(int* i) {
  Ast out = {A_PARAMETER_DECLARATION};
  Type dec_spec = m_declaration_specifier_list(i);
  if (dec_spec.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast second = m_declarator(i);
  if (second.node_type == A_NONE) {
    second = m_abstract_declarator(i);
  }
  out.type = dec_spec;
  astcpy(&out.a1.ptr, second);

  return out;
}

Ast m_identifier(int* i) {
  if (toks[*i].type == T_IDENTIFIER) {
    *i ++;
    Ast out = {A_IDENTIFIER};
    out.a1.str = toks[*i -1].val.str;
    return out;
  } else {
    return (Ast) {A_NONE};
  }
}

Ast m_identifier_list(int* i) {
  return m_comma_list(i, m_identifier, A_IDENTIFIER_LIST);
}

Ast m_direct_declarator(int* i) {
  // TODO: Finish direct declaratos
  int j = *i;
  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "["})) {

  }

  return (Ast) {A_NONE};
}

VECTOR(Ast);
Ast m_direct_declarator_list(int* i) {
  int j = *i;
  Vector_Ast list;
  init_vector_Ast(&list);

  // Parse first element
  Ast tmp;
  if (toks[j].type == T_IDENTIFIER) {
    tmp.node_type = A_IDENTIFIER;
    ast_strcpy(&tmp.a1.str, toks[j].val.str);
    j++;
  } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("})) {
    int k = j+1;
    tmp = m_declarator(&k);
    if (tmp.node_type !=  A_NONE && tokcmp(toks[k], (Token) {T_PUNCTUATOR, ")"})) {
      j = k+1;
    } else {
      return (Ast) {A_NONE};
    }
  }
  push_Ast(&list, tmp);

  // TODO: Parse all other direct declarators
  Ast out = {A_DIRECT_DECLARATOR_LIST};
  out.a1.ptr = list.vec;
  *i = j;
  return out;
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
