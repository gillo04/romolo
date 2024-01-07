#pragma once
#include <stdio.h>
#include <stdlib.h>
#define VECTOR_RESIZE_INCREMENT 50
#define STRING_RESIZE_INCREMENT 5

// Vector data structure

#define VECTOR(T)                               \
  typedef struct {                              \
    int size;                                   \
    int end;                                    \
    T* vec;                                     \
  } Vector_##T;                                 \
                                                \
  void init_vector_##T(Vector_##T* v) {         \
    v->size = VECTOR_RESIZE_INCREMENT;          \
    v->end = 0;                                 \
    v->vec = malloc(VECTOR_RESIZE_INCREMENT     \
                    * sizeof(T));               \
  }                                             \
                                                \
  void push_##T(Vector_##T* v, T val) {         \
    if (v->end == v->size) {                    \
      /* Resize vector */                       \
      v->size += VECTOR_RESIZE_INCREMENT;       \
      v->vec = realloc(v->vec, v->size          \
                      * sizeof(T));             \
    }                                           \
    /* Push value */                            \
    v->vec[v->end] = val;                       \
    v->end++;                                   \
  }                                             \
                                                \
  T pop_##T(Vector_##T* v) {                    \
    if (v->end == 0) {                          \
      return (T) {0, 0};                        \
    } else {                                    \
      v->end--;                                 \
      return v->vec[v->end];                    \
    }                                           \
  }                                             \

typedef struct {
  char* str;
  int len;
  int cap;
} String;

// Set the contents of a String
void set_string(String* dest, char* src);

// Append a char to a String
void append_char(String* dest, char c);

// Append c string to a String
void append_string(String* dest, char* src);

// Append long long int to a String
void append_int(String* dest, long long num);
