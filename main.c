#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "log.h"
#include "lexer.h"
#include "data-structures.h"
#include "parser.h"
#include "optimizer.h"

int main(int argc, char* argv[]) {
  char* source_code = load(argc, argv);
  if (source_code == 0) {
    return 1;
  }
  printf("SOURCE LOADED\n");

  Token* tokens = lexer(source_code);
  if (tokens == 0) {
    return 1;
  }
  printf("LEXER FINISHED\n");
  // print_tokens(tokens);
  
  Ast ast = parser(tokens, source_code);
  if (ast.node_type == 0) {
    return 1;
  }  
  printf("PARSER FINISHED\n");
  print_ast(&ast, 0);

  // optimizer(&ast);
  // printf("OPTIMIZER FINISHED\n");

  free(source_code);
  free_tokens(tokens);
}
