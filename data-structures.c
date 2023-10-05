#include <stdio.h>
#include <string.h>
#include "data-structures.h"

void set_string(String* dest, char* src) {
  if (dest->str != 0) {
    free(dest->str);
  }

  dest->len = strlen(src);
  dest->cap = dest->len+1;
  dest->str = (char*) malloc(dest->len);

  strcpy(dest->str, src);
}

void append_char(String* dest, char c) {
  if (dest->cap <= dest->len+1) {
    dest->str = realloc(dest->str, STRING_RESIZE_INCREMENT);
    dest->cap += STRING_RESIZE_INCREMENT;
  }

  dest->len++;
  dest->str[dest->len-1] = c;
  dest->str[dest->len] = 0;

}
