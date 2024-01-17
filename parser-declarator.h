#pragma once
#include "parser.h"

Ast m_abstract_declarator(int* i);

Ast m_direct_abstract_declarator(int* i);

Ast m_type_name(int* i);

Ast m_type_qualifier_list(int* i);

Ast m_pointer(int* i);

Ast m_parameter_list(int* i);

Ast m_parameter_type_list(int* i);

Ast m_parameter_declaration(int* i);

Ast m_identifier(int* i);

Ast m_identifier_list(int* i);

Ast m_direct_declarator(int* i);

Ast m_direct_declarator_list(int* i);

Ast m_declarator(int* i);
