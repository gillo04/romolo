#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "log.h"
#include "lexer.h"
#include "data-structures.h"
#include "parser.h"
#include "optimizer.h"
#include "generator.h"

int main(int argc, char* argv[]) {
  char* source_code = load(argc, argv);
  if (source_code == 0) {
    return 1;
  }
  printf("SOURCE LOADED\n");

  Token* tokens = lexer(source_code);
  if (tokens == 0) {
    printf("Error lexing\n");
    return 1;
  }
  printf("LEXER FINISHED\n");
  // print_tokens(tokens);
  
  Ast ast = parser(tokens, source_code);
  if (ast.node_type == 0) {
    printf("Error parsing\n");
    return 1;
  }  
  printf("PARSER FINISHED\n");

  // optimizer(&ast);
  // printf("OPTIMIZER FINISHED\n");
  print_ast(&ast, 0);

  return 0;

  char* assembly_code = generator(&ast);
  if (assembly_code == 0) {
    printf("Error generating code\n");
    return 1;
  }  
  printf("CODE GENERATION FINISHED\n\n");
  printf("%s\n", assembly_code);

  save(argc, argv, assembly_code);

  free(source_code);
  free_tokens(tokens);
  // TODO: free_ast(ast);
  free(assembly_code);
}
