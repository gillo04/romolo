#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

Type m_type_token(int* i, const char* tt_strs[], const int tt_types[]) {
  Type out = {E_NONE};
  if (toks[*i].type == T_NONE) {
    return out;
  }

  for (int j = 0; tt_strs[j] != 0; j++) {
    if (strcmp(toks[*i].val.str, tt_strs[j]) == 0) {
      (*i)++;
      out.node_type = tt_types[j];
      out.next = 0;
      break;
    }
  }
  return out;
}

const char* sc_strs[] = {"typedef", "extern", "static", "auto", "register", 0};
const int sc_types[] = {E_TYPEDEF, E_EXTERN, E_STATIC, E_AUTO, E_REGISTER, 0};
Type m_storage_class_spec(int* i) {
  return m_type_token(i, sc_strs, sc_types);
}

const char* ts_strs[] = {"void", "char", "short", "int", "long", "float", "double", "signed", "unsigned", "_Bool", 0};
const int ts_types[] = {E_VOID, E_CHAR, E_SHORT, E_INT, E_LONG, E_FLOAT, E_DOUBLE, E_SIGNED, E_UNSIGNED, E_BOOL, 0};
Type m_type_spec(int* i) {
  return m_type_token(i, ts_strs, ts_types);
}

const char* tq_strs[] = {"const", "restrict", "volatile", 0};
const int tq_types[] = {E_CONST, E_RESTRIC, E_VIOLATE, 0};
Type m_type_qualifier(int* i) {
  return m_type_token(i, tq_strs, tq_types);
}

const char* fs_strs[] = {"inline", "_Noreturn", 0};
const int fs_types[] = {E_INLINE, E_NORETURN, 0};
Type m_function_spec(int* i) {
  return m_type_token(i, fs_strs, fs_types);
}

// TODO: Implement alignment specifiers
Type m_alignment_specifier(int* i) {
  return (Type) {E_NONE};
}

Type(*declaration_specifiers[])(int*) = {m_storage_class_spec, m_type_spec, m_type_qualifier, m_function_spec, m_alignment_specifier};
Type m_declaration_specifier_list(int* i) {
  int j = *i;
  Type out = {E_NONE};
  Type tmp;
  do {
    for (int k = 0; k < sizeof(declaration_specifiers)/sizeof(void*); k++) {
      tmp = declaration_specifiers[k](&j);
      if (tmp.node_type != E_NONE) {
        break;
      }
    }

    if (tmp.node_type != E_NONE) {
      tmp.next = (Type*) malloc(sizeof(Type));
      *tmp.next = out;
      out = tmp;
    }
  } while (tmp.node_type != E_NONE);
  // TODO: Check that type follows the rules

  *i = j;
  return out;
}

Ast m_declarator(int* i);

Ast m_initializer(int* i) {
  return (Ast) {A_NONE};
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
    *i++;
    Ast initializer = m_initializer(i);
    *out.a2.ptr = initializer;
  }

  return out;
}

// TODO: Optimize stack resizing
Ast* m_init_declarator_list(int* i) {
  int j = *i;
  Ast* out = malloc(sizeof(Ast));
  *out = m_init_declarator(&j);
  int k = 0;
  while (out[k].node_type != A_NONE && tokcmp(toks[j], (Token) {T_PUNCTUATOR, ","})) {
    j++;
    k++;
    realloc(out, sizeof(Ast)*(k+1));
    out[k] = m_init_declarator(&j);
  }
  if (tokcmp(toks[j-1], (Token) {T_PUNCTUATOR, ","})) {
    j--;
  }
  *i = j;
  return out;
}

Ast m_declaration(int* i) {
  Ast out = {A_DECLARATION};
  out.type = m_declaration_specifier_list(i);
  if (out.type.node_type == E_NONE) {
    return (Ast) {A_NONE};
  }

  out.a1.ptr = m_init_declarator_list(i);
  if (out.a1.ptr->node_type == E_NONE) {
    printf("Declaration without init-declarator list at:\n\t");
    print_source_line(src, toks[*i].line);
    printf("\n");
    return (Ast) {A_NONE};
  }
  
  // TODO: match ending ';'
  
  return out;
}
