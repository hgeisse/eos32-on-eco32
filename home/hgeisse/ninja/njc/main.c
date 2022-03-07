/*
 * main.c -- Ninja Compiler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "absyn.h"
#include "scanner.h"
#include "parser.h"
#include "types.h"
#include "table.h"
#include "semant.h"
#include "codegen.h"


static void version(char *myself) {
  /* show version and compilation date */
  printf("%s version %d (compiled %s, %s)\n",
         myself, VERSION, __DATE__, __TIME__);
}


static void help(char *myself) {
  /* show some help how to use the program */
  printf("Usage: %s [options] <input file>\n", myself);
  printf("Options:\n");
  printf("  --output <file>    specify output file\n");
  printf("  --tokens           show stream of tokens (no parsing)\n");
  printf("  --absyn            show abstract syntax\n");
  printf("  --tables           show symbol tables\n");
  printf("  --version          show version and exit\n");
  printf("  --help             show this help and exit\n");
}


int main(int argc, char *argv[]) {
  int i;
  char *inFileName;
  char *outFileName;
  boolean optionTokens;
  boolean optionAbsyn;
  boolean optionTables;
  int token;
  Table *globalTable;
  FILE *outFile;

  /* analyze command line */
  inFileName = NULL;
  outFileName = NULL;
  optionTokens = FALSE;
  optionAbsyn = FALSE;
  optionTables = FALSE;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      /* option */
      if (strcmp(argv[i], "--output") == 0) {
        if (++i == argc) {
          error("output file name missing");
        }
        outFileName = argv[i];
      } else
      if (strcmp(argv[i], "--tokens") == 0) {
        optionTokens = TRUE;
      } else
      if (strcmp(argv[i], "--absyn") == 0) {
        optionAbsyn = TRUE;
      } else
      if (strcmp(argv[i], "--tables") == 0) {
        optionTables = TRUE;
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
      if (inFileName != NULL) {
        error("more than one input file");
      }
      inFileName = argv[i];
    }
  }
  if (inFileName == NULL) {
    error("no input file");
  }
  /* scan & parse source */
  yyin = fopen(inFileName, "r");
  if (yyin == NULL) {
    error("cannot open input file '%s'", inFileName);
  }
  if (optionTokens) {
    do {
      token = yylex();
      showToken(token);
    } while (token != 0);
    fclose(yyin);
    exit(0);
  }
  yyparse();
  fclose(yyin);
  if (optionAbsyn) {
    showAbsyn(progTree);
  }
  /* do semantic amalysis */
  globalTable = check(progTree, optionTables);
  /* generate code */
  if (outFileName != NULL) {
    outFile = fopen(outFileName, "w");
    if (outFile == NULL) {
      error("cannot open output file '%s'", outFileName);
    }
  } else {
    outFile = stdout;
  }
  genCode(progTree, globalTable, outFile);
  if (outFileName != NULL) {
    fclose(outFile);
  }
  /* done */
  return 0;
}
