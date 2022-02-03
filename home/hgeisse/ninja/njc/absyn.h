/*
 * absyn.h -- abstract syntax
 */


#ifndef _ABSYN_H_
#define _ABSYN_H_


#define ABSYN_VOIDTY		0
#define ABSYN_NAMETY		1
#define ABSYN_ARRAYTY		2
#define ABSYN_RECORDTY		3
#define ABSYN_MEMBDEC		4
#define ABSYN_TYPEDEC		5
#define ABSYN_GVARDEC		6
#define ABSYN_FUNCDEC		7
#define ABSYN_PARDEC		8
#define ABSYN_LVARDEC		9
#define ABSYN_EMPTYSTM		10
#define ABSYN_COMPSTM		11
#define ABSYN_ASSIGNSTM		12
#define ABSYN_IFSTM1		13
#define ABSYN_IFSTM2		14
#define ABSYN_WHILESTM		15
#define ABSYN_DOSTM		16
#define ABSYN_BREAKSTM		17
#define ABSYN_CALLSTM		18
#define ABSYN_RETSTM		19
#define ABSYN_RETEXPSTM		20
#define ABSYN_BINOPEXP		21
#define ABSYN_UNOPEXP		22
#define ABSYN_NILEXP		23
#define ABSYN_INTEXP		24
#define ABSYN_CHAREXP		25
#define ABSYN_BOOLEXP		26
#define ABSYN_STRINGEXP		27
#define ABSYN_NEWEXP		28
#define ABSYN_SIZEOFEXP		29
#define ABSYN_CALLEXP		30
#define ABSYN_SIMPLEVAR		31
#define ABSYN_ARRAYVAR		32
#define ABSYN_RECORDVAR		33
#define ABSYN_DECLIST		34
#define ABSYN_STMLIST		35
#define ABSYN_EXPLIST		36

#define ABSYN_BINOP_LOR		0
#define ABSYN_BINOP_LAND	1
#define ABSYN_BINOP_EQ		2
#define ABSYN_BINOP_NE		3
#define ABSYN_BINOP_LT		4
#define ABSYN_BINOP_LE		5
#define ABSYN_BINOP_GT		6
#define ABSYN_BINOP_GE		7
#define ABSYN_BINOP_ADD		8
#define ABSYN_BINOP_SUB		9
#define ABSYN_BINOP_MUL		10
#define ABSYN_BINOP_DIV		11
#define ABSYN_BINOP_MOD		12

#define ABSYN_UNOP_PLUS		0
#define ABSYN_UNOP_MINUS	1
#define ABSYN_UNOP_LNOT		2


typedef struct absyn {
  int type;
  int line;
  union {
    struct {
      int dummy;		/* empty struct not allowed in C */
    } voidTy;
    struct {
      Sym *name;
    } nameTy;
    struct {
      struct absyn *nElems;	/* used only in newExp, NULL otherwise */
      struct absyn *baseTy;
    } arrayTy;
    struct {
      struct absyn *members;
    } recordTy;
    struct {
      Sym *name;
      struct absyn *ty;
    } membDec;
    struct {
      Sym *name;
      struct absyn *ty;
    } typeDec;
    struct {
      Sym *name;
      struct absyn *ty;
    } gvarDec;
    struct {
      Sym *name;
      struct absyn *rtype;
      struct absyn *params;
      struct absyn *locals;
      struct absyn *body;
    } funcDec;
    struct {
      Sym *name;
      struct absyn *ty;
    } parDec;
    struct {
      Sym *name;
      struct absyn *ty;
    } lvarDec;
    struct {
      int dummy;		/* empty struct not allowed in C */
    } emptyStm;
    struct {
      struct absyn *stms;
    } compStm;
    struct {
      struct absyn *var;
      struct absyn *exp;
    } assignStm;
    struct {
      struct absyn *test;
      struct absyn *thenPart;
    } ifStm1;
    struct {
      struct absyn *test;
      struct absyn *thenPart;
      struct absyn *elsePart;
    } ifStm2;
    struct {
      struct absyn *test;
      struct absyn *body;
    } whileStm;
    struct {
      struct absyn *test;
      struct absyn *body;
    } doStm;
    struct {
      int dummy;		/* empty struct not allowed in C */
    } breakStm;
    struct {
      Sym *name;
      struct absyn *args;
    } callStm;
    struct {
      int dummy;		/* empty struct not allowed in C */
    } retStm;
    struct {
      struct absyn *exp;
    } retExpStm;
    struct {
      int op;
      struct absyn *left;
      struct absyn *right;
      boolean isRefComp;	/* filled in by semantic analysis */
    } binopExp;
    struct {
      int op;
      struct absyn *right;
    } unopExp;
    struct {
      int dummy;		/* empty struct not allowed in C */
    } nilExp;
    struct {
      char *val;
    } intExp;
    struct {
      int val;
    } charExp;
    struct {
      int val;
    } boolExp;
    struct {
      char *val;
    } stringExp;
    struct {
      struct absyn *ty;
      int size;			/* filled in by semantic analysis */
    } newExp;
    struct {
      struct absyn *exp;
    } sizeofExp;
    struct {
      Sym *name;
      struct absyn *args;
    } callExp;
    struct {
      Sym *name;
    } simpleVar;
    struct {
      struct absyn *array;
      struct absyn *index;
    } arrayVar;
    struct {
      struct absyn *record;
      Sym *member;
      int offset;		/* filled in by semantic analysis */
    } recordVar;
    struct {
      boolean isEmpty;
      struct absyn *head;
      struct absyn *tail;
    } decList;
    struct {
      boolean isEmpty;
      struct absyn *head;
      struct absyn *tail;
    } stmList;
    struct {
      boolean isEmpty;
      struct absyn *head;
      struct absyn *tail;
    } expList;
  } u;
} Absyn;


Absyn *newVoidTy(int line);
Absyn *newNameTy(int line, Sym *name);
Absyn *newArrayTy(int line, Absyn *nElems, Absyn *baseTy);
Absyn *newRecordTy(int line, Absyn *members);
Absyn *newMembDec(int line, Sym *name, Absyn *ty);
Absyn *newTypeDec(int line, Sym *name, Absyn *ty);
Absyn *newGvarDec(int line, Sym *name, Absyn *ty);
Absyn *newFuncDec(int line, Sym *name, Absyn *rtype, Absyn *params,
                  Absyn *locals, Absyn *body);
Absyn *newParDec(int line, Sym *name, Absyn *ty);
Absyn *newLvarDec(int line, Sym *name, Absyn *ty);
Absyn *newEmptyStm(int line);
Absyn *newCompStm(int line, Absyn *stms);
Absyn *newAssignStm(int line, Absyn *var, Absyn *exp);
Absyn *newIfStm1(int line, Absyn *test, Absyn *thenPart);
Absyn *newIfStm2(int line, Absyn *test, Absyn *thenPart, Absyn *elsePart);
Absyn *newWhileStm(int line, Absyn *test, Absyn *body);
Absyn *newDoStm(int line, Absyn *test, Absyn *body);
Absyn *newBreakStm(int line);
Absyn *newCallStm(int line, Sym *name, Absyn *args);
Absyn *newRetStm(int line);
Absyn *newRetExpStm(int line, Absyn *exp);
Absyn *newBinopExp(int line, int op, Absyn *left, Absyn *right);
Absyn *newUnopExp(int line, int op, Absyn *right);
Absyn *newNilExp(int line);
Absyn *newIntExp(int line, char *val);
Absyn *newCharExp(int line, int val);
Absyn *newBoolExp(int line, int val);
Absyn *newStringExp(int line, char *val);
Absyn *newNewExp(int line, Absyn *ty);
Absyn *newSizeofExp(int line, Absyn *exp);
Absyn *newCallExp(int line, Sym *name, Absyn *args);
Absyn *newSimpleVar(int line, Sym *name);
Absyn *newArrayVar(int line, Absyn *array, Absyn *index);
Absyn *newRecordVar(int line, Absyn *record, Sym *member);
Absyn *emptyDecList(void);
Absyn *newDecList(Absyn *head, Absyn *tail);
Absyn *emptyStmList(void);
Absyn *newStmList(Absyn *head, Absyn *tail);
Absyn *emptyExpList(void);
Absyn *newExpList(Absyn *head, Absyn *tail);

void showAbsyn(Absyn *node);


#endif /* _ABSYN_H_ */
