#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum type {
  TK_NAME,
  TK_NUMBER,
  TK_STRING,
  TK_OPEN_PARENTHESES,
  TK_CLOSE_PARENTHESES,
};

enum subtype {
  TK_INTEGER,
  TK_FLOAT,
  TK_FUNCTION,
  TK_RETURN,
};

typedef struct {
  size_t cursor;
  size_t length;
  const char *input;
} parser_t;

typedef struct {
  enum type type;
  enum subtype subtype;
  size_t offset;
  size_t length;
} token_t;

#define inrange(a, b, c) (a >= b && a <= c)
#define IS_WORD(chr) ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || chr == '_')
#define IS_DIGIT(chr) (chr >= '0' && chr <= '9')
#define IS_WS(chr) (chr == ' ' || chr == '\f' || chr == '\n' || chr == '\r' || chr == '\t' || chr == '\v' || chr == '\xA0')

#define TEST_NAME(chr) (IS_WORD(chr) || IS_DIGIT(chr))

#define STR_ASYNC "async"
#define STR_DO "do"
#define STR_END "end"
#define STR_FOR "for"
#define STR_FUNCTION "function"
#define STR_LOCAL "local"
#define STR_REPEAT "repeat"
#define STR_RETURN "return"
#define STR_UNTIL "until"
#define STR_WHILE "while"
#define STR_YIELD "yield"

inline const char *copy(const char *str, size_t begin, size_t len) {
  char *copy = malloc(len); 
  strncpy(copy, &str[begin], len);
  return copy;
}

token_t *newtoken(enum type type, enum subtype subtype, size_t offset, size_t length) {
  token_t *token = malloc(sizeof(token_t));
  token->type = type;
  token->subtype = subtype;
  token->offset = offset;
  token->length = length;
  return token;
}

token_t *parse_name(parser_t *parser) {
  size_t begin = parser->cursor;
  size_t current = parser->cursor;

  while (TEST_NAME(parser->input[current]))
    current++;

  size_t length = current - begin;
  enum subtype subtype = 0;

  const char *name = copy(parser->input, begin, length);
  if (length == 8 && strcmp(name, STR_FUNCTION) == 0)
    subtype = TK_FUNCTION;
  else if (length == 6 && strcmp(name, STR_RETURN) == 0)
    subtype = TK_RETURN;

  free((void *) name);
  return newtoken(TK_NAME, subtype, begin, length);
}

token_t *parse_number(parser_t *parser) {
  size_t begin = parser->cursor,
        current = parser->cursor;
  
  int stage = 0; // 0.1E2
  int isFloat = 1;

  integer: {
    while (IS_DIGIT(parser->input[current])) current++;
    if (parser->input[current] == '.')
      goto decimal;
    else if (parser->input[current] == 'e' || parser->input[current] == 'E')
      goto exponetial;
    else {
      isFloat = 0;
      goto end;
    }
  }

  decimal: {
    current++;
    while (IS_DIGIT(parser->input[current])) current++;
    if (parser->input[current] == 'e' || parser->input[current] == 'E')
      goto exponetial;
    else goto end;
  }
  
  exponetial: {
    current++;
    if (parser->input[current] == '+' || parser->input[current] == '-') current++;
    size_t count;
    while (IS_DIGIT(parser->input[current])) count++;
    if (count == 0); // ERROR
    current += count;
  }

  end:;

  return newtoken(TK_NUMBER, isFloat ? TK_FLOAT : TK_INTEGER, begin, current - begin);
}

int main() {
  const char code[] = "function a() {\nreturn 20e}";
  parser_t parser = {
    .cursor = 0,
    .length = strlen(code),
    .input = code
  };

  while (parser.cursor < parser.length) {
    const char chr = parser.input[parser.cursor];
    token_t *token = NULL;
    if (IS_WORD(chr)) {
      token = parse_name(&parser);
    } else if (IS_DIGIT(chr)) {
      token = parse_number(&parser);
    } else if (IS_WS(chr)) {
      parser.cursor++;
    } else {
      parser.cursor++;
    }

    if (token) {
      parser.cursor += token->length;
      const char *str = copy(code, token->offset, token->length);
      printf("[%i;%i]{%i}: %s\n", token->type, token->subtype, token->length, str);
      free((void *) str);
    }
  }

  return 0;
}