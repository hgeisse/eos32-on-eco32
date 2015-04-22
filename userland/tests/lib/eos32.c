/*
 * eos32.c -- system call tests
 */


#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


void test__exit(void) {
  _exit(0x42);
}


void test_fork(void) {
  int pid;
  int res;

  pid = fork();
  if (pid < 0) {
    _exit(1);
  }
  if (pid == 0) {
    _exit(0x33);
  } else {
    if (wait(&res) != pid) {
      _exit(2);
    }
    if (res != 0x3300) {
      _exit(3);
    }
    _exit(0x42);
  }
}


void test_execve(void) {
  char *name = "/tst/eos32x";
  char *argv[2];

  argv[0] = name;
  argv[1] = NULL;
  execve(name, argv, NULL);
}


void test_wait(void) {
  int pid;
  int res;

  pid = fork();
  if (pid < 0) {
    _exit(1);
  }
  if (pid == 0) {
    _exit(0x33);
  } else {
    if (wait(&res) != pid) {
      _exit(2);
    }
    if (res != 0x3300) {
      _exit(3);
    }
    _exit(0x42);
  }
}


void test_brk(void) {
  char *last;

  last = (char *) curbrk;
  if (brk((void *) (last + 0x2000)) < 0) {
    _exit(1);
  }
  * (last + 0x1234) = 't';
  _exit(0x42);
}


void test_getpid(void) {
  _exit(getpid() == 11 ? 0x42 : 0x01);
}


void test_getppid(void) {
  _exit(getppid() == 3 ? 0x42 : 0x01);
}


struct test {
  char *name;
  void (*func)(void);
  int res;
} tests[] = {
  { "_exit",   test__exit,   0x4200 },
  { "fork",    test_fork,    0x4200 },
  { "execve",  test_execve,  0x0000 },
  { "wait",    test_wait,    0x4200 },
  { "brk",     test_brk,     0x4200 },
  { "getpid",  test_getpid,  0x4200 },
  { "getppid", test_getppid, 0x4200 },
};


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("\nError: ");
  vprintf(fmt, ap);
  va_end(ap);
  exit(1);
}


void runTest(struct test *tp) {
  int pid;
  int res;

  printf("running '%s': ", tp->name);
  pid = fork();
  if (pid < 0) {
    error("cannot fork test process");
  }
  if (pid == 0) {
    /* child */
    (*tp->func)();
    _exit(255);
  } else {
    /* parent */
    if (wait(&res) != pid) {
      error("test process has different pid");
    }
    if (res == tp->res) {
      printf("ok\n");
    } else {
      printf("failed (res = 0x%08X, ref = 0x%08X)\n", res, tp->res);
    }
  }
}


void test(char *k, char *s) {
  char key[10];
  char salt[2];
  char *res;
  int i;

  memset(key, 0, 10);
  strncpy(key, k, 8);
  salt[0] = s[0];
  salt[1] = s[1];
  res = crypt(key, salt);
  for (i = 0; i < 13; i++) {
    printf("%c", res[i]);
  }
  printf("\n");
}


int main(void) {
  int i;

  test("rootpass", "GH");
  test("hugo", "53");
  test("geheim", "zw");
  test("root", "Vw");
  exit(99);
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    runTest(&tests[i]);
  }
  return 0;
}
