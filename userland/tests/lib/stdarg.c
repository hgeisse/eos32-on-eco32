/*
 * stdarg.c -- variable argument list tests
 */


#include <stdio.h>
#include <stdarg.h>


typedef struct {
  char c;
} MiniStruct;


int testVarArgs(char *fmt, ...) {
  int count;
  va_list ap;

  count = 0;
  va_start(ap, fmt);
  while (*fmt != '\0') {
    switch (*fmt) {
      case 'i':
        if (va_arg(ap, int) != ++count) {
          return -1;
        }
        break;
      case 'd':
        if (va_arg(ap, double) != ++count) {
          return -2;
        }
        break;
      case 'p':
        if (va_arg(ap, char *)[0] != ++count) {
          return -3;
        }
        break;
      case 's':
        if (va_arg(ap, MiniStruct).c != ++count) {
          return -4;
        }
        break;
      default:
        return -5;
    }
    fmt++;
  }
  va_end(ap);
  return count;
}


void showResult(char *text, int result, int shouldbe) {
  if (result == shouldbe) {
    printf("%s succeeded\n", text);
  } else {
    printf("%s failed, ", text);
    if (result >= 0) {
      printf("wrong number of arguments\n");
    } else {
      switch (result) {
        case -1:
          printf("wrong int argument\n");
          break;
        case -2:
          printf("wrong double argument\n");
          break;
        case -3:
          printf("wrong pointer argument\n");
          break;
        case -4:
          printf("wrong struct argument\n");
          break;
        case -5:
          printf("unknown format character\n");
          break;
        default:
          printf("unknown return value\n");
          break;
      }
    }
  }
}


int main(void) {
  MiniStruct ms = { 3 };
  int result;

  //result = testVarArgs("iisdi", '\1', 2, ms, 4.0, 5);
  result = testVarArgs("iisii", '\1', 2, ms, 4, 5);
  showResult("test 1", result, 5);
  result = testVarArgs("");
  showResult("test 2", result, 0);
  //result = testVarArgs("pdp", "\1", 2.0, "\3");
  result = testVarArgs("pip", "\1", 2, "\3");
  showResult("test 3", result, 3);
  return 0;
}
