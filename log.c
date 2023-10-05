#include <stdio.h>
#include "lexer.h"
#include "parser.h"

void print_tokens(Token* t) {
  while (t->type != T_NONE) {
    printf("%d\t", t->line);
    if (t->type == T_CONSTANT) {
      printf("CONSTANT\t%lld\n", t->val.num);
    } else {
      switch(t->type) {
        case T_KEYWORD:
          printf("KEYWORD\t\t%s\n", t->val.str);
          break;
        case T_IDENTIFIER:
          printf("IDENTIFIER\t%s\n", t->val.str);
          break;
        case T_STRING_LITERAL:
          printf("STRING LITERAL\t%s\n", t->val.str);
          break;
        case T_PUNCTUATOR:
          printf("PUNCTUATOR\t%s\n", t->val.str);
          break;
        default:
          printf("Unrecognized token\n");
          break;
      }
    }
    t++;
  }
}

void print_source_line(char* source, int line) {
  while(source[line] != 0 && source[line] != '\n') {
    putchar(source[line++]);
  }
}
