#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  TOK_INVALID,
  TOK_IDENTIFIER,
  TOK_BOOLEAN,
  TOK_INTEGER,
  TOK_DOUBLE,
  TOK_CHARACTER,
  TOK_STRING,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_QUOTE,
  TOK_BACKQUOTE,
  TOK_COMMA,
  TOK_DOT,
  TOK_EOF,
} TokenType;

typedef struct {
  TokenType type;
  char *lexeme;
  union {
    long i;
    double d;
    char *s;
  } value;
  int line;
} Token;

typedef struct {
  size_t source_len;
  char *source;
  size_t tokens_count;
  Token *tokens;
  size_t current_token;
  size_t start_pos;
  size_t current_pos;
  int line;
} Scanner;

void print_token(Token token);
void scanner_init(Scanner *scanner, size_t source_len, char source[source_len]);
void scan_tokens(Scanner *scanner);

Token* add_token(Scanner *scanner, TokenType type);
bool is_at_end(Scanner *scanner);
char advance(Scanner *scanner);
bool match(Scanner *scanner, char expected);
char peek(Scanner *scanner);
char peek_next(Scanner *scanner);
void scan_token(Scanner *scanner);
void scan_string(Scanner *scanner);
void scan_number(Scanner *scanner);
void scan_identifier(Scanner *scanner);

#endif /* SCANNER_H */
