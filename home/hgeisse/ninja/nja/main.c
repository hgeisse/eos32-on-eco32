/*
 * main.c -- Ninja Assembler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "scanner.h"
#include "parser.h"
#include "coder.h"
#include "table.h"


static void version(char *myself) {
  /* show version and compilation date */
  printf("%s version %d (compiled %s, %s)\n",
         myself, VERSION, __DATE__, __TIME__);
}


static void help(char *myself) {
  /* show some help how to use the program */
  printf("Usage: %s [options] <input file> <output file>\n", myself);
  printf("Options:\n");
  printf("  --map              show a map of symbols\n");
  printf("  --version          show version and exit\n");
  printf("  --help             show this help and exit\n");
}


int main(int argc, char *argv[]) {
  int i;
  char *inFileName;
  char *outFileName;
  boolean map;

  /* analyze command line */
  inFileName = NULL;
  outFileName = NULL;
  map = FALSE;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      /* option */
      if (strcmp(argv[i], "--map") == 0) {
        map = TRUE;
      } else
      if (strcmp(argv[i], "--version") == 0) {
        version(argv[0]);
        exit(0);
      } else
      if (strcmp(argv[i], "--help") == 0) {
        help(argv[0]);
        exit(0);
      } else {
        error("unrecognized option '%s'; try '%s --help'",
              argv[i], argv[0]);
      }
    } else {
      /* file */
      if (outFileName != NULL) {
        error("more than two file names not allowed");
      }
      if (inFileName != NULL) {
        outFileName = argv[i];
      } else {
        inFileName = argv[i];
      }
    }
  }
  if (inFileName == NULL) {
    error("no input file");
  }
  if (outFileName == NULL) {
    error("no output file");
  }
  /* process input */
  yyin = fopen(inFileName, "r");
  if (yyin == NULL) {
    error("cannot open input file '%s'", inFileName);
  }
  initCoder();
  yyparse();
  checkUndefined(map);
  exitCoder(outFileName);
  return 0;
}
