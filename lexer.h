#pragma once
enum {
  T_NONE = 0,
  T_KEYWORD,
  T_IDENTIFIER,
  T_CONSTANT,
  T_STRING_LITERAL,
  T_PUNCTUATOR
};

typedef struct {
  int type;
  union {
    char* str;
    long long num;
  } val;
  int line;
} Token;

// Returns a null terminated token array
Token* lexer(char* source);

// Frees all tokens in a null terminated token array
void free_tokens(Token* tokens);
