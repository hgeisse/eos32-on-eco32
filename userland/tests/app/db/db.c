/*
 * db.c -- data base
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"


typedef struct info {
  char *name;
  int value;
} Info;


Info data[] = {
  { "Susan",   13 },
  { "John",    42 },
  { "Nancy",    7 },
  { "Charles", 22 },
  { "Paul",    42 },
  { "Ann",     17 },
  { "Mary",    55 },
  { "Edward",   1 },
  { "Dick",    12 }
};


typedef struct record {
  char *name;
  int value;
  struct record *left;
  struct record *right;
} Record;


Record *db = NULL;


void prepare(void) {
  int i;
  Record *p, *q, *r;
  int cmp;

  for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
    printf("entering '%s' -- %d\n", data[i].name, data[i].value);
    r = malloc(sizeof(Record));
    if (r == NULL) {
      printf("Error: out of memory!\n");
      exit(1);
    }
    r->name = data[i].name;
    r->value = data[i].value;
    r->left = NULL;
    r->right = NULL;
    if (db == NULL) {
      db = r;
    } else {
      p = db;
      while (1) {
        q = p;
        cmp = strcmp(r->name, q->name);
        if (cmp < 0) {
          p = q->left;
        } else {
          p = q->right;
        }
        if (p == NULL) {
          if (cmp < 0) {
            q->left = r;
          } else {
            q->right = r;
          }
          break;
        }
      }
    }
  }
}


int lookup(char *name) {
  Record *p;
  int cmp;

  p = db;
  while (p != NULL) {
    cmp = strcmp(name, p->name);
    if (cmp == 0) {
      return p->value;
    }
    if (cmp < 0) {
      p = p->left;
    } else {
      p = p->right;
    }
  }
  return -1;
}
