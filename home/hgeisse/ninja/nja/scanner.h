/*
 * scanner.h -- scanner interface
 */


#ifndef _SCANNER_H_
#define _SCANNER_H_


typedef struct {
  int line;
} NoVal;

typedef struct {
  int line;
  int val;
} IntVal;

typedef struct {
  int line;
  char *val;
} StringVal;


extern FILE *yyin;


int yylex(void);


#endif /* _SCANNER_H_ */
