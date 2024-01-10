#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* load(int argc, char* argv[]) {
  FILE* ptr;

  if (argc < 2) {
    printf("Loader error:\tNo file inputted\n");
    return 0;
  } else {
    ptr = fopen(argv[1], "r");
  }

  if (NULL == ptr) {
    printf("Loader error:\tFile can't be opened \n");
    return 0;
  }

  // Get fle size
  long size;
  fseek(ptr, 0, SEEK_END);
  size = ftell(ptr);
  fseek(ptr, 0, SEEK_SET);

  // Read file
  int src_i = 0;
  char* source = (char*) malloc(size + 1);
  do {
    source[src_i++] = fgetc(ptr);
  } while (source[src_i - 1] != EOF);
  source[--src_i] = 0;

  fclose(ptr);

  return source;
}

void save(int argc, char* argv[], char* src) {
  FILE *file;

  char* filename;
  if (argc >= 3) {
    filename = argv[2];
  } else {
    filename = "out.s";
  }

  // Open the file in write mode
  file = fopen(filename, "w");
  
  if (file == NULL) {
      printf("Loader error:\tFailed to open the file.\n");
      return;
  }
  
  fprintf(file, "%s\n", src);
  
  // Close the file
  fclose(file);
}
