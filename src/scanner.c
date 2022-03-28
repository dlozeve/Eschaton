#include "scanner.h"
#include "error.h"
#include <ctype.h>
#include <string.h>

void print_token(Token token) {
  printf("[line %d] ", token.line);
  switch (token.type) {
  case TOK_IDENTIFIER:
    printf("<IDENTIFIER>\t%s", token.value.s);
    break;
  case TOK_BOOLEAN:
    printf("<BOOLEAN>\t%d", (bool)token.value.i);
    break;
  case TOK_INTEGER:
    printf("<INTEGER>\t%ld", token.value.i);
    break;
  case TOK_DOUBLE:
    printf("<DOUBLE>\t%f", token.value.d);
    break;
  case TOK_CHARACTER:
    printf("<CHARACTER>\t%c", token.value.s[0]);
    break;
  case TOK_STRING:
    printf("<STRING>\t\"%s\"", token.value.s);
    break;
  case TOK_LPAREN:
    printf("<LPAREN>\t%s", token.lexeme);
    break;
  case TOK_RPAREN:
    printf("<RPAREN>\t%s", token.lexeme);
    break;
  case TOK_QUOTE:
    printf("<QUOTE>\t%s", token.lexeme);
    break;
  case TOK_BACKQUOTE:
    printf("<BACKQUOTE>\t%s", token.lexeme);
    break;
  case TOK_COMMA:
    printf("<COMMA>\t%s", token.lexeme);
    break;
  case TOK_DOT:
    printf("<DOT>\t\t%s", token.lexeme);
    break;
  case TOK_EOF:
    printf("<EOF>");
    break;
  default:
    printf("Invalid token");
  }
  printf("\n");
}

void scanner_init(Scanner *scanner, size_t source_len,
                  char source[source_len]) {
  scanner->source_len = source_len;
  scanner->source = source;
  scanner->tokens_count = source_len;
  scanner->tokens = calloc(source_len, sizeof(Token));
  scanner->current_token = 0;
  scanner->start_pos = 0;
  scanner->current_pos = 0;
  scanner->line = 1;
}

Token *add_token(Scanner *scanner, TokenType type) {
  char *lexeme =
      calloc(scanner->current_pos - scanner->start_pos + 1, sizeof(char));
  strncpy(lexeme, &scanner->source[scanner->start_pos],
          scanner->current_pos - scanner->start_pos);
  scanner->tokens[scanner->current_token].type = type;
  scanner->tokens[scanner->current_token].lexeme = lexeme;
  scanner->tokens[scanner->current_token].value.s = 0;
  scanner->tokens[scanner->current_token].line = scanner->line;
  scanner->current_token++;
  return &scanner->tokens[scanner->current_token - 1];
}

bool is_at_end(Scanner *scanner) {
  return scanner->current_pos >= scanner->source_len;
}

void scan_tokens(Scanner *scanner) {
  while (!is_at_end(scanner)) {
    scanner->start_pos = scanner->current_pos;
    scan_token(scanner);
  }

  add_token(scanner, TOK_EOF);
}

char advance(Scanner *scanner) {
  return scanner->source[scanner->current_pos++];
}

bool match(Scanner *scanner, char expected) {
  if (is_at_end(scanner)) {
    return false;
  }
  if (scanner->source[scanner->current_pos] != expected) {
    return false;
  }
  scanner->current_pos++;
  return true;
}

char peek(Scanner *scanner) {
  if (is_at_end(scanner)) {
    return '\0';
  }
  return scanner->source[scanner->current_pos];
}

char peek_next(Scanner *scanner) {
  if (scanner->current_pos + 1 >= scanner->source_len) {
    return '\0';
  }
  return scanner->source[scanner->current_pos + 1];
}

void scan_token(Scanner *scanner) {
  char c = advance(scanner);

  switch (c) {
  case '(':
    add_token(scanner, TOK_LPAREN);
    break;
  case ')':
    add_token(scanner, TOK_RPAREN);
    break;
  case '\'':
    add_token(scanner, TOK_QUOTE);
    break;
  case '`':
    add_token(scanner, TOK_BACKQUOTE);
    break;
  case ',':
    add_token(scanner, TOK_COMMA);
    break;
  case '.':
    add_token(scanner, TOK_DOT);
    break;
  case '#':
    if (match(scanner, '\\')) {
      Token *token = add_token(scanner, TOK_CHARACTER);
      char *chr = calloc(1, sizeof(char));
      chr[0] = advance(scanner);
      token->value.s = chr;
    } else if (match(scanner, 'f')) {
      Token *token = add_token(scanner, TOK_BOOLEAN);
      token->value.i = false;
    } else if (match(scanner, 't')) {
      Token *token = add_token(scanner, TOK_BOOLEAN);
      token->value.i = true;
    } else {
      report(scanner->line, "Unexpected #");
    }
    break;
  case ';':
    while ((peek(scanner) != '\n') && !is_at_end(scanner)) {
      advance(scanner);
    }
    break;
  case '"':
    scan_string(scanner);
    break;
  case ' ':
  case '\t':
  case '\r':
    break;
  case '\n':
    scanner->line++;
    break;
  case '+':
    if (isspace(peek(scanner)) || is_at_end(scanner)) {
      scan_identifier(scanner);
    } else if (isdigit(peek(scanner))) {
      scan_number(scanner);
    } else if (isalpha(peek(scanner)) || (strchr("!$%&*/:<>?@^_~", peek(scanner)) != NULL)) {
      scan_identifier(scanner);
    } else {
      report(scanner->line, "Unexpected character: %c", peek(scanner));
    }
    break;
  case '-':
    if (isspace(peek(scanner)) || is_at_end(scanner)) {
      scan_identifier(scanner);
    } else if (isdigit(peek(scanner))) {
      scan_number(scanner);
    } else if (isalpha(peek(scanner)) || (strchr("!$%&*/:<>?@^_~", peek(scanner)) != NULL)) {
      scan_identifier(scanner);
    } else {
      report(scanner->line, "Unexpected character: %c", peek(scanner));
    }
    break;
  default:
    if (isdigit(c)) {
      scan_number(scanner);
    } else if (isalpha(c) || (strchr("!$%&*/:<>?@^_~", c) != NULL)) {
      scan_identifier(scanner);
    } else {
      report(scanner->line, "Unexpected character: %c", c);
    }
    break;
  }
}

void scan_string(Scanner *scanner) {
  // TODO support escapes
  while ((peek(scanner) != '"') && !is_at_end(scanner)) {
    if (peek(scanner) == '\n')
      scanner->line++;
    advance(scanner);
  }
  if (is_at_end(scanner)) {
    report(scanner->line, "Unterminated string.");
    return;
  }
  advance(scanner); // the closing quote
  Token *token = add_token(scanner, TOK_STRING);
  char *value = calloc(scanner->current_pos - scanner->start_pos, sizeof(char));
  strncpy(value, &scanner->source[scanner->start_pos + 1],
          scanner->current_pos - scanner->start_pos - 2);
  token->value.s = value;
}

void scan_number(Scanner *scanner) {
  // TODO support scientific notation, complex numbers, binary, octal,
  // and hex numbers, and things like inf and nan
  while (isdigit(peek(scanner))) {
    advance(scanner);
  }

  if ((peek(scanner) == '.') && isdigit(peek_next(scanner))) {
    advance(scanner);
    while (isdigit(peek(scanner))) {
      advance(scanner);
    }
    Token *token = add_token(scanner, TOK_DOUBLE);
    token->value.d = atof(&scanner->source[scanner->start_pos]);
  } else {
    Token *token = add_token(scanner, TOK_INTEGER);
    token->value.i = atol(&scanner->source[scanner->start_pos]);
  }
}

void scan_identifier(Scanner *scanner) {
  char c = peek(scanner);
  while (!is_at_end(scanner) && (isalpha(c) || isdigit(c) || strchr("!$%&*/:<>?@^_~+-.@", c) != NULL)) {
    advance(scanner);
    c = peek(scanner);
  }
  Token *token = add_token(scanner, TOK_IDENTIFIER);
  char *value = calloc(scanner->current_pos - scanner->start_pos, sizeof(char));
  strncpy(value, &scanner->source[scanner->start_pos],
          scanner->current_pos - scanner->start_pos);
  token->value.s = value;
}
