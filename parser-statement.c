#include "log.h"
#include "parser.h"
#include "parser-utils.h"
#include "parser-expression.h"
#include "parser-statement.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Token* toks;
extern char* src;

Ast m_labeled_statement(int* i) {
  int j = *i;
  if (toks[j].type == T_IDENTIFIER && tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ":"})) {
    Ast out = {A_LABEL};
    ast_strcpy(&out.a1.str, toks[j].val.str);

    int k = j + 2;
    Ast stat = m_statement(&k);
    if (stat.node_type != A_NONE) {
      astcpy(&out.a2.ptr, stat);
      *i = k;
      return out;
    }
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "case"})) {
    int k = j+1;
    Ast out = {A_CASE};
    Ast exp = m_expression(&k);
    if (exp.node_type != A_NONE && tokcmp(toks[k], (Token) {T_PUNCTUATOR, ":"})) {
      k++;
      Ast stat = m_statement(&k);
      if (stat.node_type != A_NONE) {
        astcpy(&out.a1.ptr, exp);
        astcpy(&out.a2.ptr, stat);
        *i = k;
        return out;
      }
    }
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "default"}) && tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ":"})) {
    Ast out = {A_DEFAULT};
    int k = j + 2;
    Ast stat = m_statement(&k);
    if (stat.node_type != A_NONE) {
      astcpy(&out.a1.ptr, stat);
      *i = k;
      return out;
    }
  }

  return (Ast) {A_NONE};
}

Ast m_block_item(int* i) {
  // TODO: add declaration as option
  return m_statement(i);
}

Ast m_compound_statement(int* i) {
  int j = *i;
  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "{"})) {
    return (Ast) {A_NONE};
  }
  j++;

  Ast out = {A_COMPOUND_STATEMENT};
  out.a1.ptr = (Ast*) malloc(sizeof(Ast));
  int k = 0;

  do {
    out.a1.ptr = (Ast*) realloc(out.a1.ptr, sizeof(Ast) * (k+1));
    out.a1.ptr[k++] = m_block_item(&j);
    printf("%d\n", out.a1.ptr[k-1].node_type);
  } while (out.a1.ptr[k-1].node_type != A_NONE);

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, "}"})) {
    return (Ast) {A_NONE};
  }
  *i = j+1;
  return out;
}

Ast m_expression_statement(int* i) {
  Ast out = m_expression(i);
  if (tokcmp(toks[*i], (Token) {T_PUNCTUATOR, ";"})) {
    if (out.node_type == A_NONE) {
      out.node_type = A_NULL_STATEMENT;
    }
    *i += 1;
    return out;
  }
  return (Ast) {A_NONE};
}

Ast m_if_else(int* i) {
  int j = *i;
  if (!(tokcmp(toks[j], (Token) {T_KEYWORD, "if"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, "("}))) {
    return (Ast) {A_NONE};
  }

  j += 2;
  Ast exp = m_expression(&j);

  if (!(tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"}) &&
    exp.node_type != A_NONE)) {
    return (Ast) {A_NONE};
  }
  
  j++;
  Ast then_stat = m_statement(&j);

  if (then_stat.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  if (tokcmp(toks[j], (Token) {T_KEYWORD, "else"})) {
    j++;
    Ast else_stat = m_statement(&j);
    if (else_stat.node_type == A_NONE) {
      return (Ast) {A_NONE};
    }
    Ast out = {A_IF_ELSE};
    astcpy(&out.a1.ptr, exp);
    astcpy(&out.a2.ptr, then_stat);
    astcpy(&out.a3.ptr, else_stat);
    *i = j;
    return out;
  } else {
    Ast out = {A_IF};
    astcpy(&out.a1.ptr, exp);
    astcpy(&out.a2.ptr, then_stat);
    *i = j;
    return out;
  }
}

Ast m_switch(int* i) {
  int j = *i;
  if (!(tokcmp(toks[j], (Token) {T_KEYWORD, "switch"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, "("}))) {
    return (Ast) {A_NONE};
  }

  j += 2;
  Ast exp = m_expression(&j);

  if (!(tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"}) &&
    exp.node_type != A_NONE)) {
    return (Ast) {A_NONE};
  }
  
  j++;
  Ast stat = m_statement(&j);

  if (stat.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast out = {A_SWITCH};
  astcpy(&out.a1.ptr, exp);
  astcpy(&out.a2.ptr, stat);
  *i = j;
  return out;
}

Ast m_selection_statement(int* i) {
  Ast out = m_if_else(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  return m_switch(i);
}

Ast m_while_loop(int* i) {
  int j = *i;
  if (!(tokcmp(toks[j], (Token) {T_KEYWORD, "while"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, "("}))) {
    return (Ast) {A_NONE};
  }

  j += 2;
  Ast exp = m_expression(&j);

  if (!(tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"}) &&
    exp.node_type != A_NONE)) {
    return (Ast) {A_NONE};
  }
  
  j++;
  Ast stat = m_statement(&j);

  if (stat.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast out = {A_WHILE};
  astcpy(&out.a1.ptr, exp);
  astcpy(&out.a2.ptr, stat);
  *i = j;
  return out; 
}

Ast m_do_while_loop(int* i) {
  int j = *i;
  if (!tokcmp(toks[j], (Token) {T_KEYWORD, "do"})) {
    return (Ast) {A_NONE};
  }
  j++;
  Ast stat = m_statement(&j);

  if (!(tokcmp(toks[j], (Token) {T_KEYWORD, "while"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, "("}) &&
    stat.node_type != A_NONE)) {
    return (Ast) {A_NONE};
  }

  j += 2;
  Ast exp = m_expression(&j);

  if (!(tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ";"}) &&
    exp.node_type != A_NONE)) {
    return (Ast) {A_NONE};
  }
  
  j += 2;
  Ast out = {A_DO_WHILE};
  astcpy(&out.a1.ptr, exp);
  astcpy(&out.a2.ptr, stat);
  *i = j;
  return out; 
}

Ast m_for_loop(int* i) {
  // TODO: Add declarator as possible first clause
  int j = *i;
  if (!(tokcmp(toks[j], (Token) {T_KEYWORD, "for"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, "("}))) {
    return (Ast) {A_NONE};
  }

  j += 2;
  Ast exp1 = m_expression(&j);

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, ";"})) {
    return (Ast) {A_NONE};
  }

  j++;
  Ast exp2 = m_expression(&j);

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, ";"})) {
    return (Ast) {A_NONE};
  }

  j++;
  Ast exp3 = m_expression(&j);

  if (!tokcmp(toks[j], (Token) {T_PUNCTUATOR, ")"})) {
    return (Ast) {A_NONE};
  }
  
  j++;
  Ast stat = m_statement(&j);

  if (stat.node_type == A_NONE) {
    return (Ast) {A_NONE};
  }

  Ast out = {A_FOR};
  Ast clauses = {A_FOR_CLAUSES};
  astcpy(&clauses.a1.ptr, exp1);
  astcpy(&clauses.a2.ptr, exp2);
  astcpy(&clauses.a3.ptr, exp3);

  astcpy(&out.a1.ptr, clauses);
  astcpy(&out.a2.ptr, stat);
  *i = j;
  return out; 
}

Ast m_jump_statement(int* i) {
  int j = *i;
  if (tokcmp(toks[j], (Token) {T_KEYWORD, "goto"}) &&
    toks[j+1].type == T_IDENTIFIER &&
    tokcmp(toks[j+2], (Token) {T_PUNCTUATOR, ";"})) {
    Ast out = {A_GOTO};
    ast_strcpy(&out.a1.str, toks[j+1].val.str);
    *i = j+3;
    return out;
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "continue"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ";"})) {
    Ast out = {A_CONTINUE};
    *i = j+2;
    return out;
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "break"}) &&
    tokcmp(toks[j+1], (Token) {T_PUNCTUATOR, ";"})) {
    Ast out = {A_BREAK};
    *i = j+2;
    return out;
  } else if (tokcmp(toks[j], (Token) {T_KEYWORD, "return"})) {
    j++;
    Ast out = {A_RETURN};
    Ast exp = m_expression(&j);
    astcpy(&out.a1.ptr, exp);
    *i = j+1;
    return out;
  }
  
  return (Ast) {A_NONE};
}

Ast m_iteration_statement(int* i) {
  Ast out = m_while_loop(i);
  if (out.node_type != A_NONE) {
    return out;
  }
  
  out = m_do_while_loop(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  return m_for_loop(i);
}

Ast m_statement(int* i) {
  Ast out = m_labeled_statement(i);
  if (out.node_type != A_NONE) {
    return out;
  }
  
  out = m_compound_statement(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  out = m_expression_statement(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  out = m_selection_statement(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  out = m_iteration_statement(i);
  if (out.node_type != A_NONE) {
    return out;
  }

  return m_jump_statement(i);
}
