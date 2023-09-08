#include <stdio.h>

#include "io.h"

int main(int argc, char* argv[]) {
  char* source_code = load(argc, argv);
  if (source_code == 0) {
    return 1;
  }

  printf("%s", source_code);
}
