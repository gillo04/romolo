#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "data-structures.h"

Token lex_from_array(char* source, int* i, const char** array, int array_size) {
  Token out = {-1};

  for (int j = 0; j < array_size; j++) {
    int len = strlen(array[j]);
    if (memcmp(source + *i, array[j], len) == 0) {
      out.val.str = malloc(strlen(array[j]));
      strcpy(out.val.str, array[j]);
      *i += len;
      return out;
    }
  }
  
  return (Token) {T_NONE};
}

const char* keywords[] = {
  "auto", "break", "case", "char", "const", "continue", "default", "double", "do", "else", "enum",
  "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict",
  "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
  "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", 
  "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
};

Token lex_keyword(char* source, int* i) {
  int j = *i;
  Token out = lex_from_array(source, i, keywords, sizeof(keywords)/sizeof(char*));
  if (out.type != T_NONE) {
    out.type = T_KEYWORD;
  }

  if (isalnum(*(source + *i)) || *(source + *i) == '_') {
    *i = j;
    free(out.val.str);
    return (Token) {T_NONE};
  }

  return out;
}

const char* punctuators[] = {
  "[", "]", "(", ")", "{", "}", "->",
  "++", "--",
  "==", "!=", "&&", "||", "...",
  "*=", "/=", "%=", "+=", "-=", "<<=",
  ">>=", "&=", "^=", "|=", "<=", ">=",
  "##", ",", "#", "<<", ">>",
  "<:", ":>", "<%", "%>", "%:%:", "%:", 
  "&", "*", "+", "-", "~", ".", 
  "=", "?", ":", ";", "^", "|",
  "/", "%", "<", ">", "!"
};
Token lex_punctuator(char* source, int* i) {
  Token out = lex_from_array(source, i, punctuators, sizeof(punctuators)/sizeof(char*));
  if (out.type != T_NONE) {
    out.type = T_PUNCTUATOR;
  }

  if (strcmp(out.val.str, "<:") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "[");
  } else if (strcmp(out.val.str, ":>") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "]");
  } else if (strcmp(out.val.str, "<%") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "{");
  } else if (strcmp(out.val.str, "%>") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "}");
  } else if (strcmp(out.val.str, "%:") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "#");
  } else if (strcmp(out.val.str, "%:%:") == 0) {
    free(out.val.str);
    strcpy(out.val.str, "##");
  }

  return out;
}


Token lex_identifier(char* source, int* i) {
  int j = *i;
  if (isalpha(source[j]) || source[j] == '_') {
    j++;
    Token out = {T_IDENTIFIER};

    while (source[j] != 0 && (isalnum(source[j]) || source[j] == '_')) {
      j++;
    }

    out.val.str = (char*) malloc(j - *i + 1);
    memcpy(out.val.str, source + *i, j - *i);
    out.val.str[j - *i] = 0;
    *i = j;

    return out;
  } else {
    return (Token) {T_NONE};
  }
}

Token lex_integer(char* source, int* i) {
  // TODO: implement int type notation
  int j = *i;
  Token out = {T_CONSTANT};

  if (isdigit(source[j])) {
    if (source[j] != '0') {
      // Lex decimal integer
      while (isdigit(source[j])) {
        j++;
      }

      char* tmp = (char*) malloc(j - *i + 1);
      memcpy(tmp, source + *i, j - *i);
      tmp[j - *i] = 0;
      out.val.num = strtol(tmp, 0, 10);
      free(tmp);
    } else if (tolower(source[j+1]) == 'x') {
      // Lex hexadecimal integer
      j += 2;
      while (isxdigit(source[j])) {
        j++;
      }

      char* tmp = (char*) malloc(j - *i + 1);
      memcpy(tmp, source + *i, j - *i);
      tmp[j - *i] = 0;
      out.val.num = strtol(tmp, 0, 16);
      free(tmp);
    } else {
      // Lex octal integer
      while (source[j] >= '0' && source[j] <= '7') {
        j++;
      }

      char* tmp = (char*) malloc(j - *i + 1);
      memcpy(tmp, source + *i, j - *i);
      tmp[j - *i] = 0;
      out.val.num = strtol(tmp, 0, 8);
      free(tmp);
    }
    *i = j;
    return out;
  } else {
    return (Token) {T_NONE};
  }
}



long long lex_character(char* source, int* i, char delimiter) {
  int j = *i;
  long long out = 0;
  if (source[j] == '\\') {
    j++;
    if (source[j] >= '0' && source[j] <= '7') {
      // Octal escape sequence
      int start = j;
      j++;
      while (source[j] >= '0' && source[j] <= '7' && j-start < 3) {
        j++;
      }
      char* tmp = (char*) malloc(j - start + 1);
      memcpy(tmp, source + start, j - start);
      tmp[j - start] = 0;
      out = strtol(tmp, 0, 8);
    } else if (source[j] == 'x') {
      // Hex escape sequence
      j++;
      int start = j;
      while (isxdigit(source[j])) {
        j++;
      }
      char* tmp = (char*) malloc(j - start + 1);
      memcpy(tmp, source + start, j - start);
      tmp[j - start] = 0;
      out = strtol(tmp, 0, 16);
    } else {
      // Simple escape sequence
      switch (source[j]) {
        case '\'':
        case '"':
        case '\\':
        case '?':
          out = source[j];
          break;
        case 'a':
          out = 0x07;
          break;
        case 'b':
          out = 0x08;
          break;
        case 'f':
          out = 0x0C;
          break;
        case 'n':
          out = 0x0A;
          break;
        case 'r':
          out = 0x0D;
          break;
        case 't':
          out = 0x09;
          break;
        case 'v':
          out = 0x0B;
          break;
         default:
          return 0;
      }
      j++;
    }
  } else if (source[j] != delimiter && source[j] !='\n') {
    out = source[j];
    j++;
  } 
  *i = j;
  return out;
}

Token lex_character_constant(char* source, int* i) {
  int j = *i;
  if (source[j] == '\'') {
    j++;
    Token out = {T_CONSTANT};
    out.val.num = lex_character(source, &j, '\'');
    if (out.val.num == 0) {
      return (Token) {T_NONE};
    }

    if (source[j] == '\'') {
      j++;
      *i = j;
      return out;
    }
  }

  return (Token) {T_NONE};
}

Token lex_string_literal(char* source, int* i) {
  // TODO: implement string type notation
  int j = *i;
  if (source[j] == '"') {
    j++;
    Token out = {T_STRING_LITERAL};

    String tmp = {0,0,0};
    set_string(&tmp, "");
    char tmp_c = (char) lex_character(source, &j, '"');
    while (tmp_c != 0){
      append_char(&tmp, tmp_c);
      tmp_c = lex_character(source, &j, '"');
    }

    if (source[j] == '"') {
      out.val.str = (char*) malloc(tmp.len+1);
      strcpy(out.val.str, tmp.str);
      free(tmp.str);

      j++;
      *i = j;
      return out;
    }
    free(tmp.str);
  }

  return (Token) {T_NONE};
}

VECTOR(Token);      // Define token stack
Token(*lexers[])(char*, int*) = {lex_keyword, lex_identifier, lex_integer, lex_character_constant, lex_string_literal, lex_punctuator};
Token* lexer(char* source) {
  Vector_Token tokens;
  init_vector_Token(&tokens);

  int i = 0;
  int line = 0;
  while (source[i] != 0) {
    if (source[i] == '\n') {
      line = i+1;
    }

    // Remove comments
    if (source[i] == '/' && source[i + 1] == '/') {
      i += 2;
      while (source[i] != '\n' && source[i] != 0) {
        i++;
      }
      line = i+1;
    }
    if (source[i] == '/' && source[i + 1] == '*') {
      i += 2;
      while (!(source[i-1] == '*' && source[i] == '/') && source[i] != 0) {
        i++;
      }
      i++;
      line = i+1;
    }

    if (isspace(source[i])) {
      i++;
    } else {
      Token tmp;
      int matched = 0;
      for (int j = 0; j < sizeof(lexers) / sizeof(void*); j++) {
        tmp = (*lexers[j])(source, &i);
        tmp.line = line;
        if (tmp.type != T_NONE) {
          push_Token(&tokens, tmp);
          matched = 1;
          break;
        }
      }
      
      if (matched) {
        continue;
      }

      printf("Lexer error at %s:\n\tUrecognized token %s\n", __func__, &source[i]);
      return 0;
    } 
  }

  push_Token(&tokens, (Token) {T_NONE});
  return tokens.vec;
}

void free_tokens(Token* tokens) {
  int i = 0; 
  while (tokens[i].type != 0) {
    if (tokens[i].type != T_CONSTANT) {
      free(tokens[i].val.str);
    }
    i++;
  }
  free(tokens);
}
