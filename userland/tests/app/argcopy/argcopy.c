/*
 * argcopy.c -- structure as argument, has to be copied
 */


#include <stdio.h>


typedef struct {
  unsigned char size, align, outofline;
} Metrics;

typedef struct {
  Metrics charmetric;
  Metrics shortmetric;
  Metrics intmetric;
  Metrics longmetric;
  Metrics floatmetric;
  Metrics doublemetric;
} IR;


IR ir;


void test(int op, char *name, Metrics m) {
  unsigned char size, align, outofline;

  size = m.size;
  align = m.align;
  outofline = m.outofline;
  printf("op = %d, name = %s, size = %d, align = %d, outofline = %d\n",
         op, name, size, align, outofline);
}


int main(void) {
  IR *irp;

  irp = &ir;
  irp->charmetric.size = 1;
  irp->charmetric.align = 1;
  irp->charmetric.outofline = 0;
  irp->shortmetric.size = 2;
  irp->shortmetric.align = 2;
  irp->shortmetric.outofline = 0;
  irp->intmetric.size = 4;
  irp->intmetric.align = 4;
  irp->intmetric.outofline = 0;
  irp->longmetric.size = 4;
  irp->longmetric.align = 4;
  irp->longmetric.outofline = 0;
  irp->floatmetric.size = 4;
  irp->floatmetric.align = 4;
  irp->floatmetric.outofline = 1;
  irp->doublemetric.size = 8;
  irp->doublemetric.align = 4;
  irp->doublemetric.outofline = 1;
  test(1, "char", irp->charmetric);
  test(2, "short", irp->shortmetric);
  test(3, "int", irp->intmetric);
  test(4, "long", irp->longmetric);
  test(5, "float", irp->floatmetric);
  test(6, "double", irp->doublemetric);
  return 0;
}
