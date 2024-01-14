#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

Ast m_type_token(int* i, const char* tt_strs[], const int tt_types[]) {
  Ast out = {A_NONE};
  if (toks[*i].type == T_NONE) {
    return out;
  }

  for (int j = 0; tt_strs[j] != 0; j++) {
    if (tokcmp(toks[*i], (Token) {T_KEYWORD, tt_strs[j]})) {
      (*i)++;
      out.node_type = tt_types[j];
      break;
    }
  }
  return out;
}

const char* sc_strs[] = {"typedef", "extern", "static", "auto", "register", 0};
const int sc_types[] = {A_TYPEDEF, A_EXTERN, A_STATIC, A_AUTO, A_REGISTER, 0};
Ast m_storage_class_spec(int* i) {
  return m_type_token(i, sc_strs, sc_types);
}

const char* ts_strs[] = {"void", "char", "short", "int", "long", "float", "double", "signed", "unsigned", "_Bool", 0};
const int ts_types[] = {A_VOID, A_CHAR, A_SHORT, A_INT, A_LONG, A_FLOAT, A_DOUBLE, A_SIGNED, A_UNSIGNED, A_BOOL, 0};
Ast m_type_spec(int* i) {
  return m_type_token(i, ts_strs, ts_types);
}

const char* tq_strs[] = {"const", "restrict", "volatile", 0};
const int tq_types[] = {A_CONST, A_RESTRIC, A_VIOLATE, 0};
Ast m_type_qualifier(int* i) {
  return m_type_token(i, tq_strs, tq_types);
}

const char* fs_strs[] = {"inline", "_Noreturn", 0};
const int fs_types[] = {A_INLINE, A_NORETURN, 0};
Ast m_function_spec(int* i) {
  return m_type_token(i, fs_strs, fs_types);
}

// TODO: Implement alignment specifiers
Ast m_alignment_specifier(int* i) {
  return (Ast) {A_NONE};
}

Ast(*declaration_specifiers[])(int*) = {m_storage_class_spec, m_type_spec, m_type_qualifier, m_function_spec, m_alignment_specifier};
Ast m_declaration_specifier_list(int* i) {
  int j = *i;
  Ast out = {A_DECLARATION_SPECIFIERS};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    for (int k = 0; k < sizeof(declaration_specifiers)/sizeof(void*); k++) {
      out.a1.ptr[l] = declaration_specifiers[k](&j);
      if (out.a1.ptr[l].node_type != A_NONE) {
        break;
      }
    }

    l++;
  } while (out.a1.ptr[l-1].node_type != A_NONE);
  // TODO: Check that type follows the rules

  *i = j;
  return out;
}

Ast m_declarator(int* i);

Ast m_initializer(int* i) {
  (*i)++;
  return (Ast) {A_INITIALIZER};
}

Ast m_init_declarator(int* i) {
  Ast out = {A_INIT_DECLARATOR};
  Ast declarator = m_declarator(i);
  if (declarator.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  astcpy(&out.a1.ptr, declarator);
  astcpy(&out.a2.ptr, (Ast) {A_NONE});
  if (tokcmp(toks[*i], (Token) {T_PUNCTUATOR, "="})) {
    (*i)++;
    Ast initializer = m_initializer(i);
    if (initializer.node_type == A_NONE) {
      return (Ast) {A_NONE};
    }
    *out.a2.ptr = initializer;
  }

  return out;
}

Ast* m_init_declarator_list(int* i) {
  Ast out = m_comma_list(i, m_init_declarator, A_INIT_DECLARATOR);
  return out.a1.ptr;
}

Ast m_declaration(int* i) {
  Ast out = {A_DECLARATION};
  astcpy(&out.a1.ptr, m_declaration_specifier_list(i));
  if (out.a1.ptr->node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  out.a2.ptr = m_init_declarator_list(i);
  if (out.a2.ptr->node_type == A_NONE) {
    printf("Declaration without init-declarator list at:\n\t");
    print_source_line(src, toks[*i].line);
    printf("\n");
    return (Ast) {A_NONE};
  }
  
  if (tokcmp(toks[*i], (Token) {T_PUNCTUATOR, ";"})) {
    (*i)++;
    return out;
  }
  return (Ast) {A_NONE};
}
