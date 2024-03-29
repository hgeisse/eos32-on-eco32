%{

/*
 * eco32.md -- ECO32 back-end specification
 *
 * register usage:
 *   $0   always zero
 *   $1   reserved for assembler
 *   $2   func return value
 *   $3   func return value
 *   $4   proc/func argument
 *   $5   proc/func argument
 *   $6   proc/func argument
 *   $7   proc/func argument
 *   $8   temporary register (caller-save)
 *   $9   temporary register (caller-save)
 *   $10  temporary register (caller-save)
 *   $11  temporary register (caller-save)
 *   $12  temporary register (caller-save)
 *   $13  temporary register (caller-save)
 *   $14  temporary register (caller-save)
 *   $15  temporary register (caller-save)
 *   $16  register variable  (callee-save)
 *   $17  register variable  (callee-save)
 *   $18  register variable  (callee-save)
 *   $19  register variable  (callee-save)
 *   $20  register variable  (callee-save)
 *   $21  register variable  (callee-save)
 *   $22  register variable  (callee-save)
 *   $23  register variable  (callee-save)
 *   $24  temporary register (caller-save)
 *   $25  temporary register (caller-save)
 *   $26  reserved for OS kernel
 *   $27  reserved for OS kernel
 *   $28  reserved for OS kernel
 *   $29  stack pointer
 *   $30  interrupt return address
 *   $31  proc/func return address
 * caller-save registers are not preserved across procedure calls
 * callee-save registers are preserved across procedure calls
 *
 * tree grammar terminals produced by:
 *   ops c=1 s=2 i=4 l=4 h=4 f=4 d=4 x=4 p=4
 * and augmented by:
 *   %term LOADB=233
 *   %term LOADF4=4321
 *   %term LOADI1=1253 LOADI2=2277 LOADI4=4325
 *   %term LOADP4=4327
 *   %term LOADU1=1254 LOADU2=2278 LOADU4=4326
 *   %term VREGP=711
 */

#include "c.h"

#define NODEPTR_TYPE	Node
#define OP_LABEL(p)	((p)->op)
#define LEFT_CHILD(p)	((p)->kids[0])
#define RIGHT_CHILD(p)	((p)->kids[1])
#define STATE_LABEL(p)	((p)->x.state)

static void address(Symbol, Symbol, long);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void export(Symbol);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char * []);
static void progend(void);
static void segment(int);
static void space(int);
static Symbol rmap(int);
static void blkfetch(int, int, int, int);
static void blkstore(int, int, int, int);
static void blkloop(int, int, int, int, int, int []);
static void emit2(Node);
static void doarg(Node);
static void target(Node);
static void clobber(Node);

#define INTTMP	0x0100FF00
#define INTVAR	0x00FF0000
#define INTRET	0x00000004

static Symbol ireg[32];
static Symbol iregw;
static Symbol blkreg;
static int tmpregs[] = { 3, 9, 10 };

%}

%start stmt

%term CNSTF4=4113
%term CNSTI1=1045 CNSTI2=2069 CNSTI4=4117
%term CNSTP4=4119
%term CNSTU1=1046 CNSTU2=2070 CNSTU4=4118

%term ARGB=41
%term ARGF4=4129
%term ARGI4=4133
%term ARGP4=4135
%term ARGU4=4134

%term ASGNB=57
%term ASGNF4=4145
%term ASGNI1=1077 ASGNI2=2101 ASGNI4=4149
%term ASGNP4=4151
%term ASGNU1=1078 ASGNU2=2102 ASGNU4=4150

%term INDIRB=73
%term INDIRF4=4161
%term INDIRI1=1093 INDIRI2=2117 INDIRI4=4165
%term INDIRP4=4167
%term INDIRU1=1094 INDIRU2=2118 INDIRU4=4166

%term CVFF4=4209
%term CVFI4=4213

%term CVIF4=4225
%term CVII1=1157 CVII2=2181 CVII4=4229
%term CVIU1=1158 CVIU2=2182 CVIU4=4230

%term CVPU4=4246

%term CVUI1=1205 CVUI2=2229 CVUI4=4277
%term CVUP4=4279
%term CVUU1=1206 CVUU2=2230 CVUU4=4278

%term NEGF4=4289
%term NEGI4=4293

%term CALLB=217
%term CALLF4=4305
%term CALLI4=4309
%term CALLP4=4311
%term CALLU4=4310
%term CALLV=216

%term RETF4=4337
%term RETI4=4341
%term RETP4=4343
%term RETU4=4342
%term RETV=248

%term ADDRGP4=4359

%term ADDRFP4=4375

%term ADDRLP4=4391

%term ADDF4=4401
%term ADDI4=4405
%term ADDP4=4407
%term ADDU4=4406

%term SUBF4=4417
%term SUBI4=4421
%term SUBP4=4423
%term SUBU4=4422

%term LSHI4=4437
%term LSHU4=4438

%term MODI4=4453
%term MODU4=4454

%term RSHI4=4469
%term RSHU4=4470

%term BANDI4=4485
%term BANDU4=4486

%term BCOMI4=4501
%term BCOMU4=4502

%term BORI4=4517
%term BORU4=4518

%term BXORI4=4533
%term BXORU4=4534

%term DIVF4=4545
%term DIVI4=4549
%term DIVU4=4550

%term MULF4=4561
%term MULI4=4565
%term MULU4=4566

%term EQF4=4577
%term EQI4=4581
%term EQU4=4582

%term GEF4=4593
%term GEI4=4597
%term GEU4=4598

%term GTF4=4609
%term GTI4=4613
%term GTU4=4614

%term LEF4=4625
%term LEI4=4629
%term LEU4=4630

%term LTF4=4641
%term LTI4=4645
%term LTU4=4646

%term NEF4=4657
%term NEI4=4661
%term NEU4=4662

%term JUMPV=584

%term LABELV=600

%term LOADB=233
%term LOADF4=4321
%term LOADI1=1253 LOADI2=2277 LOADI4=4325
%term LOADP4=4327
%term LOADU1=1254 LOADU2=2278 LOADU4=4326

%term VREGP=711


%%


reg:	INDIRI1(VREGP)		"# read register\n"
reg:	INDIRI2(VREGP)		"# read register\n"
reg:	INDIRI4(VREGP)		"# read register\n"
reg:	INDIRP4(VREGP)		"# read register\n"
reg:	INDIRU1(VREGP)		"# read register\n"
reg:	INDIRU2(VREGP)		"# read register\n"
reg:	INDIRU4(VREGP)		"# read register\n"

stmt:	ASGNI1(VREGP,reg)	"# write register\n"
stmt:	ASGNI2(VREGP,reg)	"# write register\n"
stmt:	ASGNI4(VREGP,reg)	"# write register\n"
stmt:	ASGNP4(VREGP,reg)	"# write register\n"
stmt:	ASGNU1(VREGP,reg)	"# write register\n"
stmt:	ASGNU2(VREGP,reg)	"# write register\n"
stmt:	ASGNU4(VREGP,reg)	"# write register\n"

con:	CNSTI1			"%a"
con:	CNSTI2			"%a"
con:	CNSTI4			"%a"
con:	CNSTP4			"%a"
con:	CNSTU1			"%a"
con:	CNSTU2			"%a"
con:	CNSTU4			"%a"

stmt:	reg			""

acon:	con			"%0"
acon:	ADDRGP4			"%a"

addr:	ADDI4(reg,acon)		"$%0,%1"
addr:	ADDP4(reg,acon)		"$%0,%1"
addr:	ADDU4(reg,acon)		"$%0,%1"

addr:	acon			"$0,%0"
addr:	reg			"$%0,0"
addr:	ADDRFP4			"$29,%a+%F"
addr:	ADDRLP4			"$29,%a+%F"

reg:	addr			"\tadd\t$%c,%0\n"	1

reg:	CNSTI1			"# reg\n"		range(a, 0, 0)
reg:	CNSTI2			"# reg\n"		range(a, 0, 0)
reg:	CNSTI4			"# reg\n"		range(a, 0, 0)
reg:	CNSTP4			"# reg\n"		range(a, 0, 0)
reg:	CNSTU1			"# reg\n"		range(a, 0, 0)
reg:	CNSTU2			"# reg\n"		range(a, 0, 0)
reg:	CNSTU4			"# reg\n"		range(a, 0, 0)

stmt:	ASGNI1(addr,reg)	"\tstb\t$%1,%0\n"	1
stmt:	ASGNI2(addr,reg)	"\tsth\t$%1,%0\n"	1
stmt:	ASGNI4(addr,reg)	"\tstw\t$%1,%0\n"	1
stmt:	ASGNP4(addr,reg)	"\tstw\t$%1,%0\n"	1
stmt:	ASGNU1(addr,reg)	"\tstb\t$%1,%0\n"	1
stmt:	ASGNU2(addr,reg)	"\tsth\t$%1,%0\n"	1
stmt:	ASGNU4(addr,reg)	"\tstw\t$%1,%0\n"	1

reg:	INDIRI1(addr)		"\tldb\t$%c,%0\n"	1
reg:	INDIRI2(addr)		"\tldh\t$%c,%0\n"	1
reg:	INDIRI4(addr)		"\tldw\t$%c,%0\n"	1
reg:	INDIRP4(addr)		"\tldw\t$%c,%0\n"	1
reg:	INDIRU1(addr)		"\tldbu\t$%c,%0\n"	1
reg:	INDIRU2(addr)		"\tldhu\t$%c,%0\n"	1
reg:	INDIRU4(addr)		"\tldw\t$%c,%0\n"	1

reg:	CVII4(INDIRI1(addr))	"\tldb\t$%c,%0\n"	1
reg:	CVII4(INDIRI2(addr))	"\tldh\t$%c,%0\n"	1
reg:	CVUU4(INDIRU1(addr))	"\tldbu\t$%c,%0\n"	1
reg:	CVUU4(INDIRU2(addr))	"\tldhu\t$%c,%0\n"	1
reg:	CVUI4(INDIRU1(addr))	"\tldbu\t$%c,%0\n"	1
reg:	CVUI4(INDIRU2(addr))	"\tldhu\t$%c,%0\n"	1

rc:	con			"%0"
rc:	reg			"$%0"

reg:	ADDI4(reg,rc)		"\tadd\t$%c,$%0,%1\n"	1
reg:	ADDP4(reg,rc)		"\tadd\t$%c,$%0,%1\n"	1
reg:	ADDU4(reg,rc)		"\tadd\t$%c,$%0,%1\n"	1
reg:	SUBI4(reg,rc)		"\tsub\t$%c,$%0,%1\n"	1
reg:	SUBP4(reg,rc)		"\tsub\t$%c,$%0,%1\n"	1
reg:	SUBU4(reg,rc)		"\tsub\t$%c,$%0,%1\n"	1
reg:	NEGI4(reg)		"\tsub\t$%c,$0,$%0\n"	1

reg:	MULI4(reg,rc)		"\tmul\t$%c,$%0,%1\n"	1
reg:	MULU4(reg,rc)		"\tmulu\t$%c,$%0,%1\n"	1
reg:	DIVI4(reg,rc)		"\tdiv\t$%c,$%0,%1\n"	1
reg:	DIVU4(reg,rc)		"\tdivu\t$%c,$%0,%1\n"	1
reg:	MODI4(reg,rc)		"\trem\t$%c,$%0,%1\n"	1
reg:	MODU4(reg,rc)		"\tremu\t$%c,$%0,%1\n"	1

reg:	BANDI4(reg,rc)		"\tand\t$%c,$%0,%1\n"	1
reg:	BANDU4(reg,rc)		"\tand\t$%c,$%0,%1\n"	1
reg:	BORI4(reg,rc)		"\tor\t$%c,$%0,%1\n"	1
reg:	BORU4(reg,rc)		"\tor\t$%c,$%0,%1\n"	1
reg:	BXORI4(reg,rc)		"\txor\t$%c,$%0,%1\n"	1
reg:	BXORU4(reg,rc)		"\txor\t$%c,$%0,%1\n"	1
reg:	BCOMI4(reg)		"\txnor\t$%c,$0,$%0\n"	1
reg:	BCOMU4(reg)		"\txnor\t$%c,$0,$%0\n"	1

rc5:	CNSTI4			"%a"			range(a, 0, 31)
rc5:	reg			"$%0"

reg:	LSHI4(reg,rc5)		"\tsll\t$%c,$%0,%1\n"	1
reg:	LSHU4(reg,rc5)		"\tsll\t$%c,$%0,%1\n"	1
reg:	RSHI4(reg,rc5)		"\tsar\t$%c,$%0,%1\n"	1
reg:	RSHU4(reg,rc5)		"\tslr\t$%c,$%0,%1\n"	1

reg:	LOADI1(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADI2(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADI4(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADP4(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADU1(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADU2(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	LOADU4(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)

reg:	CVII4(reg)  "\tsll\t$%c,$%0,8*(4-%a)\n\tsar\t$%c,$%c,8*(4-%a)\n"  2
reg:	CVUI4(reg)  "\tand\t$%c,$%0,(1<<(8*%a))-1\n"	1
reg:	CVUU4(reg)  "\tand\t$%c,$%0,(1<<(8*%a))-1\n"	1

stmt:	LABELV			"%a:\n"
stmt:	JUMPV(acon)		"\tj\t%0\n"		1
stmt:	JUMPV(reg)		"\tjr\t$%0\n"		1

stmt:	EQI4(reg,reg)		"\tbeq\t$%0,$%1,%a\n"	1
stmt:	EQU4(reg,reg)		"\tbeq\t$%0,$%1,%a\n"	1
stmt:	NEI4(reg,reg)		"\tbne\t$%0,$%1,%a\n"	1
stmt:	NEU4(reg,reg)		"\tbne\t$%0,$%1,%a\n"	1
stmt:	LEI4(reg,reg)		"\tble\t$%0,$%1,%a\n"	1
stmt:	LEU4(reg,reg)		"\tbleu\t$%0,$%1,%a\n"	1
stmt:	LTI4(reg,reg)		"\tblt\t$%0,$%1,%a\n"	1
stmt:	LTU4(reg,reg)		"\tbltu\t$%0,$%1,%a\n"	1
stmt:	GEI4(reg,reg)		"\tbge\t$%0,$%1,%a\n"	1
stmt:	GEU4(reg,reg)		"\tbgeu\t$%0,$%1,%a\n"	1
stmt:	GTI4(reg,reg)		"\tbgt\t$%0,$%1,%a\n"	1
stmt:	GTU4(reg,reg)		"\tbgtu\t$%0,$%1,%a\n"	1

reg:	CALLI4(ar)		"\tjal\t%0\n"		1
reg:	CALLP4(ar)		"\tjal\t%0\n"		1
reg:	CALLU4(ar)		"\tjal\t%0\n"		1
stmt:	CALLV(ar)		"\tjal\t%0\n"		1

ar:	ADDRGP4			"%a"
ar:	reg			"$%0"
ar:	CNSTP4			"%a"		range(a, 0, 0x03FFFFFF)

stmt:	RETI4(reg)		"# ret\n"		1
stmt:	RETP4(reg)		"# ret\n"		1
stmt:	RETU4(reg)		"# ret\n"		1
stmt:	RETV(reg)		"# ret\n"		1

stmt:	ARGI4(reg)		"# arg\n"		1
stmt:	ARGP4(reg)		"# arg\n"		1
stmt:	ARGU4(reg)		"# arg\n"		1

stmt:	ARGB(INDIRB(reg))	"# argb %0\n"		1
stmt:	ASGNB(reg,INDIRB(reg))	"# asgnb %0 %1\n"	1

reg:	INDIRF4(VREGP)		"# read register\n"
stmt:	ASGNF4(VREGP,reg)	"# write register\n"
reg:	INDIRF4(addr)		"\tldw\t$%c,%0\n"	1
stmt:	ASGNF4(addr,reg)	"\tstw\t$%1,%0\n"	1
reg:	LOADF4(reg)		"\tadd\t$%c,$0,$%0\n"	move(a)
reg:	ADDF4(reg,reg)		"\taddf\t$%c,$%0,$%1\n"	1
reg:	SUBF4(reg,reg)		"\tsubf\t$%c,$%0,$%1\n"	1
reg:	MULF4(reg,reg)		"\tmulf\t$%c,$%0,$%1\n"	1
reg:	DIVF4(reg,reg)		"\tdivf\t$%c,$%0,$%1\n"	1
reg:	NEGF4(reg)		"\tsubf\t$%c,$0,$%0\n"	1
reg:	CVFF4(reg)		"# cvt d2s\n"		1
reg:	CVIF4(reg)		"\tcif\t$%c,$%0\n"	1
reg:	CVFI4(reg)		"\tcfit\t$%c,$%0\n"	1
stmt:	EQF4(reg,reg)		"\teqf\t$%0,$%1\n\tbfpt %a\n"	1
stmt:	NEF4(reg,reg)		"\tneqf\t$%0,$%1\n\tbfpt %a\n"	1
stmt:	LEF4(reg,reg)		"\tulef\t$%0,$%1\n\tbfpt %a\n"	1
stmt:	LTF4(reg,reg)		"\tultf\t$%0,$%1\n\tbfpt %a\n"	1
stmt:	GEF4(reg,reg)		"\tugef\t$%0,$%1\n\tbfpt %a\n"	1
stmt:	GTF4(reg,reg)		"\tugtf\t$%0,$%1\n\tbfpt %a\n"	1
reg:	CALLF4(ar)		"\tjal\t%0\n"		1
stmt:	RETF4(reg)		"# ret\n"		1
stmt:	ARGF4(reg)		"# arg\n"		1


%%


static void address(Symbol s1, Symbol s2, long n) {
  if (s2->scope == GLOBAL ||
      s2->sclass == STATIC ||
      s2->sclass == EXTERN) {
    s1->x.name = stringf("%s%s%D", s2->x.name, n >= 0 ? "+" : "", n);
  } else {
    assert(n >= INT_MIN && n <= INT_MAX);
    s1->x.offset = s2->x.offset + n;
    s1->x.name = stringd(s1->x.offset);
  }
}


static void defaddress(Symbol s) {
  print("\t.word\t%s\n", s->x.name);
}


static void defconst(int suffix, int size, Value v) {
  float f;
  double d;
  unsigned *p;

  if (suffix == F && size == 4) {
    f = v.d;
    print("\t.word\t0x%x\n", * (unsigned *) &f);
  } else
  if (suffix == F && size == 8) {
    d = v.d;
    p = (unsigned *) &d;
    print("\t.word\t0x%x\n", p[swap]);
    print("\t.word\t0x%x\n", p[1 - swap]);
  } else
  if (suffix == P) {
    print("\t.word\t0x%X\n", (unsigned long) v.p);
  } else
  if (size == 1) {
    print("\t.byte\t0x%x\n",
          (unsigned) ((unsigned char) (suffix == I ? v.i : v.u)));
  } else
  if (size == 2) {
    print("\t.half\t0x%x\n",
          (unsigned) ((unsigned short) (suffix == I ? v.i : v.u)));
  } else
  if (size == 4) {
    print("\t.word\t0x%x\n", (unsigned) (suffix == I ? v.i : v.u));
  }
}


static void defstring(int n, char *str) {
  char *s;

  for (s = str; s < str + n; s++) {
    print("\t.byte\t0x%x\n", (*s) & 0xFF);
  }
}


static void defsymbol(Symbol s) {
  if (s->scope >= LOCAL && s->sclass == STATIC) {
    s->x.name = stringf("L.%d", genlabel(1));
  } else
  if (s->generated) {
    s->x.name = stringf("L.%s", s->name);
  } else {
    assert(s->scope != CONSTANTS || isint(s->type) || isptr(s->type));
    s->x.name = s->name;
  }
}


static void export(Symbol s) {
  print("\t.export\t%s\n", s->name);
}


static int bitcount(unsigned mask) {
  unsigned i, n;

  n = 0;
  for (i = 1; i != 0; i <<= 1) {
    if (mask & i) {
      n++;
    }
  }
  return n;
}


static Symbol argreg(int argno, int offset, int ty, int sz, int ty0) {
  assert((offset & 3) == 0);
  if (offset > 12) {
    return NULL;
  }
  return ireg[(offset / 4) + 4];
}


static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
  int i;
  Symbol p, q;
  Symbol r;
  int sizeisave;
  int saved;
  Symbol argregs[4];

  usedmask[0] = usedmask[1] = 0;
  freemask[0] = freemask[1] = ~((unsigned) 0);
  offset = 0;
  maxoffset = 0;
  maxargoffset = 0;
  for (i = 0; callee[i] != NULL; i++) {
    p = callee[i];
    q = caller[i];
    assert(q != NULL);
    offset = roundup(offset, q->type->align);
    p->x.offset = q->x.offset = offset;
    p->x.name = q->x.name = stringd(offset);
    r = argreg(i, offset, optype(ttob(q->type)),
               q->type->size, optype(ttob(caller[0]->type)));
    if (i < 4) {
      argregs[i] = r;
    }
    offset = roundup(offset + q->type->size, 4);
    if (variadic(f->type)) {
      p->sclass = AUTO;
    } else
    if (r != NULL && ncalls == 0 && !isstruct(q->type) &&
        !p->addressed && !(isfloat(q->type) && r->x.regnode->set == IREG)) {
      p->sclass = q->sclass = REGISTER;
      askregvar(p, r);
      assert(p->x.regnode && p->x.regnode->vbl == p);
      q->x = p->x;
      q->type = p->type;
    } else
    if (askregvar(p, rmap(ttob(p->type))) &&
        r != NULL && (isint(p->type) || p->type == q->type)) {
      assert(q->sclass != REGISTER);
      p->sclass = q->sclass = REGISTER;
      q->type = p->type;
    }
  }
  assert(caller[i] == NULL);
  offset = 0;
  gencode(caller, callee);
  if (ncalls != 0) {
    usedmask[IREG] |= ((unsigned) 1) << 31;
  }
  usedmask[IREG] &= 0x80FF0000;
  maxargoffset = roundup(maxargoffset, 4);
  if (ncalls != 0 && maxargoffset < 16) {
    maxargoffset = 16;
  }
  sizeisave = 4 * bitcount(usedmask[IREG]);
  framesize = roundup(maxargoffset + sizeisave + maxoffset, 16);
  segment(CODE);
  print("\t.align\t4\n");
  print("%s:\n", f->x.name);
  if (framesize > 0) {
    print("\tsub\t$29,$29,%d\n", framesize);
  }
  saved = maxargoffset;
  for (i = 16; i < 32; i++) {
    if (usedmask[IREG] & (1 << i)) {
      print("\tstw\t$%d,$29,%d\n", i, saved);
      saved += 4;
    }
  }
  for (i = 0; i < 4 && callee[i] != NULL; i++) {
    r = argregs[i];
    if (r && r->x.regnode != callee[i]->x.regnode) {
      Symbol out = callee[i];
      Symbol in = caller[i];
      int rn = r->x.regnode->number;
      int rs = r->x.regnode->set;
      int tyin = ttob(in->type);
      assert(out && in && r && r->x.regnode);
      assert(out->sclass != REGISTER || out->x.regnode);
      if (out->sclass == REGISTER &&
          (isint(out->type) || out->type == in->type)) {
        int outn = out->x.regnode->number;
        print("\tadd\t$%d,$0,$%d\n", outn, rn);
      } else {
        int off = in->x.offset + framesize;
        int n = (in->type->size + 3) / 4;
        int i;
        for (i = rn; i < rn + n && i <= 7; i++) {
          print("\tstw\t$%d,$29,%d\n", i, off + (i - rn) * 4);
        }
      }
    }
  }
  if (variadic(f->type) && callee[i - 1] != NULL) {
    i = callee[i - 1]->x.offset + callee[i - 1]->type->size;
    for (i = roundup(i, 4)/4; i <= 3; i++) {
      print("\tstw\t$%d,$29,%d\n", i + 4, framesize + 4 * i);
    }
  }
  emitcode();
  saved = maxargoffset;
  for (i = 16; i < 32; i++) {
    if (usedmask[IREG] & (1 << i)) {
      print("\tldw\t$%d,$29,%d\n", i, saved);
      saved += 4;
    }
  }
  if (framesize > 0) {
    print("\tadd\t$29,$29,%d\n", framesize);
  }
  print("\tjr\t$31\n");
  print("\n");
}


static void global(Symbol s) {
  if (s->type->align != 0) {
    print("\t.align\t%d\n", s->type->align);
  } else {
    print("\t.align\t%d\n", 4);
  }
  print("%s:\n", s->x.name);
}


static void import(Symbol s) {
  print("\t.import\t%s\n", s->name);
}


static void local(Symbol s) {
  if (askregvar(s, rmap(ttob(s->type))) == 0) {
    mkauto(s);
  }
}


static void setSwap(void) {
  union {
    char c;
    int i;
  } u;

  u.i = 0;
  u.c = 1;
  swap = ((u.i == 1) != IR->little_endian);
}


static void progbeg(int argc, char *argv[]) {
  int i;

  setSwap();
  segment(CODE);
  parseflags(argc, argv);
  for (i = 0; i < 32; i++) {
    ireg[i] = mkreg("%d", i, 1, IREG);
  }
  iregw = mkwildcard(ireg);
  tmask[IREG] = INTTMP;
  vmask[IREG] = INTVAR;
  tmask[FREG] = 0;
  vmask[FREG] = 0;
  blkreg = mkreg("8", 8, 7, IREG);
}


static void progend(void) {
}


static void segment(int n) {
  static int currSeg = -1;
  int newSeg;

  switch (n) {
    case CODE:
      newSeg = CODE;
      break;
    case BSS:
      newSeg = BSS;
      break;
    case DATA:
      newSeg = DATA;
      break;
    case LIT:
      newSeg = DATA;
      break;
  }
  if (currSeg == newSeg) {
    return;
  }
  switch (newSeg) {
    case CODE:
      print("\t.code\n");
      break;
    case BSS:
      print("\t.bss\n");
      break;
    case DATA:
      print("\t.data\n");
      break;
  }
  currSeg = newSeg;
}


static void space(int n) {
  print("\t.space\t%d\n", n);
}


static Symbol rmap(int opk) {
  switch (optype(opk)) {
    case I:
    case U:
    case P:
    case B:
    case F:
      return iregw;
    default:
      return 0;
  }
}


static void blkfetch(int size, int off, int reg, int tmp) {
  assert(size == 1 || size == 2 || size == 4);
  if (size == 1) {
    print("\tldbu\t$%d,$%d,%d\n", tmp, reg, off);
  } else
  if (size == 2) {
    print("\tldhu\t$%d,$%d,%d\n", tmp, reg, off);
  } else
  if (size == 4) {
    print("\tldw\t$%d,$%d,%d\n", tmp, reg, off);
  }
}


static void blkstore(int size, int off, int reg, int tmp) {
  assert(size == 1 || size == 2 || size == 4);
  if (size == 1) {
    print("\tstb\t$%d,$%d,%d\n", tmp, reg, off);
  } else
  if (size == 2) {
    print("\tsth\t$%d,$%d,%d\n", tmp, reg, off);
  } else
  if (size == 4) {
    print("\tstw\t$%d,$%d,%d\n", tmp, reg, off);
  }
}


static void blkloop(int dreg, int doff,
                    int sreg, int soff,
                    int size, int tmps[]) {
  int label;

  label = genlabel(1);
  print("\tadd\t$%d,$%d,%d\n", sreg, sreg, size & ~7);
  print("\tadd\t$%d,$%d,%d\n", tmps[2], dreg, size & ~7);
  blkcopy(tmps[2], doff, sreg, soff, size & 7, tmps);
  print("L.%d:\n", label);
  print("\tsub\t$%d,$%d,%d\n", sreg, sreg, 8);
  print("\tsub\t$%d,$%d,%d\n", tmps[2], tmps[2], 8);
  blkcopy(tmps[2], doff, sreg, soff, 8, tmps);
  print("\tbltu\t$%d,$%d,L.%d\n", dreg, tmps[2], label);
}


static void emit2(Node p) {
  static int ty0;
  int ty, sz;
  Symbol q;
  int src;
  int dst, n;

  switch (specific(p->op)) {
    case ARG+I:
    case ARG+P:
    case ARG+U:
    case ARG+F:
      ty = optype(p->op);
      sz = opsize(p->op);
      if (p->x.argno == 0) {
        ty0 = ty;
      }
      q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, sz, ty0);
      src = getregnum(p->x.kids[0]);
      if (q == NULL) {
        print("\tstw\t$%d,$29,%d\n", src, p->syms[2]->u.c.v.i);
      }
      break;
    case ASGN+B:
      dalign = p->syms[1]->u.c.v.i;
      salign = p->syms[1]->u.c.v.i;
      blkcopy(getregnum(p->x.kids[0]), 0,
              getregnum(p->x.kids[1]), 0,
              p->syms[0]->u.c.v.i, tmpregs);
      break;
    case ARG+B:
      dalign = 4;
      salign = p->syms[1]->u.c.v.i;
      blkcopy(29, p->syms[2]->u.c.v.i,
              getregnum(p->x.kids[0]), 0,
              p->syms[0]->u.c.v.i, tmpregs);
      n = p->syms[2]->u.c.v.i + p->syms[0]->u.c.v.i;
      dst = p->syms[2]->u.c.v.i;
      for (; dst <= 12 && dst < n; dst += 4) {
        print("\tldw\t$%d,$29,%d\n", (dst / 4) + 4, dst);
      }
      break;
  }
}


static void doarg(Node p) {
  static int argno;
  int align;
  int size;
  int offset;

  if (argoffset == 0) {
    argno = 0;
  }
  p->x.argno = argno++;
  align = p->syms[1]->u.c.v.i;
  if (align < 4) {
    align = 4;
  }
  size = p->syms[0]->u.c.v.i;
  offset = mkactual(align, size);
  p->syms[2] = intconst(offset);
}


static void target(Node p) {
  static int ty0;
  int ty;
  Symbol q;

  assert(p);
  switch (specific(p->op)) {
    case CNST+I:
    case CNST+P:
    case CNST+U:
      if (range(p, 0, 0) == 0) {
        setreg(p, ireg[0]);
        p->x.registered = 1;
      }
      break;
    case CALL+I:
    case CALL+P:
    case CALL+U:
    case CALL+F:
      rtarget(p, 0, ireg[25]);
      setreg(p, ireg[2]);
      break;
    case CALL+V:
      rtarget(p, 0, ireg[25]);
      break;
    case RET+I:
    case RET+P:
    case RET+U:
    case RET+F:
      rtarget(p, 0, ireg[2]);
      break;
    case ARG+I:
    case ARG+P:
    case ARG+U:
    case ARG+F:
      ty = optype(p->op);
      q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, opsize(p->op), ty0);
      if (p->x.argno == 0) {
        ty0 = ty;
      }
      if (q) {
        rtarget(p, 0, q);
      }
      break;
    case ASGN+B:
      rtarget(p->kids[1], 0, blkreg);
      break;
    case ARG+B:
      rtarget(p->kids[0], 0, blkreg);
      break;
  }
}


static void clobber(Node p) {
  assert(p);
  switch (specific(p->op)) {
    case CALL+I:
    case CALL+P:
    case CALL+U:
    case CALL+F:
      spill(INTTMP, IREG, p);
      break;
    case CALL+V:
      spill(INTTMP | INTRET, IREG, p);
      break;
  }
}


Interface eco32IR = {
  1, 1, 0,  /* char */
  2, 2, 0,  /* short */
  4, 4, 0,  /* int */
  4, 4, 0,  /* long */
  4, 4, 0,  /* long long */
  4, 4, 1,  /* float */
  4, 4, 1,  /* double */
  4, 4, 1,  /* long double */
  4, 4, 0,  /* T * */
  0, 1, 0,  /* struct */
  0,        /* little_endian */
  0,        /* mulops_calls */
  0,        /* wants_callb */
  1,        /* wants_argb */
  1,        /* left_to_right */
  0,        /* wants_dag */
  0,        /* unsigned_char */
  address,
  blockbeg,
  blockend,
  defaddress,
  defconst,
  defstring,
  defsymbol,
  emit,
  export,
  function,
  gen,
  global,
  import,
  local,
  progbeg,
  progend,
  segment,
  space,
  0, 0, 0, 0, 0, 0, 0,
  {
    1,      /* max_unaligned_load */
    rmap,
    blkfetch, blkstore, blkloop,
    _label,
    _rule,
    _nts,
    _kids,
    _string,
    _templates,
    _isinstruction,
    _ntname,
    emit2,
    doarg,
    target,
    clobber
  }
};
