/*
 * stdio.c -- input and output function tests
 */


#include <stdio.h>


void test_printf(void) {
  printf("1.  a constant string: 'this is a constant string'\n");
  printf("2.  a percent character: '%%'\n");
  printf("3.  a computed character: '%c'\n", '@');
  printf("4.  a computed string: '%s'\n", "this is a computed string");
  printf("5.  a decimal number: %d\n", 1234);
  printf("6.  an octal number: %o\n", 1234);
  printf("7.  an octal number (alternate form): %#o\n", 1234);
  printf("8.  a hex number: %x\n", 0x1BC4);
  printf("9.  a hex number (alternate form): %#x\n", 0x1BC4);
  printf("10. a hex number (upper case): %X\n", 0x1BC4);
  printf("11. a hex number (alternate form, upper case): %#X\n", 0x1BC4);
  printf("12. a pointer: %p\n", "this has type char *");
  printf("\n");

  printf("--------------------------------------------\n");

  printf("@@@%d@@@\n", 123);
  printf("@@@%d@@@\n", -123);

  printf("@@@%8d@@@\n", 123);
  printf("@@@%8d@@@\n", -123);

  printf("@@@%.5d@@@\n", 123);
  printf("@@@%.5d@@@\n", -123);

  printf("@@@%0d@@@\n", 123);
  printf("@@@%0d@@@\n", -123);

  printf("@@@%08d@@@\n", 123);
  printf("@@@%08d@@@\n", -123);

  //printf("@@@%0.5d@@@\n", 123);
  //printf("@@@%0.5d@@@\n", -123);

  //printf("@@@%08.5d@@@\n", 123);
  //printf("@@@%08.5d@@@\n", -123);

  printf("@@@%+d@@@\n", 123);
  printf("@@@%+d@@@\n", -123);

  printf("@@@%+8d@@@\n", 123);
  printf("@@@%+8d@@@\n", -123);

  printf("@@@%+.5d@@@\n", 123);
  printf("@@@%+.5d@@@\n", -123);

  printf("@@@%+8.5d@@@\n", 123);
  printf("@@@%+8.5d@@@\n", -123);

  printf("@@@% d@@@\n", 123);
  printf("@@@% d@@@\n", -123);

  printf("@@@% 8d@@@\n", 123);
  printf("@@@% 8d@@@\n", -123);

  printf("@@@% .5d@@@\n", 123);
  printf("@@@% .5d@@@\n", -123);

  printf("@@@% 8.5d@@@\n", 123);
  printf("@@@% 8.5d@@@\n", -123);

  printf("--------------------------------------------\n");

  printf("@@@%x@@@\n", 123);
  printf("@@@%#x@@@\n", 123);

  printf("@@@%8x@@@\n", 123);
  printf("@@@%#8x@@@\n", 123);

  printf("@@@%.5x@@@\n", 123);
  printf("@@@%#.5x@@@\n", 123);

  printf("@@@%0x@@@\n", 123);
  printf("@@@%#0x@@@\n", 123);

  printf("@@@%08x@@@\n", 123);
  printf("@@@%#08x@@@\n", 123);

  //printf("@@@%0.5x@@@\n", 123);
  //printf("@@@%#0.5x@@@\n", 123);

  //printf("@@@%08.5x@@@\n", 123);
  //printf("@@@%#08.5x@@@\n", 123);

  //printf("@@@%+x@@@\n", 123);
  //printf("@@@%#+x@@@\n", 123);

  //printf("@@@%+8x@@@\n", 123);
  //printf("@@@%#+8x@@@\n", 123);

  //printf("@@@%+.5x@@@\n", 123);
  //printf("@@@%#+.5x@@@\n", 123);

  //printf("@@@%+8.5x@@@\n", 123);
  //printf("@@@%#+8.5x@@@\n", 123);

  //printf("@@@% x@@@\n", 123);
  //printf("@@@%# x@@@\n", 123);

  //printf("@@@% 8x@@@\n", 123);
  //printf("@@@%# 8x@@@\n", 123);

  //printf("@@@% .5x@@@\n", 123);
  //printf("@@@%# .5x@@@\n", 123);

  //printf("@@@% 8.5x@@@\n", 123);
  //printf("@@@%# 8.5x@@@\n", 123);

  printf("--------------------------------------------\n");

  printf("@@@%o@@@\n", 123);
  printf("@@@%#o@@@\n", 123);

  printf("@@@%8o@@@\n", 123);
  printf("@@@%#8o@@@\n", 123);

  printf("@@@%.5o@@@\n", 123);
  printf("@@@%#.5o@@@\n", 123);

  printf("@@@%0o@@@\n", 123);
  printf("@@@%#0o@@@\n", 123);

  printf("@@@%08o@@@\n", 123);
  printf("@@@%#08o@@@\n", 123);

  //printf("@@@%0.5o@@@\n", 123);
  //printf("@@@%#0.5o@@@\n", 123);

  //printf("@@@%08.5o@@@\n", 123);
  //printf("@@@%#08.5o@@@\n", 123);

  //printf("@@@%+o@@@\n", 123);
  //printf("@@@%#+o@@@\n", 123);

  //printf("@@@%+8o@@@\n", 123);
  //printf("@@@%#+8o@@@\n", 123);

  //printf("@@@%+.5o@@@\n", 123);
  //printf("@@@%#+.5o@@@\n", 123);

  //printf("@@@%+8.5o@@@\n", 123);
  //printf("@@@%#+8.5o@@@\n", 123);

  //printf("@@@% o@@@\n", 123);
  //printf("@@@%# o@@@\n", 123);

  //printf("@@@% 8o@@@\n", 123);
  //printf("@@@%# 8o@@@\n", 123);

  //printf("@@@% .5o@@@\n", 123);
  //printf("@@@%# .5o@@@\n", 123);

  //printf("@@@% 8.5o@@@\n", 123);
  //printf("@@@%# 8.5o@@@\n", 123);
}


int main(void) {
  /* !!!!! */
  test_printf();
  return 0;
}
