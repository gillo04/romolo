#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "log.h"
#include "lexer.h"
#include "data-structures.h"

int main(int argc, char* argv[]) {
  char* source_code = load(argc, argv);
  if (source_code == 0) {
    return 1;
  }

  Token* tokens = lexer(source_code);
  if (tokens == 0) {
    return 1;
  }
  //print_tokens(tokens);

  free(source_code);
  free_tokens(tokens);
}
