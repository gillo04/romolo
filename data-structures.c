#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data-structures.h"

void set_string(String* dest, char* src) {
  if (dest->str != 0) {
    free(dest->str);
  }

  dest->len = strlen(src);
  dest->cap = dest->len + 1;
  dest->str = (char*) malloc(dest->cap);

  strcpy(dest->str, src);
}

void append_char(String* dest, char c) {
  if (dest->cap <= dest->len+1) {
    dest->str = realloc(dest->str, dest->cap + STRING_RESIZE_INCREMENT);
    dest->cap += STRING_RESIZE_INCREMENT;
  }

  dest->len++;
  dest->str[dest->len-1] = c;
  dest->str[dest->len] = 0;

}

void append_string(String* dest, char* src) {
  int src_len = strlen(src);
  if (dest->cap <= dest->len + src_len) {
    dest->str = realloc(dest->str, dest->len + src_len + 1);
    dest->cap = dest->len + src_len + 1;
  }

  strcpy(dest->str + dest->len, src);
  dest->len += src_len;
}

void append_int(String* dest, long long num) {
  char num_str[12];
  sprintf(num_str, "%lld", num);
  char num_len = strlen(num_str);
  if (dest->cap <= dest->len + num_len) {
    dest->str = realloc(dest->str, dest->len + num_len + 1);
    dest->cap = dest->len + num_len + 1;
  }

  strcpy(dest->str + dest->len, num_str);
  dest->len += num_len;
}
