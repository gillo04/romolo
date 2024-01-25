#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "log.h"
#include "lexer.h"
#include "data-structures.h"
#include "parser.h"
#include "optimizer.h"
#include "generator.h"
#include "memory-manager.h"

int main(int argc, char* argv[]) {
  char* source_code = load(argc, argv);
  if (source_code == 0) {
    return 1;
  }
  log_msg(INFO, "source loaded\n", -1);

  init_logger(source_code);

  Token* tokens = lexer(source_code);
  if (tokens == 0) {
    log_msg(ERROR, "error lexing\n", -1);
    return 1;
  }
  log_msg(INFO, "lexer finished\n", -1);
  // print_tokens(tokens);
  
  Ast ast = parser(tokens, source_code);
  if (ast.node_type == 0) {
    log_msg(ERROR, "error parsing\n", -1);
    return 1;
  }  
  log_msg(INFO, "parser finished\n", -1);

  // optimizer(&ast);
  // log_msg(INFO, "optimizer finished\n", -1);
  print_ast(&ast, 0);
  return 0;

  /* TODO:
   * int verified = verify(&ast);
   * if (!verified) {
   *   return 1;
   * }
   * printf("VERIFICATION FINISHED\n");
   */

  char* assembly_code = generator(&ast);
  if (assembly_code == 0) {
    log_msg(ERROR, "error generating code\n", -1);
    return 1;
  }
  log_msg(INFO, "code generation finished\n\n", -1);
  printf("%s\n", assembly_code);

  save(argc, argv, assembly_code);

  free(source_code);
  free_tokens(tokens);
  // TODO: free_ast(ast);
  free(assembly_code);
}
