#pragma once
#include "parser.h"

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
