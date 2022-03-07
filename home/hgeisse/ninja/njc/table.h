/*
 * table.h -- symbol table
 */


#ifndef _TABLE_H_
#define _TABLE_H_


#define ENTRY_KIND_TYPE		0
#define ENTRY_KIND_VAR		1
#define ENTRY_KIND_FUNC		2


typedef struct {
  int kind;
  union {
    struct {
      Type *type;
    } typeEntry;
    struct {
      Type *type;
      boolean isStatic;
      int offset;		/* either relative to fp (if !isStatic),
				   or an absolute address (if isStatic) */
    } varEntry;
    struct {
      Type *returnType;
      ParamTypes *paramTypes;
      struct table *localTable;
      int numParams;
      int numLocals;
    } funcEntry;
  } u;
} Entry;


typedef struct bintree {
  Sym *sym;
  unsigned key;
  Entry *entry;
  struct bintree *left;
  struct bintree *right;
} Bintree;


typedef struct table {
  Bintree *bintree;
  struct table *outerScope;
} Table;


Entry *newTypeEntry(Type *type);
Entry *newVarEntry(Type *type,
                   boolean isStatic,
                   int offset);
Entry *newFuncEntry(Type *returnType,
                    ParamTypes *paramTypes,
                    Table *localTable);

Table *newTable(Table *outerScope);
Entry *enter(Table *table, Sym *sym, Entry *entry);
Entry *lookup(Table *table, Sym *sym);

void showEntry(Entry *entry);
void showTable(Table *table);


#endif /* _TABLE_H_ */
