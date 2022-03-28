#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <stdio.h>

#define VA_HEAD(...) VA_HEAD0(__VA_ARGS__, 0)
#define VA_HEAD0(_0, ...) _0
#define VA_TAIL(...) VA_TAIL0(__VA_ARGS__, 0)
#define VA_TAIL0(_0, ...) __VA_ARGS__

#define report(line, ...)                                                      \
  printf("[line %d] Error: " VA_HEAD(__VA_ARGS__) "%.0d\n", line,              \
         VA_TAIL(__VA_ARGS__));

#endif /* ERROR_H */
