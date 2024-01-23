#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-declaration.h"
#include "parser-declarator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

Ast m_struct_declarator(int* i) {
  int j = *i;
  Ast out = {A_STRUCT_DECLARATOR};
  Ast decl = m_declarator(&j);
  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, ":"})) {
    j++;
    Ast cexp = m_expression(&j);
    if (cexp.node_type == A_NONE) {
      return (Ast) {A_NONE};
    }
    astcpy(&out.a1.ptr, decl);
    astcpy(&out.a2.ptr, cexp);
    *i = j;
    return out;
  }

  if (decl.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }
  astcpy(&out.a1.ptr, decl);
  astcpy(&out.a2.ptr, (Ast) {A_NONE});
  *i = j;
  return out;
}

Ast m_struct_declarator_list(int* i) {
  return m_comma_list(i, m_struct_declarator, A_STRUCT_DECLARATOR_LIST);
}

Ast(*spec_qual[])(int*) = {m_type_spec, m_type_qualifier};
Ast m_specifier_qualifier_list(int* i) {
  int j = *i;
  Ast out = {A_SPECIFIER_QUALIFIER_LIST};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    for (int k = 0; k < sizeof(spec_qual)/sizeof(void*); k++) {
      out.a1.ptr[l] = spec_qual[k](&j);
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

Ast m_struct_declaration(int* i) {
  // TODO: Static assert declaration
  Ast out = {A_STRUCT_DECLARATION};
  Ast spec_qual = m_specifier_qualifier_list(i);
  if (spec_qual.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }
  Ast sdl = m_struct_declarator_list(i);
  if (!tokcmp(toks[*i], (Token) {T_PUNCTUATOR, ";"})) {
    return (Ast) {A_NONE};
  }
  (*i)++;
  astcpy(&out.a1.ptr, spec_qual);
  astcpy(&out.a2.ptr, sdl);
  return out;
}

Ast m_struct_declaration_list(int* i) {
  int j = *i;
  Ast out = {A_STRUCT_DECLARATION_LIST};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int l = 0;
  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (l+1));
    out.a1.ptr[l] = m_struct_declaration(&j);
    l++;
  } while (out.a1.ptr[l-1].node_type != A_NONE);

  *i = j;
  return out;
}

Ast m_struct_or_union_specifier(int* i) {
  int j = *i;
  Ast out;
  if (tokcmp(toks[j], (Token) {T_KEYWORD, "struct"})) {
    j++;
    out.node_type = A_STRUCT_SPECIFIER;
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "union"})) {
    j++;
    out.node_type = A_UNION_SPECIFIER;
  } else {
    return (Ast) {A_NONE};
  }

  Ast ident = m_identifier(&j);
  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "{"})) {
    if (ident.node_type == A_NONE){
      return (Ast) {A_NONE};
    }
    astcpy(&out.a1.ptr, ident);
    return out;
  }
  j++;

  Ast sdl = m_struct_declaration_list(&j);

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "}"}) || sdl.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }
  j++;
  
  astcpy(&out.a1.ptr, ident);
  astcpy(&out.a2.ptr, sdl);

  *i = j;
  return out;
}

Ast m_enumerator(int* i) {
  int j = *i;
  if (toks[j].type != T_IDENTIFIER) {
    return (Ast) {A_NONE};
  }
  j++;
  Ast out = {A_ENUMERATOR};

  ast_strcpy(&out.a1.str, toks[j-1].val.str);
  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "="})) {
    astcpy(&out.a2.ptr, (Ast) {A_NONE});
    *i = j;
    return out;
  }
  j++;
  Ast exp = m_conditional_expression(&j);
  if (exp.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  astcpy(&out.a2.ptr, exp);
  *i = j;
  return out;
}

Ast m_enumerator_list(int* i) {
  return m_comma_list(i, m_enumerator, A_ENUMERATOR_LIST);
}

Ast m_enum_specifier(int* i) {
  int j = *i;
  if (!tokcmp(toks[j], (Token) {T_KEYWORD, "enum"})) {
    return (Ast) {A_NONE};
  }
  j++;

  Ast out = {A_ENUM_SPECIFIER};
  if (toks[j].type == T_IDENTIFIER) {
    ast_strcpy(&out.a1.str, toks[j].val.str);
    j++;
  } else {
    out.a1.str = 0;
  }

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "{"})) {
    if (out.a1.str == 0) {
      return (Ast) {A_NONE};
    }
    astcpy(&out.a2.ptr, (Ast) {A_NONE});
    *i = j;
    return out;
  }
  j++;

  Ast elist = m_enumerator_list(&j);

  if (tokcmp(toks[j], (Token) {T_PUNCTUATOR, ","})) {
    j++;
  }
  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "}"})) {
    return (Ast) {A_NONE};
  }
  j++;
  astcpy(&out.a2.ptr, elist);

  *i = j;
  return out;
}

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
  Ast tmp = m_type_token(i, ts_strs, ts_types);
  if (tmp.node_type != A_NONE) {
    return tmp;
  }

  tmp = m_struct_or_union_specifier(i);
  if (tmp.node_type != A_NONE) {
    return tmp;
  }

  tmp = m_enum_specifier(i);
  return tmp;
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

  *i = j;
  return out;
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

Ast m_init_declarator_list(int* i) {
  return m_comma_list(i, m_init_declarator, A_INIT_DECLARATOR_LIST);
}

Ast m_declaration(int* i) {
  Ast out = {A_DECLARATION};
  astcpy(&out.a1.ptr, m_declaration_specifier_list(i));
  if (out.a1.ptr->node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  astcpy(&out.a2.ptr, m_init_declarator_list(i));
  if (tokcmp(toks[*i], (Token) {T_PUNCTUATOR, ";"})) {
    (*i)++;
    return out;
  }
  return (Ast) {A_NONE};
}
