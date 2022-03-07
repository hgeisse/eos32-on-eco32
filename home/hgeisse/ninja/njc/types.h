/*
 * types.h -- representation of types
 */


#ifndef _TYPES_H_
#define _TYPES_H_


#define TYPE_KIND_PRIMITIVE	0
#define TYPE_KIND_ARRAY		1
#define TYPE_KIND_RECORD	2
#define TYPE_KIND_NAME		3


typedef struct type {
  int kind;
  union {
    struct {
      char *printName;
    } primitiveType;
    struct {
      struct type *baseType;
      boolean sizeSpecified;	/* true only when constructed in newExp */
    } arrayType;
    struct {
      struct membertypes *memberTypes;
      int size;			/* length of memberTypes list */
    } recordType;
    struct {
      Sym *name;
      int line;			/* location of usage, for error messages */
      struct type *resolvedType;
    } nameType;
  } u;
} Type;


typedef struct membertypes {
  boolean isEmpty;
  Sym *name;
  Type *type;
  int offset;
  struct membertypes *next;
} MemberTypes;


typedef struct paramtypes {
  boolean isEmpty;
  Type *type;
  struct paramtypes *next;
} ParamTypes;


Type *newPrimitiveType(char *printName);
Type *newArrayType(Type *baseType, boolean sizeSpecified);
Type *newRecordType(MemberTypes *memberTypes);
Type *newNameType(Sym *name, int line);

MemberTypes *emptyMemberTypes(void);
MemberTypes *newMemberTypes(Sym *name, Type *type, MemberTypes *next);

ParamTypes *emptyParamTypes(void);
ParamTypes *newParamTypes(Type *type, ParamTypes *next);

void showType(FILE *stream, Type *type);
void showMemberTypes(FILE *stream, MemberTypes *memberTypes);
void showParamTypes(FILE *stream, ParamTypes *paramTypes);


#endif /* _TYPES_H_ */
