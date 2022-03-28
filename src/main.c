#include "error.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool had_error = false;

int run(size_t len, char buffer[len]) {
  Scanner scanner = {0};
  scanner_init(&scanner, len, buffer);
  scan_tokens(&scanner);

  size_t i = 0;
  while (scanner.tokens[i].type != TOK_EOF) {
    print_token(scanner.tokens[i]);
    i++;
  }
  return 0;
}

int run_file(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    printf("Error opening %s\n", filename);
    return 1;
  }

  char *buffer = 0;
  size_t len = 0;
  ssize_t bytes_read = getdelim(&buffer, &len, '\0', fp);
  if (bytes_read < 0) {
    printf("Error reading %s\n", filename);
    return 1;
  }
  fclose(fp);

  run(bytes_read, buffer);

  return 0;
}

int run_prompt() {
  char buf[256] = {0};
  int n_matched = 0;

  while (true) {
    printf("> ");
    n_matched = scanf("%s", buf);

    if (n_matched == EOF) {
      printf("\nBye!\n");
      return 0;
    }
    if (n_matched < 1) {
      report(1, "Error reading input");
      return 1;
    }

    run(strlen(buf), buf);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    printf("Usage: %s [input file]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }
  return EXIT_SUCCESS;
}
