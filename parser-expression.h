#pragma once
#include "lexer.h"
#include "parser.h"

Ast m_expression(int* i);

Ast m_primary_expression(int* i);

Ast m_postfix_expression(int* i);

Ast m_unary_expression(int* i);

Ast m_binary_expression(int* i, Ast (*prev_exp)(), char* strings[], int types[]);

Ast m_multiplicative_expression(int* i);

Ast m_additive_expression(int* i);

Ast m_shift_expression(int* i);

Ast m_relational_expression(int* i);

Ast m_equality_expression(int* i);

Ast m_bitwise_and_expression(int* i);

Ast m_bitwise_xor_expression(int* i);

Ast m_bitwise_or_expression(int* i);

Ast m_logic_and_expression(int* i);

Ast m_logic_or_expression(int* i);

Ast m_conditional_expression(int* i);

Ast m_assignment_expression(int* i);

Ast m_comma_expression(int* i);
