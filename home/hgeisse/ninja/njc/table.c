/*
 * table.c -- symbol table
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "types.h"
#include "table.h"


Entry *newTypeEntry(Type *type) {
  Entry *entry;

  entry = (Entry *) allocate(sizeof(Entry));
  entry->kind = ENTRY_KIND_TYPE;
  entry->u.typeEntry.type = type;
  return entry;
}


Entry *newVarEntry(Type *type, boolean isStatic, int offset) {
  Entry *entry;

  entry = (Entry *) allocate(sizeof(Entry));
  entry->kind = ENTRY_KIND_VAR;
  entry->u.varEntry.type = type;
  entry->u.varEntry.isStatic = isStatic;
  entry->u.varEntry.offset = offset;
  return entry;
}


Entry *newFuncEntry(Type *returnType,
                    ParamTypes *paramTypes,
                    Table *localTable) {
  Entry *entry;

  entry = (Entry *) allocate(sizeof(Entry));
  entry->kind = ENTRY_KIND_FUNC;
  entry->u.funcEntry.returnType = returnType;
  entry->u.funcEntry.paramTypes = paramTypes;
  entry->u.funcEntry.localTable = localTable;
  entry->u.funcEntry.numParams = 0;
  entry->u.funcEntry.numLocals = 0;
  return entry;
}


Table *newTable(Table *outerScope) {
  Table *table;

  table = (Table *) allocate(sizeof(Table));
  table->bintree = NULL;
  table->outerScope = outerScope;
  return table;
}


Entry *enter(Table *table, Sym *sym, Entry *entry) {
  unsigned key;
  Bintree *newtree;
  Bintree *current;
  Bintree *previous;

  key = symToStamp(sym);
  newtree = (Bintree *) allocate(sizeof(Bintree));
  newtree->sym = sym;
  newtree->key = key;
  newtree->entry = entry;
  newtree->left = NULL;
  newtree->right = NULL;
  if (table->bintree == NULL) {
    table->bintree = newtree;
  } else {
    current = table->bintree;
    while (1) {
      if (current->key == key) {
        /* symbol already in table */
        return NULL;
      }
      previous = current;
      if (current->key > key) {
        current = current->left;
      } else {
        current = current->right;
      }
      if (current == NULL) {
        if (previous->key > key) {
          previous->left = newtree;
        } else {
          previous->right = newtree;
        }
        break;
      }
    }
  }
  return entry;
}


static Entry *lookupBintree(Bintree *bintree, unsigned key) {
  while (bintree != NULL) {
    if (bintree->key == key) {
      return bintree->entry;
    }
    if (bintree->key > key) {
      bintree = bintree->left;
    } else {
      bintree = bintree->right;
    }
  }
  return NULL;
}


Entry *lookup(Table *table, Sym *sym) {
  unsigned key;
  Entry *entry;

  key = symToStamp(sym);
  while (table != NULL) {
    entry = lookupBintree(table->bintree, key);
    if (entry != NULL) {
      return entry;
    }
    table = table->outerScope;
  }
  return NULL;
}


void showEntry(Entry *entry) {
  switch (entry->kind) {
    case ENTRY_KIND_TYPE:
      printf("type: ");
      showType(stdout, entry->u.typeEntry.type);
      printf("\n");
      break;
    case ENTRY_KIND_VAR:
      printf("var: ");
      showType(stdout, entry->u.varEntry.type);
      printf("\n");
      if (entry->u.varEntry.isStatic) {
        printf("\t\t\tstatic addr = %d\n", entry->u.varEntry.offset);
      } else {
        printf("\t\t\toffset (fp) = %d\n", entry->u.varEntry.offset);
      }
      break;
    case ENTRY_KIND_FUNC:
      printf("func: ");
      showType(stdout, entry->u.funcEntry.returnType);
      printf(" ");
      showParamTypes(stdout, entry->u.funcEntry.paramTypes);
      printf("\n");
      printf("\t\t\tnumber of params = %d\n",
             entry->u.funcEntry.numParams);
      printf("\t\t\tnumber of locals = %d\n",
             entry->u.funcEntry.numLocals);
      break;
    default:
      error("unknown entry kind %d in showEntry", entry->kind);
  }
}


static void showBintree(Bintree *bintree) {
  if (bintree == NULL) {
    return;
  }
  showBintree(bintree->left);
  printf("    %-15s --> ", symToString(bintree->sym));
  showEntry(bintree->entry);
  showBintree(bintree->right);
}


void showTable(Table *table) {
  int level;

  level = 0;
  while (table != NULL) {
    printf("  level %d\n", level);
    showBintree(table->bintree);
    table = table->outerScope;
    level++;
  }
}
