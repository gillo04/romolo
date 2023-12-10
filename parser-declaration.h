#pragma once
#include "parser.h"

Type m_type_token(int* i, const char* tt_strs[], const int tt_types[]);

Type m_storage_class_spec(int* i);

Type m_type_spec(int* i);

Type m_type_qualifier(int* i);

Type m_function_spec(int* i);

Type m_alignment_specifier(int* i);

Type m_declaration_specifier_list(int* i);

Ast m_declarator(int* i);

Ast m_initializer(int* i);

Ast m_init_declarator(int* i);

Ast* m_init_declarator_list(int* i);

Ast m_declaration(int* i);
