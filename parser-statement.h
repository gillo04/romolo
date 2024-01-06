#pragma once
#include "parser.h"

Ast m_labeled_statement(int* i);

Ast m_block_item(int* i);

Ast m_compound_statement(int* i);

Ast m_expression_statement(int* i);

Ast m_if_else(int* i);

Ast m_switch(int* i);

Ast m_selection_statement(int* i);

Ast m_while_loop(int* i);

Ast m_do_while_loop(int* i);

Ast m_for_loop(int* i);

Ast m_jump_statement(int* i);

Ast m_iteration_statement(int* i);

Ast m_statement(int* i);
