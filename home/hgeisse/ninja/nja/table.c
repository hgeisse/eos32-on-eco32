/*
 * table.c -- symbol table
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "coder.h"
#include "table.h"


typedef struct backpatch {
  int addr;			/* where to patch when value gets known */
  struct backpatch *next;	/* next backpatch for same symbol */
} Backpatch;

typedef struct entry {
  char *name;			/* name of symbol */
  boolean defined;		/* is it defined already? */
  int value;			/* if yes: that's its value */
  Backpatch *backpatches;	/* if no: which locations must be patched */
  struct entry *next;		/* next symbol table entry */
} Entry;


static boolean debugEnter = FALSE;
static boolean debugLookup = FALSE;

static Entry *symTable = NULL;


static Entry *newEntry(char *name, boolean defined, int value) {
  Entry *p;

  p = allocate(sizeof(Entry));
  p->name = name;
  p->defined = defined;
  p->value = value;
  p->backpatches = NULL;
  p->next = symTable;
  symTable = p;
  return p;
}


static Entry *searchEntry(char *name) {
  Entry *p;

  p = symTable;
  while (p != NULL) {
    if (strcmp(p->name, name) == 0) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}


void enter(char *name, int value) {
  Entry *p;
  Backpatch *q;

  p = searchEntry(name);
  if (p == NULL) {
    /* symbol is not yet in table: put it in */
    if (debugEnter) {
      printf("enter %s --> %d (definition before first use)\n",
             name, value);
    }
    newEntry(name, TRUE, value);
  } else {
    /* symbol is already in table */
    if (p->defined) {
      /* and has already been defined: this is an error */
      error("symbol '%s' defined more than once", name);
    }
    /* but it has not yet been defined: define it now */
    if (debugEnter) {
      printf("enter %s --> %d (definition after first use)\n",
             name, value);
    }
    p->defined = TRUE;
    p->value = value;
    /* resolve all forward references accumulated so far */
    q = p->backpatches;
    while (q != NULL) {
      patchCode(q->addr, value);
      q = q->next;
    }
  }
}


int lookup(char *name, int addr) {
  Entry *p;
  Backpatch *q;

  p = searchEntry(name);
  if (p == NULL) {
    /* symbol is not yet in table: put it in */
    if (debugLookup) {
      printf("lookup %s (first use before definition)\n",
             name);
    }
    p = newEntry(name, FALSE, 0);
  }
  if (!p->defined) {
    /* symbol is not yet defined: add a patch */
    if (debugLookup) {
      printf("backpatch at addr %d added to symbol %s\n",
             addr, name);
    }
    q = allocate(sizeof(Backpatch));
    q->addr = addr;
    q->next = p->backpatches;
    p->backpatches = q;
  }
  /* the returned value may be the dummy value 0 */
  return p->value;
}


void checkUndefined(boolean map) {
  boolean any;
  Entry *p;

  any = FALSE;
  p = symTable;
  while (p != NULL) {
    if (!p->defined) {
      printf("undefined symbol '%s'\n", p->name);
      any = TRUE;
    } else {
      if (map) {
        printf("%s\t%d\n", p->name, p->value);
      }
    }
    p = p->next;
  }
  if (any) {
    error("undefined symbol(s)");
  }
}
