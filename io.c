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

  // Extract filename
  char filename[100];
  int j = 0;
  int captuting = 0;
  for (int i = strlen(argv[1]) - 1; i >= 0; i--) {
    if (argv[1][i] == '.') {
      captuting = 1;
    } else if (argv[1][i] == '/') {
      filename[j] = 0;
      break;
    } else if (captuting){
      filename[j++] = argv[1][i];
    }
  }
  int len = strlen(filename);
  for (int i = 0; i < len/2; i++) {
    char tmp = filename[i];
    filename[i] = filename[len-i-1];
    filename[len-i-1] = tmp;
  }
  filename[len++] = '.';
  filename[len++] = 's';
  filename[len] = 0;
  
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
