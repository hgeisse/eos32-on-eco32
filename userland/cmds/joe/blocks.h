/* Memory block functions header file
   Copyright (C) 1991 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License along with
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 675
Mass Ave, Cambridge, MA 02139, USA.  */ 

unsigned char *bset(unsigned char *bk, int sz, unsigned char c);
unsigned char *bmove(unsigned char *dst, unsigned char *src, int sz);
unsigned char *bfwrd(unsigned char *dst, unsigned char *src, int sz);
unsigned char *bbkwd(unsigned char *dst, unsigned char *src, int sz);
unsigned umin(unsigned a, unsigned b);
unsigned umax(unsigned a, unsigned b);
int min(int a, int b);
int max(int a, int b);
int beq(unsigned char *dst, unsigned char *src, int sz);
int bieq(unsigned char *dst, unsigned char *src, int sz);
unsigned char *bchr(unsigned char *bk, int sz, unsigned char c);
unsigned char *brchr(unsigned char *bk, int sz, unsigned char c);
