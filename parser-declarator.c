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

Ast m_identifier(int* i) {
  if (toks[*i].type == T_IDENTIFIER) {
    (*i)++;
    Ast out = {A_IDENTIFIER};
    ast_strcpy(&out.a1.str, toks[*i -1].val.str);
    return out;
  } else {
    return (Ast) {A_NONE};
  }
}

Ast m_identifier_list(int* i) {
  return m_comma_list(i, m_identifier, A_IDENTIFIER_LIST);
}

Ast m_abstract_declarator(int* i) {
  int j = *i;
  Ast out = {A_ABSTRACT_DECLARATOR};
  Ast ptr = m_pointer(&j);
  Ast dad = m_direct_abstract_declarator(&j);
  if (dad.node_type == A_NONE && ptr.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }
  astcpy(&out.a1.ptr, ptr);
  astcpy(&out.a2.ptr, dad);
  *i = j;
  return out;
}

Ast m_direct_abstract_declarator(int* i) {
  int j = *i;
  Ast out = {A_DIRECT_ABSTRACT_DECLARATOR};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast) * 2);
  int k = 1;

  // Parse first element
  Ast tmp;
  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("})) {
    int l = j+1;
    tmp = m_abstract_declarator(&l);
    if (tmp.node_type !=  A_NONE && tokcmp(toks[l], (Token) {T_PUNCTUATOR, ")"})) {
      j = l+1;
      out.a1.ptr[0] = tmp;
    } else {
      k--;
    }
  } else {
    k--;
  }

  // Parse additional elements
  do {
    int l = j;
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (k+1));
    Ast tmp = {A_NONE};
    if (tokcmp(toks[l], (Token) {T_PUNCTUATOR, "["})) {
      l++;
      tmp.node_type = A_ARRAY_DIRECT_DECLARATOR;
      tmp.a3.num = 0;

      // TODO: check direct declarator constraints
      if (tokcmp(toks[l], (Token) {T_KEYWORD, "static"})) {
        l++;
        tmp.a3.num = 1;
      }
      Ast tql = m_type_qualifier_list(&l);
      astcpy(&tmp.a1.ptr, tql);

      if (tmp.a3.num == 0 && tokcmp(toks[l], (Token) {T_KEYWORD, "static"})) {
        l++;
        tmp.a3.num = 1;
      }

      Ast aexp = m_assignment_expression(&l);
      astcpy(&tmp.a2.ptr, aexp);

      if (!tokcmp(toks[l], (Token) {T_PUNCTUATOR, "]"})) {
        break;
      }
      l++;
    } else if (tokcmp(toks[l], (Token) {T_PUNCTUATOR, "("})) {
      l++;
      tmp.node_type = A_FUNCTION_DIRECT_DECLARATOR;
      astcpy(&tmp.a1.ptr, m_parameter_type_list(&l));

      if (!tokcmp(toks[l], (Token) {T_PUNCTUATOR, ")"})) {
        tmp = (Ast) {A_NONE};
        out.a1.ptr[k] = tmp;
        break;
      }
      l++;
    } else {
      tmp = (Ast) {A_NONE};
      out.a1.ptr[k] = tmp;
      break;
    }

    out.a1.ptr[k] = tmp;
    j = l;
    k++;
  } while (out.a1.ptr[k-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast m_type_name(int* i) {
  int j = *i;
  Ast out = {A_TYPE_NAME};
  Ast sql = m_specifier_qualifier_list(&j);
  if (sql.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast adec = m_abstract_declarator(&j);
  astcpy(&out.a1.ptr, sql);
  astcpy(&out.a2.ptr, adec);

  *i = j;
  return out;
}

Ast m_parameter_declaration(int* i) {
  Ast out = {A_PARAMETER_DECLARATION};
  astcpy(&out.a1.ptr, m_declaration_specifier_list(i));
  if (out.a1.ptr->a1.ptr[0].node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast second = m_declarator(i);
  if (second.node_type == A_NONE) {
    second = m_abstract_declarator(i);
  }
  astcpy(&out.a2.ptr, second);

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
      (*i) += 2;
      astcpy(&out.a2.ptr, (Ast) {A_THREE_DOTS});
    } else {
      astcpy(&out.a2.ptr, (Ast) {A_NONE});
    }
  }
  return out;
}

Ast m_type_qualifier_list(int* i) {
  int j = *i;
  Ast out = {A_TYPE_QUALIFIER_LIST};
  int k = 0;
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (k + 1));
    out.a1.ptr[k] = m_type_qualifier(&j);

    k++;
  } while (out.a1.ptr[k-1].node_type != A_NONE);
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
  astcpy(&out.a1.ptr, m_type_qualifier_list(&j));
  if (out.a1.ptr->node_type != A_NONE) {
    astcpy(&out.a2.ptr, m_pointer(&j));
  } else {
    return (Ast) {A_NONE};
  }

  *i = j;
  return out;
}

Ast m_direct_declarator(int* i) {
  int j = *i;
  Ast out = {A_DIRECT_DECLARATOR};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast) * 2);
  int k = 1;

  // Parse first element
  Ast tmp;
  if (toks[j].type == T_IDENTIFIER) {
    tmp.node_type = A_IDENTIFIER;
    ast_strcpy(&tmp.a1.str, toks[j].val.str);
    j++;
  } else if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, "("})) {
    int l = j+1;
    tmp = m_declarator(&l);
    if (tmp.node_type !=  A_NONE && tokcmp(toks[l], (Token) {T_PUNCTUATOR, ")"})) {
      j = l+1;
    } else {
      return (Ast) {A_NONE};
    }
  } else {
    return (Ast) {A_NONE};
  }
  out.a1.ptr[0] = tmp;

  // Parse additional elements
  do {
    int l = j;
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (k+1));
    Ast tmp = {A_NONE};
    if (tokcmp(toks[l], (Token) {T_PUNCTUATOR, "["})) {
      l++;
      tmp.node_type = A_ARRAY_DIRECT_DECLARATOR;
      tmp.a3.num = 0;

      // TODO: check direct declarator constraints
      if (tokcmp(toks[l], (Token) {T_KEYWORD, "static"})) {
        l++;
        tmp.a3.num = 1;
      }
      Ast tql = m_type_qualifier_list(&l);
      astcpy(&tmp.a1.ptr, tql);

      if (tmp.a3.num == 0 && tokcmp(toks[l], (Token) {T_KEYWORD, "static"})) {
        l++;
        tmp.a3.num = 1;
      }

      Ast aexp = m_assignment_expression(&l);
      astcpy(&tmp.a2.ptr, aexp);

      if (!tokcmp(toks[l], (Token) {T_PUNCTUATOR, "]"})) {
        break;
      }
      l++;
    } else if (tokcmp(toks[l], (Token) {T_PUNCTUATOR, "("})) {
      l++;
      tmp.node_type = A_FUNCTION_DIRECT_DECLARATOR;
      astcpy(&tmp.a1.ptr, m_parameter_type_list(&l));
      if (tmp.a1.ptr->node_type == A_NONE) {
        astcpy(&tmp.a1.ptr, m_identifier_list(&l));
      }

      if (!tokcmp(toks[l], (Token) {T_PUNCTUATOR, ")"})) {
        break;
      }
      l++;
    } else {
      tmp = (Ast) {A_NONE};
    }

    out.a1.ptr[k] = tmp;
    j = l;
    k++;
  } while (out.a1.ptr[k-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast m_declarator(int* i) {
  Ast out = {A_DECLARATOR};
  astcpy(&out.a1.ptr, m_pointer(i));
  astcpy(&out.a2.ptr, m_direct_declarator(i));
  if (out.a2.ptr->node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  return out;
}
