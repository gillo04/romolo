#pragma once
#include "parser.h"

Ast m_struct_declarator(int* i);

Ast m_struct_declarator_list(int* i);

Ast m_specifier_qualifier_list(int* i);

Ast m_struct_declaration(int* i);

Ast m_struct_declaration_list(int* i);

Ast m_struct_or_union_specifier(int* i);

Ast m_enumerator(int* i);

Ast m_enumerator_list(int* i);

Ast m_enum_specifier(int* i);

Ast m_type_token(int* i, const char* tt_strs[], const int tt_types[]);

Ast m_storage_class_spec(int* i);

Ast m_type_spec(int* i);

Ast m_type_qualifier(int* i);

Ast m_function_spec(int* i);

Ast m_alignment_specifier(int* i);

Ast m_declaration_specifier_list(int* i);

Ast m_declarator(int* i);

Ast m_initializer(int* i);

Ast m_init_declarator(int* i);

Ast m_init_declarator_list(int* i);

Ast m_declaration(int* i);
