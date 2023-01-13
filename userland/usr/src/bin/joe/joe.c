/* JOE - Joe's Own Editor - the bulk of the code is here
   Copyright (C) 1991 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with JOE; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <eos32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "joe.h"
#include "buffer.h"
#include "blocks.h"
#include "async.h"

int width=80;
int height=24;
int scroll=1;
int noupdate=0;
int repeatamnt=1;
int nrepeatamnt;

unsigned char stalin[PATHSIZE];

int smode=0;
int tops=0;
int bots;
int oxpos=0;            /* Current cursor position */
int oypos=0;
int *scrn;            /* Screen buffer address */
unsigned char *help=0;
int helplines=0;
int helpsize=0;
int helpblksize=0;

unsigned char *omsg=0;           /* Pointer to opening message */

/**************************************************************/

#define STRLEN(x)	strlen((char *) (x))
#define STRCMP(x, y)	strcmp((char *) (x), (char *) (y))
#define STRCPY(x, y)	strcpy((char *) (x), (char *) (y))
#define STRCAT(x, y)	strcat((char *) (x), (char *) (y))

#define EPUTS(x)	eputs((char *) (x))

int SPRINTF(unsigned char *str, char *fmt, ...) {
  va_list ap;
  int n;

  va_start(ap, fmt);
  vsprintf((char *) str, fmt, ap);
  va_end(ap);
  return n;
}

/**************************************************************/

void dopen(void)
{
int x;
unsigned char buf[30];
scrn=(int *)malloc(width*height*sizeof(int));
for(x=0;x<width*height;x++) scrn[x]= ' ';
if(scroll)
 {
 SPRINTF(buf,"\033[0m\033[1;%dr\033[H\033[J",height), EPUTS(buf);
 bots=height-1;
 }
else EPUTS("\033[0m\033[H\033[J");
}

void dclose(void)
{
setregn(0,height-1);
cpos(height-1,0);
attrib(0);
}

void resize(void)
{
int x;
unsigned char buf[30];
free(scrn);
scrn=(int *)malloc(width*height*sizeof(int));
for(x=0;x<width*height;x++) scrn[x]= ' ';
if(scroll)
 {
 SPRINTF(buf,"\033[0m\033[1;%dr\033[H\033[J",height), EPUTS(buf);
 bots=height-1;
 }
else EPUTS("\033[0m\033[H\033[J");
smode=0;
if(wind) wind=helplines;
if(wind>height-3) wind=height-3;
if(wind) hupd=1;
wfit();
upd=1;
}

void cposs(int y, int x)
{
unsigned char s[9];
if(y>bots || y<tops) setregn(0,height-1);
if(y==oypos)
 {
 if(x==oxpos) return;
 if(x==0)
  {
  eputc(13);
  return;
  }
 if(oxpos>=x+1 && oxpos<=x+4)
  {
  while(oxpos!=x) eputc(8), x++;
  return;
  }
 if(x>=oxpos+1 && x<=oxpos+4)
  {
  while(x!=oxpos) tputcc(scrn[oypos*width+oxpos++]);
  return;
  }
 if(x>oxpos)
  SPRINTF(s,"\033[%dC",x-oxpos);
 else
  SPRINTF(s,"\033[%dD",oxpos-x);
 EPUTS(s);
 return;
 }
if(x==oxpos)
 {
 if(y>=oypos+1 && y<=oypos+4)
  {
  while(y!=oypos) /* acheck(), */ eputc(10), oypos++;
  return;
  }
 if(y==0 && x==0)
  {
  EPUTS("\033[H");
  return;
  }
 if(y>oypos)
  SPRINTF(s,"\033[%dB",y-oypos);
 else
  SPRINTF(s,"\033[%dA",oypos-y);
 EPUTS(s);
 return;
 }
if(x<3 && y>oypos && y<oypos+5)
 {
 while(oypos!=y) ++oypos, eputc('\012');
 eputc('\015'); oxpos=0;
 while(x!=oxpos) tputcc(scrn[oypos*width+oxpos++]);
 return;
 }
if(x==0 && y==0)
 {
 EPUTS("\033[H");
 return;
 }
if(x==0)
 {
 SPRINTF(s,"\033[%dH",y+1);
 EPUTS(s);
 return;
 }
SPRINTF(s,"\033[%d;%dH",y+1,x+1);
EPUTS(s);
return;
}

void cpos(int y, int x)
{
cposs(y,x);
oxpos=x;
oypos=y;
}

void setregn(int top, int bot)
{
unsigned char sst[16];
if(top!=tops || bots!=bot)
 {
 tops=top;
 bots=bot;
 if(scroll)
  {
  oxpos=0;
  oypos=0;
  SPRINTF(sst,"\033[%d;%dr\033[H",top+1,bot+1);
  /* I shouldn't need the \033[H, but so many vt100 emulators forget to home
   * the cursor after a set scrolling region...
   */
  EPUTS(sst);
  }
 }
}

void attrib(int x)
{
if(smode== -1) goto clr;
if(!(x&INVERSE) && (smode&INVERSE)) goto clr;
if(!(x&BLINK) && (smode&BLINK)) goto clr;
if(!(x&UNDERLINE) && (smode&UNDERLINE)) goto clr;
if(!(x&BOLD) && (smode&BOLD)) goto clr;
goto ovr;
clr:
smode=0;
EPUTS("\033[m");
ovr:
if(x&INVERSE && !(smode&INVERSE)) EPUTS("\033[7m");
if(x&BLINK && !(smode&BLINK)) EPUTS("\033[5m");
if(x&UNDERLINE && !(smode&UNDERLINE)) EPUTS("\033[4m");
if(x&BOLD && !(smode&BOLD)) EPUTS("\033[1m");
smode=x;
}

int uuu=0;
int cntr=0;
int upd=1;
int hupd=0;
int newy=1;
int helpon=0;
int wind=0;
int xpos=0;
int ypos=0;
TXTSIZ saddr=0;
TXTSIZ xoffset=0;

/* Clear end of line if needed.  i is row number and j is column number */

void clreolchk(int i, TXTSIZ j)
{
int *k=scrn+i*width;
int t, jj;
if(j<xoffset) jj=0;
else
 if(j>=xoffset+width-1) return;
 else jj=j-xoffset;
for(t=width-1;t>=jj;--t) if(k[t]!=' ') goto ohoh;
return;
ohoh:
if(t==jj)
 {
 cpos(i,jj);
 tputcc(' ');
 k[jj]=' ';
 oxpos++;
 return;
 }
while(t>=jj) k[t--]=' ';
cpos(i,jj);
attrib(0);
EPUTS("\033[K");
}

/* Change buffer character to displayable form */

int showas(int ch)
{
#ifdef NOHIGHBIT
if(ch>=128)
 {
 ch&=127;
 ch|=INVERSE;
 }
#endif
if((ch&127)<32)
 {
 ch+='@';
 ch|=UNDERLINE;
 }
if((ch&127)==127)
 {
 ch&=~127;
 ch|='?'|UNDERLINE;
 }
return ch;
}

int udline(int i)
{
int q=i*width;
TXTSIZ j;
int t;
int u;
int ch;
for(j=0;1;j++)
 {
 if(have) return -1;
 if(fmeof())
  {
  clreolchk(i++,j);
  j=0;
  while(i<curwin->wind+curwin->height) clreolchk(i++,j);
  return 1;
  }
 ch=fmgetc();
 if(ch==NL)
  {
  clreolchk(i,j);
  return 0;
  }
 if(ch==TAB)
  {
  ch=' ';
  if(fmnote()-1>=markb && fmnote()<=marke && curbuf==markbuf) ch^=INVERSE;
  t=i*width+j-xoffset;
  do
   {
   if(j>=xoffset && j<xoffset+width-1)
    {
    u=scrn[t];
    if(ch!=u || u==-1)
     {
     cpos(i,(int)(j-xoffset));
     scrn[t]=ch;
     tputcc(ch);
     oxpos++;
     }
    }
   t++;
   j++;
   } while(j&7);
  j--;
  }
 else
  {
  ch=showas(ch);
  if(fmnote()-1>=markb && fmnote()<=marke && curbuf==markbuf) ch^=INVERSE;
  t=q+j-xoffset;
  if(j>=xoffset && j<xoffset+width-1)
   {
   u=scrn[t];
   if(ch!=u || u==-1)
    {
    cpos(i,(int)(j-xoffset));
    scrn[t]=ch;
    tputcc(ch);
    oxpos++;
    }
   }
  }
 }
 /* HG: never reached */
 return 0;
}

int udscrn(void)
{
int i;
int v;
for(i=ypos;i<curwin->height+curwin->wind;i++)
 if((v=udline(i)) != 0) break;
if(v== -1) return 0;
fmpoint(saddr);
for(i=curwin->wind+1;i<ypos;i++)
 if(udline(i)) return 0;
return 1;
}

void dupdate1(int fgf)
{
int y;
TXTSIZ x;
TXTSIZ sve=fmnote();

TXTSIZ sve1;

/* Status line */
STRCPY(stalin,"\\i");
if(gfnam[0]) STRCAT(stalin,gfnam);
else STRCAT(stalin,"(Unnamed)");
if(changed) STRCAT(stalin," (Modified)");
if(record) STRCAT(stalin," (Macro recording)");
if(!helpon && STRLEN(stalin)+21<width+1)
 {
 int x=STRLEN(stalin);
 while(x<width-20) stalin[x++]=' ';
 stalin[x]=0;
 STRCAT(stalin,"Hit Ctrl-K H for help");
 }
else
 {
 int x=STRLEN(stalin);
 while(x<width+1) stalin[x++]=' ';
 stalin[x]=0;
 }

msgout(curwin->wind,stalin,0,1);

x=getcol();
if(fmnrnl()) fmpoint(fmnote()+1);
sve1=fmnote();

/* calculate what screen cursor position should be */

if(x>xoffset+width-2)
 xpos=width-2, xoffset=x-width+2;
else
 if(x<xoffset)
  xpos=0, xoffset=x;
 else
  xpos=x-xoffset;

/* calculate new y cursor position and point to beginning of screen */

if(newy)
 {
 if(fmnote()<=saddr)
  {
  ypos=curwin->wind+1;
  saddr=fmnote();
  }
 else
  {
  /* is cursor within 24 lines of old beginning of screen */
 
  for(y=0;y!=curwin->height-2;y++)
   {
   if(fmnote()==saddr) goto over;
   fmrgetc();
   if(fmnrnl()) fmpoint(fmnote()+1);
   }
  if(cntr)
   {
   for(x=0;x<(curwin->height-1)/2;x++)
    {
    fmfnl();
    y--;
    fmgetc();
    }
   }
  over:
  cntr=0;
  saddr=fmnote();
  ypos=y+curwin->wind+1;
  }
 newy=0;
 }

/* Now update screen */
if(have)
 { if(fgf) cpos(ypos,xpos); }
else
 {
 fmpoint(sve1);
 if(udscrn()) upd=0;
 if(fgf) cpos(ypos,xpos);
 }
fmpoint(sve);
}

void dupdatehelp(void)
{
int att;
int i,j,c;
unsigned char *from=help;
int *too=scrn;
if(helplines>height-3) wind=height-3;
else wind=helplines;
for(i=0;i!=wind;++too, ++i)
 {
 j=0; att=0;
 loop:
 if(have) return;
 if(j==width-1)
  {
  while(*from!='\n') ++from;
  ++from;
  continue;
  }
 if(*from=='\n')
  {
  hclreol:
  ++from;
  cpos(i,j);
  EPUTS("\033[K");
  while(j!=width-1) *too++ =' ', ++j;
  continue;
  }
 if(*from=='\\')
  {
  ++from;
  if(*from=='\n') goto hclreol;
  if(*from=='u')
   {
   att^=UNDERLINE;
   ++from;
   goto loop;
   }
  if(*from=='i')
   {
   att^=INVERSE;
   ++from;
   goto loop;
   }
  }
 c= *from++ | att;
 if(c!= *too)
  {
  cpos(i,j);
  tputcc(*too= c);
  oxpos++;
  }
 ++too; ++j;
 goto loop;
 }
hupd=0;
}

int updall=0;

void dupdate(void)
{
int xp,yp;

aflush();
if(hupd) dupdatehelp();
if(upd)
 {
 int total=height-wind;
 struct window *x;
 dupdate1(1);
 stwin(curwin);
 x=curwin;
 curwin=topwin;
 xp=xpos; yp=ypos;
 do
  {
  if((curbuf==x->buffer || updall) && curwin!=x)
   {
   if(total<=0) break;
   ldwin(curwin);
   total-=curwin->height;
   newy=1;
   dupdate1(0);
   stwin(curwin);
   }
  else if(curwin==x)
   total-=curwin->height;
  }
  while(curwin=curwin->next,curwin!=topwin);
 updall=0;
 ldwin(x);
 curwin=x;
 cpos(ypos=yp,xpos=xp);
 }
}

void invalidate(int line)
{
int x;
for(x=0;x<width;x++) scrn[width*line+x]= -1;
}

void tputcc(int c)
{
attrib(c);
eputc(c&255);
}

int backup=0;
FILE *handle;
unsigned char gfnam[PATHSIZE];

TXTSIZ bufsiz;		/* Size of buffer */
TXTPTR point;		/* The point */
TXTPTR buffer;		/* The buffer */
TXTPTR filend;		/* First character not in buffer */
TXTPTR hole;		/* Beginning of hole */
TXTPTR ehole;		/* First character not in hole */
int changed=0;		/* Set when file has been changed */
int nundorecs=0;
struct undorec
 {
 struct undorec *next;
 TXTSIZ size;
 TXTSIZ where;
 unsigned char *buffer;
 }
 *undorecs=0;
struct undorec *undoptr=0;
struct undorec *redorecs=0;
int undoflag=1;

void fmopen(void)
{
buffer=(unsigned char *)TXTMALLOC(bufsiz=HOLESIZE);
point=buffer;
hole=buffer;
ehole=buffer+HOLESIZE;
filend=ehole;
changed=0;
undorecs=0;
nundorecs=0;
redorecs=0;
undoptr=0;
}

void fmexpand(unsigned amount)
{
if(filend+amount-buffer>bufsiz)
 {
 unsigned char *old=buffer;
 buffer=(TXTPTR)TXTREALLOC(buffer,bufsiz=(filend+amount+HOLESIZE-buffer));
 point+=buffer-old;
 filend+=buffer-old;
 hole+=buffer-old;
 ehole+=buffer-old;
 }
}

void fmhole(void)
{
if(point==hole) return;
if(point==ehole)
 {
 point=hole;
 return;
 }
if(point<hole)
 {
 bmove(ehole-(hole-point),point,hole-point);
 ehole-=(hole-point);
 hole=point;
 }
else
 {
 bmove(hole,ehole,point-ehole);
 hole+=point-ehole;
 ehole=point;
 point=hole;
 }
}

void fmbig(TXTSIZ size)
{
if(size>fmholesize())
 {
 size+=HOLESIZE;
 fmexpand(size);
 bmove(ehole+size,ehole,filend-ehole);
 ehole+=size;
 filend+=size;
 }
}

int fmfnl(void)
{
while(((point==hole)?(point=ehole):point)!=filend)
 if(*point==NL) return 1;
 else point++;
return 0;
}

int fmrnl(void)
{
if(fmrc()==NL) return 1;
while((point==ehole?point=hole:point)!=buffer)
 if(*(--point)==NL) return 1;
return 0;
}

void killredo(void)
{
struct undorec *u;
while(redorecs)
 {
 u=redorecs->next;
 if(redorecs->buffer) free(redorecs->buffer);
 free(redorecs);
 redorecs=u;
 }
}

void killundo(void)
{
struct undorec *u;
while(undorecs)
 {
 if(undorecs->buffer) free(undorecs->buffer);
 u=undorecs->next;
 free(undorecs);
 undorecs=u;
 }
}

void fminsu(TXTSIZ size)
{
struct window *z;
struct undorec *it;
if(undoflag)
 {
 if(redorecs) killredo();
 if(undorecs && !undorecs->buffer &&
    (undorecs->where==fmnote() || undorecs->where+undorecs->size==fmnote()))
  undorecs->size+=size;
 else
  {
  /* New record */
  it=(struct undorec *)malloc(sizeof(struct undorec));
  it->next=undorecs;
  undorecs=it;
  it->size=size;
  it->where=fmnote();
  it->buffer=0;
  ++nundorecs;
  if(nundorecs==20)
   {
   struct undorec *p;
   for(it=undorecs;it->next;p=it,it=it->next);
   if(it->buffer) free(it->buffer);
   free(it);
   p->next=0;
   }
  }
 }
if(curbuf==markbuf)
 {
 if(fmnote()<markb) markb+=size;
 if(fmnote()<marke) marke+=size;
 }
z=topwin;
do
 {
 if(z->buffer==curbuf)
  {
  if(z==curwin)
   {
   if(fmnote()<saddr) saddr+=size;
   }
  else
   {
   if(fmnote()<z->saddr) z->saddr+=size;
   if(fmnote()<z->cursor) z->cursor+=size;
   }
  }
 z=z->next;
 }
 while(z!=topwin);
}

void undo(void)
{
struct undorec *u;
if(!undorecs) return;
extend=0;
if(!undoptr) undoptr=undorecs;
if(fmnote()==undoptr->where)
 {
 if(undoptr->buffer)
  {
  undoflag=0;
  fminss(undoptr->buffer,undoptr->size);
  undoflag=1;
  markbuf=curbuf;
  markb=fmnote();
  marke=markb+undoptr->size;
  u=(struct undorec *)malloc(sizeof(struct undorec));
  u->next=redorecs;
  redorecs=u;
  u->size=undoptr->size;
  u->buffer=0;
  u->where=fmnote();
  }
 else
  {
  u=(struct undorec *)malloc(sizeof(struct undorec));
  u->next=redorecs;
  redorecs=u;
  u->size=undoptr->size;
  u->buffer=(unsigned char *)malloc(undoptr->size);
  fmcpy(u->buffer,undoptr->size);
  u->where=fmnote();
  undoflag=0;
  fmdel(undoptr->size);
  markb=marke=0;
  undoflag=1;
  }
 u=undoptr->next;
 if(undoptr->buffer) free(undoptr->buffer);
 free(undoptr);
 undoptr=undorecs=u;
 }
else fmpoint(undoptr->where), newy=1;
}

void redo(void)
{
struct undorec *u;
if(!redorecs) return;
extend=0;
fmpoint(redorecs->where), newy=1;
if(redorecs->buffer)
 {
 undoflag=0;
 fminss(redorecs->buffer,redorecs->size);
 undoflag=1;
 markbuf=curbuf;
 markb=fmnote();
 marke=markb+redorecs->size;
 u=(struct undorec *)malloc(sizeof(struct undorec));
 u->next=undorecs;
 undorecs=undoptr=u;
 u->size=redorecs->size;
 u->buffer=0;
 u->where=fmnote();
 }
else
 {
 u=(struct undorec *)malloc(sizeof(struct undorec));
 u->next=undorecs;
 undorecs=undoptr=u;
 u->size=redorecs->size;
 u->buffer=(unsigned char *)malloc(redorecs->size);
 fmcpy(u->buffer,redorecs->size);
 u->where=fmnote();
 undoflag=0;
 fmdel(redorecs->size);
 markb=marke=0;
 undoflag=1;
 }
u=redorecs->next;
if(redorecs->buffer) free(redorecs->buffer);
free(redorecs);
redorecs=u;
}

void fmdelu(TXTSIZ size)
{
struct window *z;
struct undorec *it;
if(undoflag)
 {
 if(redorecs) killredo();
 if(undorecs && undorecs->buffer && (undorecs->where==fmnote()))
  {
  /* Add to end */
  undorecs->buffer=(unsigned char *)realloc(undorecs->buffer,
  undorecs->size+size);
  fmcpy(undorecs->buffer+undorecs->size,size);
  undorecs->size+=size;
  }
 else if(undorecs && undorecs->buffer && (undorecs->where==fmnote()+size))
  {
  /* Add to beginning */
  undorecs->buffer=(unsigned char *)realloc(
  undorecs->buffer,undorecs->size+size);
  bbkwd(undorecs->buffer+size,undorecs->buffer,undorecs->size);
  fmcpy(undorecs->buffer,size);
  undorecs->size+=size;
  undorecs->where-=size;
  }
 else
  {
  /* New record */
  it=(struct undorec *)malloc(sizeof(struct undorec));
  it->next=undorecs;
  undorecs=it;
  it->size=size;
  it->where=fmnote();
  it->buffer=(unsigned char *)malloc(size);
  fmcpy(it->buffer,size);
  ++nundorecs;
  if(nundorecs==20)
   {
   struct undorec *p;
   for(it=undorecs;it->next;p=it,it=it->next);
   if(it->buffer) free(it->buffer);
   free(it);
   p->next=0;
   }
  }
 }
if(markbuf==curbuf)
 {
 if(fmnote()<markb) markb-=umin(size,markb-fmnote());
 if(fmnote()<marke) marke-=umin(size,marke-fmnote());
 }
z=topwin;
do
 {
 if(curbuf==z->buffer)
  {
  if(z==curwin)
   {
   if(fmnote()<saddr) saddr-=umin(size,saddr-fmnote());
   }
  else
   {
   if(fmnote()<z->saddr) z->saddr-=umin(size,z->saddr-fmnote());
   if(fmnote()<z->cursor) z->cursor-=umin(size,z->cursor-fmnote());
   }
  }
 z=z->next;
 }
 while(z!=topwin);
}

void fmdel(TXTSIZ x)
{
fmhole();
fmdelu(x);
ehole+=x;
changed=1;
}

void fminss(unsigned char *string, unsigned size)
{
fminsu(size);
fmhole();
if(size>fmholesize()) fmbig(size);
bmove(hole,string,size);
hole+=size;
changed=1;
}

void fmcpy(unsigned char *string, int size)
{
fmhole();
bbkwd(string,ehole,size);
}

int fmcmp(unsigned char *string, int size)
{
unsigned char *x;
if(point==hole) point=ehole;
if(hole>point && hole<point+size && hole!=ehole)
 {
 if(fmcmp(string,hole-point)) return 1;
 else
  {
  x=point;
  point=ehole;
  if(fmcmp(string+(hole-x),size-(hole-x)))
   {
   point=x;
   return 1;
   }
  else
   {
   point=x;
   return 0;
   }
  }
 }
else
 {
 x=point;
 do
  if(*(x++)!=*(string++)) return 1;
  while(--size);
 return 0;
 }
}

int tupp(unsigned char c)
{
if(c>='a' && c<='z') return c+'A'-'a';
else return c;
}

int fmicmp(unsigned char *string, int size)
{
unsigned char *x;
if(point==hole) point=ehole;
if(hole>point && hole<point+size && hole!=ehole)
 {
 if(fmcmp(string,hole-point)) return 1;
 else
  {
  x=point;
  point=ehole;
  if(fmcmp(string+(hole-x),size-(hole-x)))
   {
   point=x;
   return 1;
   }
  else
   {
   point=x;
   return 0;
   }
  }
 }
else
 {
 x=point;
 do
  if(tupp(*(x++))!=tupp(*(string++))) return 1;
  while(--size);
 return 0;
 }
}

int fmsave(FILE *file, TXTSIZ size)
{
if(!size) return 1;
if(point==hole) point=ehole;
if(hole>point && hole<point+size && hole!=ehole)
 {
 if(hole-point!=fwrite(point,1,hole-point,file)) return 0;
 if(size-(hole-point)!=fwrite(ehole,1,size-(hole-point),file)) return 0;
 return 1;
 }
else
 return size==fwrite(point,1,size,file);
}

int fminsfil(FILE *file)
{
struct stat buf;
TXTSIZ amount;
fstat(fileno(file),&buf);
if(buf.st_size==0) return 1;
fminsu(buf.st_size);
changed=1;
fmhole();
fmbig(buf.st_size);
amount=fread(hole,1,buf.st_size,file);
hole+=amount;
return amount==buf.st_size;
}

/* Output a message string which might possibly longer than width */
/* if flg is set and the string is longer than the line, the cursor is
 * left at the last position on the line.  If it is clear, the cursor may be
 * elsewhere because of screen optimization. */
/* if flg1 is set, attribute escape sequences \i and \u are taken to mean
 * switch to inverse or switch to underline attributes
 */

void msgout(int row, unsigned char *str, int flg, int flg1)
{
int j=0, c, att=0;
int *too=scrn+width*row;
loop:
if(j==width-1)
 {
 if(flg) cpos(row,j);
 return;
 }
if(!*str)
 {
 hclreol:
 cpos(row,j);
 EPUTS("\033[K");
 while(j!=width-1) *too++ =' ', ++j;
 return;
 }
if(*str=='\\' && flg1 && (str[1]=='i' || str[1]=='u'))
 {
 ++str;
 if(!*str) goto hclreol;
 if(*str=='u')
  {
  att^=UNDERLINE;
  ++str;
  goto loop;
  }
 if(*str=='i')
  {
  att^=INVERSE;
  ++str;
  goto loop;
  }
 }
c= showas(*str++) ^ att;
if(c!= *too)
 {
 cpos(row,j);
 tputcc(*too= c);
 oxpos++;
 }
++too; ++j;
goto loop;
}

int getl(char *prompt, unsigned char *dat)
{
int ch,x,y;
int flag=0;
unsigned char buf[PATHSIZE];
STRCPY(buf,prompt);
STRCAT(buf," (^C to abort): ");
y=STRLEN(buf);
STRCAT(buf,dat);
x=STRLEN(buf);
while(1)
 {
 if(x>width-1) msgout(height-1,buf+x-(width-1),1,0);
 else msgout(height-1,buf,1,0);
 ch=anext();
 if(ch=='`' && !flag)
  {
  flag=1;
  continue;
  }
 if((ch>=32 && ch!=127) || flag)
  {
  if(flag && ch=='?') ch=127;
  else if(flag && ch!='`') ch&=0x1f;
  flag=0;
  buf[x+1]=0, dat[x+1-y]=0;
  buf[x]=ch, dat[x++-y]=ch;
  continue;
  }
 if(ch=='L'-'@')
  {
  ch= -1;
  break;
  }
 if(ch==13 || ch==10)
  {
  ch=1;
  break;
  }
 if((ch==8 || ch==127) && x-y)
  {
  x--;
  dat[x-y]=0;
  buf[x]=0;
  continue;
  }
/*
 if(ch==9)
  {
  dat[x-y]=0;
  docomplete(dat);
  STRCPY(buf+y,dat);
  x=STRLEN(buf);
  continue;
  }
*/
 if(ch==3)
  {
  ch=0;
  break;
  }
 }
return ch;
}

void msg(unsigned char *ms)
{
msgout(height-1,ms,1,1);
anext();
}

int askyn(char *ms)
{
int ch;
msgout(height-1,(unsigned char *)ms,1,1);
up:
ch=anext();
switch(ch)
 {
case 'y':
case 'n':
 eputc(ch);
 ch&=0x5f;
 break;
case 'Y':
case 'N':
 eputc(ch);
 break;
case 3:
 ch= -1;
 break;
default:
 goto up;
 }
return ch;
}

int query(unsigned char *ms)
{
msgout(height-1,ms,1,1);
return anext();
}

int nquery(char *ms)
{
msgout(height-1,(unsigned char *)ms,1,1);
cpos(ypos,xpos);
return anext();
}

void imsg(void)
{
attrib(0);
if(omsg) msgout(1,omsg,0,1);
upd=1;
msgout(height-1,
       (unsigned char *)"\\i** Joe's Own Editor version 0.1.5 (1991) **\\i",
       0,1);
cpos(1,0);
}

int pic;
int autoind;
int overwrite;
int wrap;
int tabmagic;
TXTSIZ rmargin;

int options=0;
unsigned char sstring[PATHSIZE];
unsigned char rstring[PATHSIZE];
int len;

TXTSIZ markb=0;
TXTSIZ marke=0;

TXTSIZ added;
TXTSIZ extend;
int leave;       /* set if editor should now exit */

TXTSIZ getrcol(void)
{
TXTSIZ x,y;
unsigned char ch;
x=fmnote();
if(fmnrnl()) fmgetc();
y=0;
while(fmnote()!=x)
 {
 ch=fmgetc();
 if(ch==TAB)
  while((++y)&7);
 else
  y++;
 }
return y;
}

void gocol(TXTSIZ x)
{
TXTSIZ y;
int ch;
if(fmnrnl()) fmgetc();
extend=0;
for(y=0;y!=x;y++)
 {
 if(fmeof()) goto dn;
 ch=fmgetc();
 if(ch==NL)
  {
  fmpoint(fmnote()-1);
  extend=x;
  return;
  }
 if(ch==TAB)
  {
  while((++y)&7)
   {
   if(y==x)
    {
    fmpoint(fmnote()-1);
dn:
    extend=x;
    return;
    }
   }
  y--;
  }
 }
}

TXTSIZ calcs(void)
{
TXTSIZ y=0;
if(fmnrnl()) fmgetc();
extend=0;
while(! (fmeof()?1:fmrc()==NL))
 if(fmrc()==' ')
  {
  ++y;
  fmgetc();
  }
 else if(fmrc()==TAB)
  {
  do ++y; while(y%TABWIDTH);
  fmgetc();
  }
 else break;
return y;
}

void unfill(void)
{
fmfnl();
extend=0;
while(fmnote())
 {
 unsigned char x=fmrgetc();
 if(x==' ' || x==TAB) fmdel(1);
 else
  {
  fmgetc();
  break;
  }
 }
}

/* Fill from end of line to extend position */

void fillup(void)
{
TXTSIZ x;
if(extend && pic)
 {
 x=getrcol();
 while(extend>x)
  {
  fminsc(' ');
  fmgetc();
  ++x;
  }
 }
extend=0;
}

/* Save current buffer in named file.  Returns 0 on error.  Clears 'changed'
 * variable if sucessfull
 */

int saveit1(unsigned char *tos)
{
unsigned char sting[PATHSIZE];
TXTSIZ temp=fmnote();
fmpoint(0);
handle=fopen((char *)tos,"w+");
if(handle)
 {
 if(!fmsave(handle,fmsize()))
  {
  SPRINTF(sting,"\\iError writing to file %s\\i",tos);
  msg(sting);
  fmpoint(temp);
  return(0);
  }
 fmpoint(temp);
 if(fclose(handle)==EOF)
  {
  SPRINTF(sting,"\\iError closing file %s\\i",tos);
  msg(sting);
  fmpoint(temp);
  return(0);
  }
 changed=0;
 curbuf->changed=0;
 return(1);
 }
else
 {
 SPRINTF(sting,"\\iError opening file %s\\i",tos);
 msg(sting);
 fmpoint(temp);
 return(0);
 }
}

void rewrite(void)
{
unsigned char s[25];
int *t,c;
oxpos= 0;
oypos= 0;
tops= 0;
bots= height-1;
smode=0;
if(scroll) SPRINTF(s,"\033[m\033[1;%dr\033[H\033[J",height);
else SPRINTF(s,"\033[m\033[H\033[J");
EPUTS(s);
t=scrn;
c=width*height;
do *(t++)= ' '; while(--c);
upd=1;
newy=1;
updall=1;
if(helpon) hupd=1;
}

/* Toggle help text */

void thelp(void)
{
struct window *x;
newy=1;
upd=1;
if(helpon)
 {
 x=topwin;
 do
  {
  if(x->hheight) x->height=x->hheight;
  else x->height*=height, x->height/=height-wind;
  x=x->next;
  }
  while(x!=topwin);
 wind=0, hupd=0;
 }
else
 {
 if(helplines>height-3) wind=height-3;
 else wind=helplines;
 hupd=1;
 x=topwin;
 do
  {
  x->hheight=x->height;
  x->height*=height-wind;
  x->height/=height;
  x=x->next;
  }
  while(x!=topwin);
 }
helpon= !helpon;
wfit();
}

/* Move cursor to beginning of file */

void bof(void)
{
extend=0;
fmpoint(0);
newy=1;
}

/* Move cursor to beginning of line */

void bol(void)
{
if(fmnrnl()) fmgetc();
extend=0;
}

/* Move cursor to end of line */

void eol(void)
{
extend=0;
fmfnl();
}

/* Move cursor to end of file */

void eof(void)
{
extend=0;
fmpoint(fmsize());
newy=1;
}

/* Move cursor right */

void urtarw(void)
{
fillup();
extend=0;
if(fmeof())
 {
 if(pic)
  {
  into:
  fminsc(' ');
  fmgetc();
  }
 return;
 }
else if(fmrc()==NL)
 {
 if(pic) goto into;
 bol();
 udnarw();
 return;
 }
fmgetc();
}

void rtarw(void)
{
fillup();
extend=0;
if(fmeof())
 {
 if(pic)
  {
  into:
  fminsc(' ');
  fmgetc();
  }
 return;
 }
else if(fmrc()==NL)
 {
 if(pic) goto into;
 newy=1;
 }
fmgetc();
}

void ultarw(void)
{
if(extend)
 {
 extend=0;
 return;
 }
fillup();
if(fmnote())
 {
 fmpoint(fmnote()-1);
 if(fmrc()==NL)
  {
  fmgetc();
  uuparw();
  eol();
  }
 }
}

void ltarw(void)
{
if(extend)
 {
 extend=0;
 return;
 }
fillup();
if(fmnote())
 fmpoint(fmnote()-1);
if(fmrc()==NL) newy=1;
}

/* Move cursor up */

void uparw(void)
{
TXTSIZ x;
x=getcol();
bol();
if(fmnote())
 {
 fmpoint(fmnote()-1);
 if(fmnrnl())
  fmgetc();
 }
gocol(x);
newy=1;
}

/* user's cursor up routine (uses scrolling regions) */

void uuparw(void)
{
TXTSIZ sve=fmnote();
int y=(curwin->wind+1)*width;
int x;
if(scroll)
 {
 if(fmnrnl())
  {
  if(fmnote()+1==saddr)
   {
   if(fmnrnl()) fmgetc();
   saddr=fmnote();
   setregn(curwin->wind+1,curwin->wind+(curwin->height-1));
   cpos(curwin->wind+1,oxpos);
   attrib(0);
   EPUTS("\033M");
   for(x=(curwin->wind+curwin->height)*width-1;x>=y+width;x--)
    scrn[x]=scrn[x-width];
   for(x=y;x<y+width;x++) scrn[x]= ' ';
   }
  fmpoint(sve);
  }
 else
  fmpoint(sve);
 }
uparw();
}

/* Move cursor down */

void dnarw(void)
{
TXTSIZ x;
newy=1;
x=getcol();
if(!fmfnl())
 bol();
else
 fmgetc();
gocol(x);
}

/* user's down arrow function */

void udnarw(void)
{
TXTSIZ sve=fmnote();
int x;
if(!fmfnl())
 {
 if(pic)
  {
  fminsc(NL);
  fmpoint(sve);
  udnarw();
  return;
  }
 else
  {
  goto cant;
  }
 }
if(scroll)
 {
 if(ypos!=curwin->height+curwin->wind-1) goto cant;
 for(x=0;x!=curwin->height-2;x++) fmnrnl();
 fmfnl();
 fmgetc();
 saddr=fmnote();
 setregn(curwin->wind+1,curwin->wind+curwin->height-1);
 cpos((curwin->wind+curwin->height-1),oxpos);
 attrib(0);
 eputc(10);
 for(x=(curwin->wind+1)*width;x!=(curwin->wind+curwin->height-1)*width;x++)
  scrn[x]=scrn[x+width];
 for(x=(curwin->wind+curwin->height-1)*width;
     x!=(curwin->wind+curwin->height)*width;x++)
  scrn[x]= ' ';
 }
cant:
fmpoint(sve);
dnarw();
}

/* Magic Tabs (tm) */

TXTSIZ tabcol;	/* Original column of text preceeded by tab stops */

void tabmark(void)
{
TXTSIZ cur=fmnote();
unsigned char c;
tabcol=0;
if(!tabmagic) return;
while(!fmeof())
 {
 c=fmgetc();
 if(c=='\t')
  {
  while(!fmeof())
   {
   c=fmgetc();
   if(c=='\n') break;
   if(c!='\t')
    {
    fmrgetc();
    tabcol=getrcol();
    break;
    }
   }
  fmpoint(cur); return;
  }
 if(c=='\n') break;
 }
fmpoint(cur); return;
}

void tabfix(void)
{
TXTSIZ cur=fmnote(),newcol;
unsigned char c;
if(!tabcol) return;
while(!fmeof())
 {
 c=fmgetc();
 if(c=='\t')
  {
  while(!fmeof())
   {
   c=fmgetc();
   if(c=='\n') break;
   if(c!='\t')
    {
    fmrgetc();
    newcol=getrcol();
    while(newcol<tabcol)
     {
     fminsc('\t');
     newcol+=8;
     }
    fmrgetc();
    while(newcol>tabcol)
     {
     if(fmrgetc()=='\t')
      {
      fmdel(1);
      newcol-=8;
      }
     else break;
     }
    break;
    }
   }
  fmpoint(cur); return;
  }
 if(c=='\n') break;
 }
fmpoint(cur); return;
}

/* Delete character under cursor */

void delch(void)
{
unsigned char c;
int x;
if(extend && pic) return;
if(extend)
 {
 extend=0;
 return;
 }
if(!fmeof())
 {
 if((c=fmrc())==NL && scroll)
  {
  if(ypos<curwin->wind+curwin->height-2)
   {
   for(x=(ypos+1)*width;x<width*(curwin->wind+curwin->height-1);x++)
    scrn[x]=scrn[x+width];
   for(x=(curwin->wind+curwin->height-1)*width;
   x<(curwin->wind+curwin->height)*width;x++) scrn[x]= ' ';
   setregn(ypos+1,(curwin->wind+curwin->height-1));
   cpos((curwin->wind+curwin->height-1),oxpos);
   attrib(0);
   eputc(10);
   }
  fmdel(1);
  }
 else if(c==TAB) fmdel(1);
 else
  {
  tabmark();
  fmdel(1);
  tabfix();
  }
 }
}

void type(unsigned char ch)
{
int ox=oxpos;
int x,y;
TXTSIZ temp, temp1;
int eflag=0;
if(quote8th)
 {
 quote8th=0;
 ch|=128;
 }
ypos=oypos;
if(extend)
 {
 if(ch!=NL) fillup();
 else extend=0;
 eflag=1;
 }
if(ch==NL)
 {
 if(overwrite && !tabmagic && !fmeof()) fmdel(1);
 fminsc(ch);
 fmgetc();
 newy=1;
 if(ypos!=(curwin->wind+curwin->height-1))
  {
  if(!fmeof())
   {
   if(ypos<curwin->wind+curwin->height-2 && scroll)
    {
    setregn(ypos+1,(curwin->wind+curwin->height-1));
    cpos(ypos+1,oxpos);
    attrib(0);
    EPUTS("\033M");
    cpos(ypos+1,0);
    for(x=(curwin->wind+curwin->height)*width-1;x>=(ypos+2)*width;x--)
     scrn[x]=scrn[x-width];
    for(x=(ypos+1)*width;x<(ypos+2)*width;x++) scrn[x]=' ';
    }
   else cpos(ypos+1,0);
   }
  else
   cpos(ypos+1,0);
  }
 else if(scroll)
   {
   setregn(curwin->wind+1,(curwin->wind+curwin->height-1));
   cpos((curwin->height+curwin->wind-1),0);
   attrib(0);
   eputc(10);
   for(x=curwin->wind*width;x<(curwin->wind+curwin->height-1)*width;x++)
    scrn[x]=scrn[x+width];
   for(x=(curwin->wind+curwin->height-1)*width;
       x<(curwin->wind+curwin->height)*width;x++) scrn[x]= ' ';
   temp=fmnote();
   fmpoint(saddr);
   fmfnl();
   fmgetc();
   saddr=fmnote();
   fmpoint(temp);
   }
 if(ox<(width-2) && (fmeof()) && scroll) uuu=1;
 if(autoind)
  {
  temp=fmnote();
  uparw();
  for(x=0;1;x++)
   {
   ch=fmgetc();
   if(!(ch==' ' || ch==TAB)) break;
   temp1=fmnote();
   fmpoint(temp);
   fminsc(ch);
   uuu=0;
   added++;
   fmpoint(temp1);
   temp++;
   }
  fmpoint(fmnote()-(x+1));
  dnarw();
  y=overwrite, overwrite=0;
  for(;x;x--) rtarw();
  overwrite=y;
  }
 }
else
 {
 if(overwrite)
  {
  if(!tabmagic)
   {
   if(!fmeof())
    {
    unsigned char c=fmrc();
    fmdel(1);
    if(ch!=TAB && c!=TAB && c!=NL && ox<(width-2)) uuu=1;
    }
   else if(ch!=TAB && ox<(width-2)) uuu=1;
   }
  else
   if(fmrc()!=NL && !fmeof())
    if(ch==TAB && fmrc()!=TAB)
     {
     TXTSIZ foo=getrcol();
     do
      {
      if(fmeof()) break;
      if(fmrc()==NL) break;
      if(fmrc()==TAB)
       {
       fmdel(1);
       break;
       }
      else fmdel(1);
      ++foo;
      }
      while(foo&7);
     }
    else if(ch!=TAB && fmrc()==TAB)
     {
     TXTSIZ tt;
     tabmark();
     if((tt=tabcol) != 0)
      {
      fminsc(ch);
      tabmark();
      fmdel(1);
      if(tabcol!=tt) fmdel(1);
      }
     }
    else
     {
     fmdel(1);
     if(ch!=TAB && ox<(width-2)) uuu=1;
     }
   else if(ox<(width-2) && ch!=TAB) uuu=1;
  }
 if(wrap)
  {
  unsigned char xx;
  if(getrcol()<rmargin) goto skip;
  if(ch==' ')
   fminsc(' '), rtarw();
  else
   {
   temp=fmnote();
   while(1)
    {
    if(fmnote())
     {
     fmpoint(fmnote()-1);
     xx=fmrc();
     if(xx==NL) break;
     if(xx==' ' || x==TAB)
      {
      fmdel(1);
      added=0;
      type(NL);
      temp+=added;
      break;
      }
     }
    else break;
    }
   fmpoint(temp);
   fminsc(ch);
   rtarw();
   uuu=0;
   }
  }
 else
  {
skip:
  if(overwrite || ch==TAB) fminsc(ch);
  else
   {
   tabmark();
   fminsc(ch);
   tabfix();
   }
  if(ch!=TAB && ch!=NL)
   {
   int cc=showas(ch);
   if(fmnote()>=markb && fmnote()<marke) cc^=INVERSE;
   fmgetc();
   tputcc(cc);
   scrn[ypos*width+oxpos]=cc;
   oxpos++;
   if(fmeof()) { if(!eflag && ox<width-2) uuu=1; }
   else if(fmrc()==NL && !eflag && ox<width-2) uuu=1;
   }
  else fmgetc();
  }
 }
}

void itype(unsigned char ch)
{
int x,y;
TXTSIZ temp,temp1;
if(extend)
 {
 if(ch!= NL) fillup();
 else extend=0;
 }
if(ch==NL)
 {
 fminsc(ch);
 fmgetc();
 newy=1;
 if(autoind)
  {
  temp=fmnote();
  uparw();
  for(x=0;1;x++)
   {
   ch=fmgetc();
   if(!(ch==' ' || ch==TAB)) break;
   temp1=fmnote();
   fmpoint(temp);
   fminsc(ch);
   added++;
   fmpoint(temp1);
   temp++;
   }
  fmpoint(fmnote()-(x+1));
  dnarw();
  y=overwrite, overwrite=0;
  for(;x;x--) rtarw();
  overwrite=y;
  }
 }
else
 {
 if(overwrite)
  if(!fmeof()) fmdel(1);
 if(wrap)
  {
  if(getrcol()<rmargin) goto skip;
  if(ch==' ')
   fminsc(' '), rtarw();
  else
   {
   temp=fmnote();
   while(1)
    {
    if(fmnote())
     {
     fmpoint(fmnote()-1);
     x=fmrc();
     if(x==NL) break;
     if(x==' ' || x==TAB)
      {
      fmdel(1);
      added=0;
      itype(NL);
      temp+=added;
      break;
      }
     }
    else break;
    }
   fmpoint(temp);
   fminsc(ch);
   rtarw();
   }
  }
 else
  {
skip:
  fminsc(ch);
  rtarw();
  }
 }
}

/* Insert space */

void inss(void)
{
int t=overwrite;
if(extend)
 {
 extend=0;
 return;
 }
overwrite=0;
type(' ');
ltarw();
overwrite=t;
}

/* Deleting backspace */

void backs(void)
{
int flag=0,c;
if(extend)
 {
 extend=0;
 return;
 }
if(fmeof()) c=1;
else if(fmrc()==NL) c=1;
if(fmnote())
 {
 ultarw();
 if(fmrc()==TAB) flag=1;
 if(overwrite && !tabmagic)
  {
  itype(' ');
  ultarw();
  }
 else if(overwrite && tabmagic)
  {
  if(c) delch();
  else if(!flag)
   {
   itype(' ');
   ltarw();
   }
  }
 else delch();
 if(oxpos && !flag)
  {
  eputc(8), tputcc(32), eputc(8), oxpos--,scrn[oypos*width+oxpos]=32;
  if(fmeof()) uuu=1;
  else if(fmrc()==NL || overwrite) uuu=1;
  }
 }
}

/* quit: exit without saving */

void eexit(void)
{
int c;
if(curwin->next==curwin)
 {
 if(changed)
  {
  c=askyn("Do you really want to throw away this file?"); 
  if(c=='N') return;
  if(c== -1) return;
  leave=1;
  dclose();
  EPUTS("\nFile not saved.\r\n");
  }
 else
  {
  leave=1;
  dclose();
  EPUTS("\nFile not changed so no update needed\r\n");
  }
 }
else
 {
 struct window *t=curwin;
 if(changed && curbuf->count==1)
  {
  c=askyn("Do you really want to throw away this file?");
  if(c=='N') return;
  if(c== -1) return;
  }
 if(curbuf->count==1)
  {
  killundo();
  killredo();
  free(curbuf->buf), free(curbuf);
  if(curbuf==markbuf) markbuf=0;
  }                           
 else curbuf->count--;
 curwin->next->prev=curwin->prev;
 curwin->prev->next=curwin->next;
 curwin=curwin->prev;
 free(t);
 ldwin(curwin);
 if(topwin==t) topwin=curwin;
 wfit();
 }
}

void pgup(void)
{
int nlins=curwin->height-1;
int hlins=nlins/2;
int x,y;
TXTSIZ curpos,z;
if(!hlins) hlins=1;
z=getcol();
curpos=fmnote();
fmpoint(saddr);
for(x=0;x<hlins;x++)
 {
 if(!fmnrnl())
  {
  if(!x)
   {
   gocol(z);
   newy=1;
   return;
   }
  else
   break;
  }
 }
if(fmnrnl()) fmgetc();
saddr=fmnote();
fmpoint(curpos);
setregn(curwin->wind+1,(curwin->wind+curwin->height-1));
cpos(curwin->wind+1,oxpos);
attrib(0);
for(y=0;y<x;y++)
 {
 if(scroll) EPUTS("\033M");
 fmnrnl();
 }
if(fmnrnl()) fmgetc();
cpos(oypos,oxpos);
gocol(z);
x*=width;
if(scroll) for(y=(curwin->wind+1)*width;y<x+(curwin->wind+1)*width;y++)
 {
 scrn[y+x]=scrn[y];
 scrn[y]= ' ';
 }
}

void pgdn(void)
{
int nlins=curwin->height-1;
int hlins=nlins/2;
TXTSIZ curpos,z;
int x,y;
z=getcol();
curpos=fmnote();
x=nlins;
fmpoint(saddr);
do
 {
 if(fmfnl()) fmgetc();
 else
  {
  newy=1;
  gocol(z);
  return;
  }
 }
 while(--x);
for(x=1;x<hlins;x++)
 {
 if(fmfnl()) fmgetc();
 else break;
 }

fmpoint(saddr);
for(y=0;y<x;y++)
 {
 fmfnl();
 fmgetc();
 }
saddr=fmnote();

setregn(curwin->wind+1,(curwin->wind+curwin->height-1));
cpos((curwin->wind+curwin->height-1),oxpos);
fmpoint(curpos);
attrib(0);
for(y=0;y<x;y++)
 {
 fmfnl();
 fmgetc();
 if(scroll) eputc(10);
 }

gocol(z);
cpos(ypos,xpos);
if(scroll)
 {
 y=width*x;
 for(curpos=(curwin->wind+1)*width+y;curpos<(curwin->wind+curwin->height)*
     width;curpos++)
  scrn[curpos-y]=scrn[curpos];
 for(curpos=(curwin->wind+curwin->height)*width-width*x;
     curpos<(curwin->wind+curwin->height)*width;curpos++)
  scrn[curpos]= ' ';
 }
}

void deleol(void)
{
TXTSIZ temp=fmnote();
TXTSIZ temp1;
if(extend && pic) return;
extend=0;
fmfnl();
temp1=fmnote()-temp;
fmpoint(temp);
if(temp1) fmdel(temp1);
}

void killlin(void)
{
extend=0;
if(!fmeof())
 {
 if(fmrc()==NL) delch();
 else deleol();
 }
}

void dellin(void)
{
bol();
deleol();
delch();
}

void fixpath(unsigned char *s)
{
unsigned char tmp[PATHSIZE], *p, c;
struct passwd *passwd;
if(*s=='~')
 {
 p=s+1;
 while(*p!='/' && *p) ++p;
 if((c=*p) != '\0')
  {
  *p=0;
  if(!s[1])
   {
   *p=c;
   STRCPY(tmp,getenv("HOME"));
   STRCAT(tmp,p);
   STRCPY(s,tmp);
   }
  else if((passwd=getpwnam((char *)(s+1))) != NULL)
   {
   *p=c;
   STRCPY(tmp,passwd->pw_dir);
   STRCAT(tmp,p);
   STRCPY(s,tmp);
   }
  }
 }
}

/*

struct list
 {
 struct list *next;
 unsigned char *name;
 };

unsigned char *complete(struct list *list, unsigned char *name)
{
struct list *found=0;
int x;
while(list)
 {
 for(x=0;name[x] && list->name[x];++x) if(name[x]!=list->name[x]) goto next;
 if(found) return 0;
 found=list;
 next:
 list=list->next;
 }
if(found) return found->name;
else return 0;
}

struct list *getnames(unsigned char *name)
{
DIR *dir=opendir(name);
struct direct *dirent;
struct list *first=0, *next=0;
if(dir)
 {
 while(dirent=readdir(dir))
  {
  if(next) next=next->next=malloc(sizeof(struct list));
  else first=next=malloc(sizeof(struct list));
  next->next=0;
  next->name=strdupp(dirent->d_name);
  }
 closedir(dir);
 }
return first;
}

void rmlist(struct list *list)
{
struct list *nxt;
if(list)
 do
  nxt=list->next, free(list);
  while(list=nxt);
}

void docomplete(unsigned char *s)
{
struct list *list=getnames(".");
unsigned char *name;
if(!list) return;
name=complete(list,s);
if(name) STRCPY(s,name);
else eputc(7);
rmlist(list);
}

*/

void exsave(void)
{
unsigned char sting[PATHSIZE];
if(!changed)
 {
 eexit();
 return;
 }
if(gfnam[0]==0)
 {
 if(!getl("Save file",gfnam))
  return;
 fixpath(gfnam);
 }
else if(!backup)
 {
 SPRINTF(sting,"/bin/cp %s %s~",gfnam,gfnam);
 cpos(height-2,0);
 system((char *)sting);
 cpos(ypos,xpos);
 }
if(saveit1(gfnam))
 {
 SPRINTF(sting,"\nFile %s saved.\r\n",gfnam);
 if(curwin->next==curwin)
  {
  leave=1;
  dclose();
  EPUTS(sting);
  }
 else
  eexit();
 }
}

void saveit(void)
{
unsigned char gfnam1[PATHSIZE];
unsigned char sting[PATHSIZE];
STRCPY(gfnam1,gfnam);
if(!getl("Save file",gfnam1))
 return;
fixpath(gfnam1);
if(!gfnam1[0]) return;
if(!backup && !STRCMP(gfnam1,gfnam))
 {
 SPRINTF(sting,"/bin/cp %s %s~",gfnam,gfnam);
 cpos(height-2,0);
 system((char *)sting);
 cpos(ypos,xpos);
 }
saveit1(gfnam1);
}

void findline(void)
{
unsigned char sting[PATHSIZE];
TXTSIZ x;
sting[0]=0;
if(!getl("Goto line",sting))
 return;
x=atol((char *)sting);
if(!x)
 {
 msg((unsigned char *)"\\iBad line number\\i");
 return;
 }
x--;
bof();
for(;x;x--)
 {
 if(!fmfnl()) break;
 fmgetc();
 }
newy=1;
cntr=1;
return;
}

void repeat(void)
{
unsigned char sting[PATHSIZE];
TXTSIZ x;
sting[0]=0;
if(!getl("Repeat",sting))
 return;
x=atol((char *)sting);
nrepeatamnt=x;
}

int search(void)
{
if(options&s_backwards)
 {
 while(fmnote())
  {
  fmrgetc();
  if(options&s_ignore) { if(!fmicmp(sstring,len)) return 1; }
  else if(!fmcmp(sstring,len)) return 1;
  }
 return 0;
 }
else
 {
 while(fmnote()+len<=fmsize())
  {
  if(!(options&s_ignore)) { if(!fmcmp(sstring,len)) return 1; }
  else if(!fmicmp(sstring,len)) return 1;
  fmgetc();
  }
 return 0;
 }
}

void find(int c)
{
int x;
int opts=0;
int n=0;
int rest=0;
int rlen;
TXTSIZ p;
unsigned char ss[80];
extend=0;
if(c=='L'-'@' && sstring[0]) goto srch;
ss[0]=0;
if(!(x=getl("Search string",ss))) return;
if(x== -1)
 {
 if(ss[0])
  STRCPY(sstring,ss);
 goto srch;
 }
if(!ss[0]) return;
STRCPY(sstring,ss);
ss[0]=0;
if(!getl("(I)gnore case (B)ackwards (R)eplace n",ss)) return;
for(x=0;ss[x];x++)
 {
 if(ss[x]=='i' || ss[x]=='I') opts|=s_ignore;
 if(ss[x]=='b' || ss[x]=='B') opts|=s_backwards;
 if(ss[x]=='r' || ss[x]=='R') opts|=s_replace;
 if(ss[x]=='x' || ss[x]=='X') opts|=s_regex;
 if(ss[x]>='0' && ss[x]<='9') n*=10, n+=ss[x]-'0';
 }
options=opts;
if(options&s_replace)
 {
 ss[0]=0;
 if(!(x=getl("Replace with",ss))) return;
 if(x!= -1)
  STRCPY(rstring,ss);
 }
srch:
if(!sstring[0]) return;
len=STRLEN(sstring);
rlen=STRLEN(rstring);
rpt:
p=fmnote();
if(search())
 {
 if(!(options&s_backwards)) fmpoint(fmnote()+len);
 if(options&s_replace)
  {
  if(rest) goto dn;
  newy=1;
  upd=1;
  cntr=1;
  extend=0;
  dupdate();
again:
  x=nquery(
  "Replace? (Yes, No, ^C to abort or R to replace rest without asking)");
  if(x=='n' || x=='N') goto rpt;
  if(x== 3) return;
  if(x=='y' || x=='Y') goto dn;
  if(x=='r' || x=='R')
   {
   rest=1;
   goto dn;
   }
  goto again;
dn:
  if(options&s_backwards)
   {
   fmdel(len);
   fminss(rstring,rlen);
   }
  else
   {
   fmpoint(fmnote()-len);
   fmdel(len);
   fminss(rstring,rlen);
   fmpoint(fmnote()+rlen);
   }
  if(n)
   {
   if(n==1) goto exi;
   else n--;
   }
  goto rpt;
  }
 else if(n)
  {
  if(n==1) goto exi;
  n--;
  goto rpt;
  }
 }
else
 {
 if(!(options&s_replace) || n>1)
  msg((unsigned char *)"Not found");
 fmpoint(p);
 return;
 }
exi:
cntr=1;
newy=1;
}

void findnext(void)
{
find('L'-'@');
}

void findfirst(void)
{
find(0);
}

struct buffer *markbuf;

void setbeg(void)
{
markb=fmnote();
if(markbuf!=curbuf)
 {
 markbuf=curbuf;
 marke=0;
 }
}

void setend(void)
{
marke=fmnote();
if(markbuf!=curbuf)
 {
 markbuf=curbuf;
 markb=0;
 }
}

void writeblk(void)
{
unsigned char gfnam1[PATHSIZE];
unsigned char sting[PATHSIZE];
TXTSIZ sv=fmnote();
struct buffer *bt=curbuf;
if(markbuf)
 {
 stbuf(curbuf);
 ldbuf(markbuf);
 }
if(markb>=marke || marke>fmsize() || !markbuf)
 {
 msg((unsigned char *)"\\iNo block\\i");
 if(markbuf)
  ldbuf(bt);
 return;
 }
gfnam1[0]=0;
if(!getl("File to write block to",gfnam1))
 {
 ldbuf(bt);
 return;
 }
fixpath(gfnam1);
handle=fopen((char *)gfnam1,"w+");
if(handle)
 {
 fmpoint(markb);
 if(!fmsave(handle,marke-markb))
  {
  SPRINTF(sting,"\\iError writting to file %s\\i",gfnam1);
  msg(sting);
  }
 stbuf(markbuf);
 ldbuf(bt);
 fmpoint(sv);
 fclose(handle);
 }
else
 {
 ldbuf(bt);
 SPRINTF(sting,"\\iError opening file %s\\i",gfnam1);
 msg(sting);
 }
}

void cmdblk(void)
{
unsigned char ch;
int fr[2];
int fw[2];
unsigned char gfnam1[PATHSIZE];
TXTSIZ sv=fmnote(), sz;
struct buffer *bt=curbuf;
if(markbuf)
 {
 stbuf(curbuf);
 ldbuf(markbuf);
 }
if(markb>=marke || marke>fmsize() || !markbuf)
 {
 marke=markb=0;
 markbuf=bt;
 }
gfnam1[0]=0;
if(markb==marke)
 {
 if(!getl("Command to capture:",gfnam1))
  {
  ldbuf(bt);
  return;
  }
 }
else if(!getl("Command to pipe block through:",gfnam1))
 {
 ldbuf(bt);
 return;
 }
fixpath(gfnam1);
pipe(fr);
pipe(fw);
dclose();
eputc('\n');
aclose();
if(!fork())
 {
 signorm();
 printf("%s\n",gfnam1);
 if(markb!=marke) close(0);
 close(1);
 if(markb!=marke) dup(fw[0]);
 dup(fr[1]);
 close(fw[0]);
 close(fr[1]);
 close(fw[1]);
 close(fr[0]);
 execl("/bin/sh","/bin/sh","-c",gfnam1,NULL);
 exit(0);
 }
close(fr[1]);
close(fw[0]);
if(fork())
 {
 if(bt==markbuf) if(sv>=markb && sv<marke) sv=markb;
 sz=marke-markb;
 fmpoint(markb);
 if(sz) fmdel(sz);
 if(bt==markbuf) if(sv>markb) sv-=sz;
 close(fw[1]);
 stbuf(markbuf);
 ldbuf(bt);
 fmpoint(sv);
 markbuf=bt;
 markb=sv;
 while(1==read(fr[0],&ch,1)) putchar(ch), fminsc(ch), fmgetc();
 fflush(stdout);
 close(fr[0]);
 marke=fmnote();
 fmpoint(markb);
 wait(0);
 wait(0);
 }
else
 {
 fmpoint(markb);
 while(fmnote()!=marke)
  {
  ch=fmgetc();
  write(fw[1],&ch,1);
  }
 close(fw[1]);
 _exit(-1);
 }
aopen();
rewrite();
updall=1;
newy=1;
}

void delblk(void)
{
struct buffer *bt=curbuf;
TXTSIZ x=fmnote();
TXTSIZ sz;
if(markbuf)
 {
 stbuf(curbuf);
 ldbuf(markbuf);
 }
if(marke<=markb || marke>fmsize() || !markbuf)
 {
 msg((unsigned char *)"\\iNo block\\i");
 if(markbuf)
  ldbuf(bt);
 return;
 }
if(bt==markbuf) if(x>=markb && x<marke) x=markb;
sz=marke-markb;
fmpoint(markb);
fmdel(sz);
if(bt==markbuf) if(x>markb) x-=sz;
stbuf(markbuf);
ldbuf(bt);
fmpoint(x);
updall=1;
newy=1;
}

void moveblk(void)
{
unsigned char *t;
TXTSIZ sz, x=fmnote();
struct buffer *bt=curbuf;
if(markbuf)
 {
 stbuf(curbuf);
 ldbuf(markbuf);
 }
if(marke<=markb || marke>fmsize() || !markbuf)
 {
 msg((unsigned char *)"\\iNo block\\i");
 if(markbuf)
  ldbuf(bt);
 return;
 }
if(x>=markb && x<=marke && bt==markbuf)
 {
 x=markb;
 ldbuf(bt);
 return;
 }
sz=marke-markb;
t=(unsigned char *)malloc(sz);
fmpoint(markb);
fmcpy(t,sz);
fmdel(sz);
if(bt==markbuf) if(x>markb) x-=sz, newy=1;
stbuf(markbuf); 
ldbuf(bt);
fmpoint(x);
fminss(t,sz);
free(t);
markb=x;
marke=x+sz;
markbuf=bt;
updall=1;
}

void cpyblk(void)
{
unsigned char *t;
TXTSIZ x=fmnote();
struct buffer *bt=curbuf;
TXTSIZ sz;
if(markbuf)
 {
 stbuf(curbuf);
 ldbuf(markbuf);
 }
if(marke<=markb || marke>fmsize() || !markbuf)
 {
 msg((unsigned char *)"\\iNo block\\i");
 if(markbuf)
  ldbuf(bt);
 return;
 }
sz=marke-markb;
t=(unsigned char *)malloc(sz);
fmpoint(markb);
fmcpy(t,sz);
stbuf(markbuf);
ldbuf(bt); 
fmpoint(x);
fminss(t,sz);
free(t);
marke=x+sz;
markb=x;
markbuf=bt;
updall=1;
}

void insfil(void)
{
unsigned char gfnam1[PATHSIZE];
unsigned char sting[PATHSIZE];
gfnam1[0]=0;
if(!getl("File to insert",gfnam1)) return;
fixpath(gfnam1);
handle=fopen((char *)gfnam1,"r");
if(handle)
 {
 if(!fminsfil(handle))
  {
  SPRINTF(sting,"\\iError inserting file %s\\i",gfnam1);
  msg(sting);
  }
 newy=1;
 fclose(handle);
 }
else
 {
 SPRINTF(sting,"\\iError opening file %s\\i",gfnam1);
 msg(sting);
 return;
 }
}

void push(void)
{
leave=1;
dclose();
shell();
leave=0;
rewrite();
}

void suspend(void)
{
leave=1;
dclose();
susp();
leave=0;
rewrite();
}

void ioverwrite(void) { overwrite= !overwrite; }
void iwrap(void) { wrap= !wrap; }
void iautoind(void) { autoind= !autoind; }
void itabmagic(void) { tabmagic= !tabmagic; }
void ipic(void) { pic= !pic; }
void ooverwrite(void) { overwrite=0; }
void owrap(void) { wrap=0; }
void oautoind(void) { autoind=0; }
void otabmagic(void) { tabmagic=0; }
void opic(void) { pic=0; }

void setrmargin(void)
{
unsigned char sting[80];
SPRINTF(sting,"%d",rmargin);
if(!getl("Right margin",sting)) return;
rmargin=atol((char *)sting);
if(rmargin<2) rmargin=2;
}

void mode(void)
{
unsigned char s[PATHSIZE];
s[0]=0;
STRCAT(s,"(R)ght Mrgn ");
if(overwrite) STRCAT(s,"(I) Overtype ");
else STRCAT(s,"(I)nsert ");
if(tabmagic) STRCAT(s,"(T)ab Magic on ");
else STRCAT(s,"(T)ab Magic off ");
if(wrap) STRCAT(s,"(W)rap on ");
else STRCAT(s,"(W)rap off ");
if(autoind) STRCAT(s,"(A) Indent on ");
else STRCAT(s,"(A) Indent off ");
if(pic) STRCAT(s,"(P)ic on: ");
else STRCAT(s,"(P)ic off: ");
switch(query(s))
 {
 case 'i':
 case 'I':
 case 'o':
 case 'O':
  ioverwrite();
  break;
 case 'W':
 case 'w':
  iwrap();
  break;
 case 'a':
 case 'A':
  iautoind();
  break;
 case 't':
 case 'T':
  itabmagic();
  break;
 case 'p':
 case 'P':
  ipic();
  break;
 case 'r':
 case 'R':
  setrmargin();
 }
}

/* Center the current line */

void ctrlin(void)
{
TXTSIZ x;
int tmp=pic;
int y;
unfill();
bol();
while(y=fmrc(), y==' ' || y=='\t') fmdel(1);
eol();
x=fmnote();
bol();
if(x-fmnote()>rmargin) return;
y=(rmargin/2)-(x-fmnote())/2;
while(y--) fminsc(' ');
pic=1;
udnarw();
pic=tmp;
}

/* Reformat a paragraph */

void reformat(void)
{
TXTSIZ tmp,idt,idt1,b,e,cur,ncur= -1;
unsigned char ch;

cur=fmnote();		/* Save cursor position */

/* First, determine indentation on current or first non-blank line */

up:
idt=calcs();
if(fmeof()) return;     /* Not if at end of file */
if(fmrc()==NL)  /* Ignore any blank lines */
 {
 dnarw();
 goto up;
 }
bol();

/* Now find beginning of paragraph */
/* It will be indicated by a change of indentation or a blank line or bof */

while(fmnote())         /* Beginning is at bof */
 {
 uparw();
 idt1=calcs();
 if(fmrc()==NL) /* Beginning is blank line */
  {
  bol();
  dnarw();
  break;
  }
 bol();
 if(idt1>idt) break;
 if(idt1<idt)
  {
  dnarw();
  break;
  }
 }

/* Point is now at beginning of paragraph (hopefully) */
/* Set the mark */

b=fmnote();

idt=calcs(); bol();	/* Save indentation level of first line of paragraph */

/* Now move to after end of paragraph */
while(1)
 {
 tmp=fmnote();
 dnarw();
 if(fmnote()==tmp)      /* Paragraph ends on end of file */
  {
  eol();
  fminsc(NL);		/* Stick in a NL */
  fmgetc();
  extend=0;		/* I don't think I have to do this but... */
  break;
  }
 idt1=calcs();
 if(fmrc()==NL)		/* Paragraph ends on blank line */
  {
  bol();
  break;
  }
 bol();
 if(idt1>idt) break;    /* Paragraph ends when indentation increases */
 }

/* Point now after end of paragraph, cut paragraph */
e=fmnote();

/* Now reinsert paragraph in a nice way */

if(e>b)
 {
 unsigned oldwrap=wrap;
 unsigned oldoverwrite=overwrite;
 unsigned oldauto=autoind;
 unsigned flag=0;
 unsigned char ccc=0;
 TXTSIZ ppp=b; 
 undoflag=0;
 overwrite=0;
 wrap=1;
 while(ppp!=e)
  {
  tmp=fmnote();
  fmpoint(ppp);
  if(ppp>=cur && ncur== -1) ncur=tmp;
  ppp++;
  ch=fmrc();
  fmpoint(tmp);
  if(ch==NL) ch=' ';
  if(ch==' ' || ch==TAB)
   {
   if(flag==0) itype(ch);
   else if(flag==1 && ch!='\t')
     {
     itype(' ');
     if(!(ccc=='.' || ccc==':' || ccc=='?' || ccc=='!' || ccc=='\"' ||
          ccc==';')) flag=2;
     }
   else if(ch=='\t') itype('\t');
   }
  else
   {
   flag=1;
   itype(ch);
   }
  ccc=ch;
  }
 autoind=0;
 if(flag) itype(NL);
 /* Make undo record for entered paragraph */
  {
  /* New record */
  struct undorec *it=(struct undorec *)malloc(sizeof(struct undorec));
  if(redorecs) killredo();
  it->next=undorecs;
  undorecs=it;
  it->size=fmnote()-e;
  it->where=e;
  it->buffer=0;
  ++nundorecs;
  if(nundorecs==20)
   {
   struct undorec *p;
   for(it=undorecs;it->next;p=it,it=it->next);
   if(it->buffer) free(it->buffer);
   free(it);
   p->next=0;
   }
  }

 undoflag=1;
 wrap=oldwrap;
 overwrite=oldoverwrite;
 autoind=oldauto;
 fmpoint(b);
 fmdel(e-b);
 fmpoint(ncur-(e-b));
 newy=1;
 }
}

void killword(void)
{
unsigned char ch;
ch=fmrc();
if(((ch>='a' && ch<='z') || (ch>='A' && ch <='Z')) && !fmeof())
 do
  {
  delch();
  ch=fmrc();
  } while (((ch>='a' && ch<='z') || (ch>='A' && ch <='Z')) && !fmeof());
else
 if((ch==' ' || ch==TAB || ch==NL) && !fmeof())
  do
   {
   delch();
   ch=fmrc();
   } while (!fmeof() && (ch==' ' || ch==NL || ch==TAB));
else
 if(ch>='0' && ch<='9' && !fmeof())
  do
   {
   delch();
   ch=fmrc();
   } while (!fmeof() && ch>='0' && ch<='9');
else delch();
}

void backword(void)
{
unsigned char ch;
if(fmnote())
 {
 fmpoint(fmnote()-1);
 ch=fmgetc();
 if((ch>='a' && ch<='z') || (ch>='A' && ch <='Z'))
  {
up:
  backs();
  if(fmnote())
   {
   fmpoint(fmnote()-1);
   ch=fmrc();
   fmgetc();
   if((ch>='a' && ch<='z') || (ch>='A' && ch <='Z')) goto up;
   }
  }
 else if(ch==' ' || ch==TAB || ch==NL)
   {
up1:
   backs();
   if(fmnote())
    {
    fmpoint(fmnote()-1);
    ch=fmrc();
    fmgetc();
    if(ch==' ' || ch==TAB || ch==NL) goto up1;
    }
   }
 else if(ch>='0' && ch<='9')
   {
up2:
   backs();
   if(fmnote())
    {
    fmpoint(fmnote()-1);
    ch=fmrc();
    fmgetc();
    if(ch>='0' && ch<='9') goto up2;
    }
   }
 else backs();
 }
}

int word(void)
{
int c;
if(fmnote()==fmsize()) return 0;
c=fmrc();
if(c>='a' && c<='z') return 1;
if(c>='A' && c<='Z') return 1;
if(c>='0' && c<='9') return 1;
return 0;
}

void wrdl(void)
{
extend=0;
newy=1;
if(!fmnote()) return;
fmrgetc();
while(!word())
 {
 if(!fmnote()) return;
 fmrgetc();
 }
while(word())
 {
 if(!fmnote()) return;
 fmrgetc();
 }
fmgetc();
}

void wrdr(void)
{
extend=0;
newy=1;
while(!word())
 {
 if(fmnote()==fmsize()) return;
 fmgetc();
 }
while(word())
 {
 if(fmnote()==fmsize()) return;
 fmgetc();
 }
}

unsigned char lft[]="{[(<`";
unsigned char rht[]="}])>'";

void gotomatching(void)
{
TXTSIZ cur=fmnote(),cnt;
unsigned char c;
int x;
extend=0;
if(fmeof()) return;
c=fmrc();
for(x=0;x!=STRLEN(lft);++x)
 if(lft[x]==c)
  {
  cnt=0;
  while(!fmeof())
   {
   c=fmgetc();
   if(lft[x]==c) ++cnt;
   if(rht[x]==c)
    if(!--cnt)
     {
     newy=1;
     fmrgetc();
     return;
     }
   }
  fmpoint(cur);
  return;
  }
 else if(rht[x]==c)
  {
  cnt=1;
  while(fmnote())
   {
   c=fmrgetc();
   if(rht[x]==c) ++cnt;
   if(lft[x]==c)
    if(!--cnt)
     {
     newy=1;
     return;
     }
   }
  fmpoint(cur);
  return;
  }
}

int setindent(void)
{
TXTSIZ idt,idt1,cur=fmnote();
if(curbuf==markbuf && cur>=markb && cur<marke) return 1;
markbuf=curbuf; markb=0; marke=0;
/* Find beginning */
idt=calcs();
if(fmeof()) goto done;
if(fmrc()==NL) goto done;
while(fmnrnl())
 {
 idt1=calcs();
/* if(fmrc()!=NL) if(idt1<idt) Use this line instead of one below for
   setindent to ignore blank lines */
 if(fmrc()==NL || idt1<idt)
  {
  if(fmfnl()) fmgetc();
  break;
  }
 }
/* Point is now at beginning of block (hopefully) */
/* Set the mark */
markb=fmnote();

while(fmfnl())
 {
 fmgetc();
 idt1=calcs();
/* if(fmrc()!=NL) if(idt1<idt) Use this line instead of one below for
   setindent to ignore blank lines */
 if(fmrc()==NL || idt1<idt)
  {
  bol();
  break;
  }
 }
marke=fmnote();
done: fmpoint(cur);
return 0;
} 
 
void indentr(void)
{
TXTSIZ cur=fmnote();
if(!setindent()) return;
fmpoint(markb);
while(fmnote()<marke)
 {
 calcs();
 if(fmeof()) break;
 if(fmrc()!=NL)
  {
  if(fmnote()<=cur) ++cur;
  fminsc(' ');
  }
 if(fmfnl()) fmgetc();
 else break;
 }
fmpoint(cur);
return;
}

void indentl(void)
{
TXTSIZ cur=fmnote(),idt;
if(!setindent()) return;
fmpoint(markb);
while(fmnote()<marke)
 {
 idt=calcs();
 if(fmeof()) break;
 if(fmrc()!=NL) if(!idt) goto done;
 if(fmfnl()) fmgetc();
 else break;
 }
fmpoint(markb);
while(fmnote()<marke)
 {
 calcs();
 if(fmeof()) break;
 if(fmrc()!=NL)
  {
  if(fmnote()<=cur) --cur;
  fmrgetc();
  fmdel(1);
  }
 if(fmfnl()) fmgetc();
 else break;
 }
done: fmpoint(cur);
return;
}

struct window *curwin;
struct buffer *curbuf;
struct window *topwin;

void ldbuf(struct buffer *zuffer)
{
if(zuffer==curbuf) return;
curbuf=zuffer;
backup=zuffer->backup;
STRCPY(gfnam,zuffer->gfnam);
bufsiz=zuffer->bufsiz;
buffer=zuffer->buf;
filend=zuffer->filend;
hole=zuffer->hole;
ehole=zuffer->ehole;
changed=zuffer->changed;
undorecs=zuffer->undorecs;
nundorecs=zuffer->nundorecs;
redorecs=zuffer->redorecs;
undoptr=0;
}

void ldbuf1(struct buffer *zuffer)
{
curbuf=zuffer;
backup=zuffer->backup;
STRCPY(gfnam,zuffer->gfnam);
bufsiz=zuffer->bufsiz;
buffer=zuffer->buf;
filend=zuffer->filend;
hole=zuffer->hole;
ehole=zuffer->ehole;
changed=zuffer->changed;
undorecs=zuffer->undorecs;
redorecs=zuffer->redorecs;
nundorecs=zuffer->nundorecs;
undoptr=0;
}

void stbuf(struct buffer *zuffer)
{
zuffer->backup=backup;
STRCPY(zuffer->gfnam,gfnam);
zuffer->bufsiz=bufsiz;
zuffer->buf=buffer;
zuffer->filend=filend;
zuffer->hole=hole;
zuffer->ehole=ehole;
zuffer->changed=changed;
zuffer->undorecs=undorecs;
zuffer->nundorecs=nundorecs;
zuffer->redorecs=redorecs;
undoptr=0;
}

void ldwin(struct window *window)
{
saddr=window->saddr;
xoffset=window->xoffset;
pic=window->pic;
autoind=window->autoind;
overwrite=window->overwrite;
wrap=window->wrap;
tabmagic=window->tabmagic;
rmargin=window->rmargin;
extend=window->extend;
ldbuf1(window->buffer);
fmpoint(window->cursor);
}
                                   
void stwin(struct window *window)
{ 
window->saddr=saddr;
window->xoffset=xoffset;
window->pic=pic;
window->autoind=autoind;
window->overwrite=overwrite;
window->wrap=wrap;
window->tabmagic=tabmagic;
window->rmargin=rmargin;
window->extend=extend;
window->cursor=fmnote();
stbuf(window->buffer);
}

void wfit(void)
{
struct window *x;
int total;
updall=1;
newy=1;
up:
total=height-wind;
for(x=topwin;1;x=x->next)
 {
 if(x->height<3) x->height=3;
 if(curwin==x && total>=3) break;
 if(total<3) goto in;
 total-=x->height;
 if(total<0)
  {
  in:
  topwin=topwin->next;
  goto up;
  }
 }
for(x=topwin,total=wind;1;x=x->next)
 {
 x->wind=total;
 if(x->height<3) x->height=3;
 total+=x->height;
 if(total>=height || x->next==topwin)
  {
  total-=x->height;
  x->height=height-total;
  return;
  }
 }
}

void wnext(void)
{
stwin(curwin);
curwin=curwin->next;
ldwin(curwin);
wfit();
}

void wprev(void)
{
stwin(curwin);
curwin=curwin->prev;
ldwin(curwin);
wfit();
}

void wexplode(void)
{
struct window *x;
int y;
if(curwin->height!=height-wind)
 { /* Make curwin only */
 topwin=curwin;
 x=topwin;
 do
  {
  x->height=height-wind;
  x->wind=wind;
  x=x->next;
  }
  while(x!=topwin);
 newy=1;
 }
else
 { /* Show all windows */
 x=topwin; y=0;
 do y++, x=x->next; while(x!=topwin);
 if((height-wind)/y<3) y=3;
 else y=(height-wind)/y;
 x=topwin;
 do x->height=y, x=x->next; while(x!=topwin);
 wfit();
 }
}

void wgrow(void)
{
if(curwin->wind+curwin->height==height)
 {
 if(curwin->wind!=wind) if(curwin->prev->height>3)
  curwin->prev->height--, curwin->height++, curwin->wind--, updall=1;
 }
else
 {
 if(curwin->next->height>3)
  curwin->height++, curwin->next->wind++, curwin->next->height--, updall=1;
 }
newy=1;
}

void wshrink(void)
{
if(curwin->wind+curwin->height==height)
 {
 if(curwin->wind!=wind) if(curwin->height>3)
 curwin->height--, curwin->prev->height++, curwin->wind++, updall=1;
 }
else
 {
 if(curwin->height>3)
 curwin->height--, curwin->next->wind--, curwin->next->height++, updall=1;
 }
newy=1;
}

void wsplit(void)
{
struct window *new;
if(curwin->height<6) return;
new=(struct window *)malloc(sizeof(struct window));
new->buffer=curbuf;
stwin(new);
new->next=curwin->next;
new->prev=curwin;
curwin->next->prev=new;
curwin->next=new;
if(curwin->height&1)
 {
 curwin->height/=2;
 new->height=curwin->height+1;
 }
else
 {
 curwin->height/=2;
 new->height=curwin->height;
 }
if(curwin->hheight&1)
 {
 curwin->hheight/=2;
 new->hheight=curwin->hheight+1;
 }
else
 {
 curwin->hheight/=2;
 new->hheight=curwin->hheight;
 }
new->wind=curwin->wind+curwin->height;
curwin=new;
curbuf->count++;
updall=1;
newy=1;
}

void wedit(void)
{
unsigned char gfnam1[PATHSIZE];
unsigned char sting[PATHSIZE];
int c;
struct window *x;
stwin(curwin);
if(curbuf->count==1 && curbuf->changed)
 {
 c=askyn("Do you really want to throw away this file?"); 
 if(c=='N') return;
 if(c== -1) return;
 }
gfnam1[0]=0;
if(!getl("File to edit",gfnam1)) return;
fixpath(gfnam1);
x=topwin;
do
 {
 if(!STRCMP(gfnam1,x->buffer->gfnam))
  {
  if(curbuf->count==1)
   {
   killredo();
   killundo();
   free(curbuf->buf), free(curbuf);
   if(curbuf==markbuf) markbuf=0;
   }                           
  else 
   curbuf->count--;
  curwin->buffer=x->buffer;
  curwin->buffer->count++;
  ldbuf(x->buffer);
  bof();
  return;
  }
 x=x->next;
 }
 while(x!=topwin);
STRCPY(gfnam,gfnam1);
stmode(gfnam);
if(curbuf->count==1) free(curbuf->buf),
                             free(curbuf);
else curbuf->count--;
curwin->buffer=(struct buffer *)malloc(sizeof(struct buffer));
curbuf=curwin->buffer;
curbuf->count=1;
fmopen();
bof();
handle=fopen((char *)gfnam1,"r");
if(handle)
 {
 if(!fminsfil(handle))
  {
  SPRINTF(sting,"\\iError loading file %s\\i",gfnam1);
  msg(sting);
  }
 changed=0;
 newy=1;
 fclose(handle);
 }
else
 {
 newy=1;
 upd=1;
 if(errno==ENOENT)
  {
  dupdate();
  msgout(curwin->wind+1,(unsigned char *)"New File",0,0);
  cpos(curwin->wind+1,0);
  backup=1;
  }
 else
  {
  dupdate();
  msgout(curwin->wind+1,(unsigned char *)"\\iError opening file\\i",0,1);
  cpos(curwin->wind+1,0);
  }
 noupdate=1;
 return;
 }
}

void rtn(void)
{
type(NL);
}

void stquote(void)
{
quoteflg=1;
}

void stquote8th(void)
{
quote8th=1;
}

CMD kkm[73]=
{
 {"uparw",0,(CFP)uuparw},
 {"rtarw",0,(CFP)urtarw},
 {"ltarw",0,(CFP)ultarw},
 {"dnarw",0,(CFP)udnarw},
 {"eol",0,(CFP)eol},
 {"pgdn",0,(CFP)pgdn},
 {"bol",0,(CFP)bol},
 {"pgup",0,(CFP)pgup},
 {"ctrlin",0,(CFP)ctrlin},
 {"setbeg",0,(CFP)setbeg},
 {"cpyblk",0,(CFP)cpyblk},
 {"saveit",0,(CFP)saveit},
 {"wedit",0,(CFP)wedit},
 {"findfirst",0,(CFP)findfirst},
 {"findnext",0,(CFP)findnext},
 {"wgrow",0,(CFP)wgrow},
 {"thelp",0,(CFP)thelp},
 {"wexplode",0,(CFP)wexplode},
 {"reformat",0,(CFP)reformat},
 {"setend",0,(CFP)setend},
 {"findline",0,(CFP)findline},
 {"moveblk",0,(CFP)moveblk},
 {"wnext",0,(CFP)wnext},
 {"wprev",0,(CFP)wprev},
 {"wsplit",0,(CFP)wsplit},
 {"insfil",0,(CFP)insfil},
 {"wshrink",0,(CFP)wshrink},
 {"bof",0,(CFP)bof},
 {"eof",0,(CFP)eof},
 {"writeblk",0,(CFP)writeblk},
 {"exsave",0,(CFP)exsave},
 {"delblk",0,(CFP)delblk},
 {"push",0,(CFP)push},
 {"suspend",0,(CFP)suspend},
 {"eexit",0,(CFP)eexit},
 {"delch",0,(CFP)delch},
 {"inss",0,(CFP)inss},
 {"backs",0,(CFP)backs},
 {"type",0,(CFP)(void (*)(void))type},
 {"rtn",0,(CFP)rtn},
 {"backword",0,(CFP)backword},
 {"rewrite",0,(CFP)rewrite},
 {"mode",0,(CFP)mode},
 {"killword",0,(CFP)killword},
 {"wrdr",0,(CFP)wrdr},
 {"dellin",0,(CFP)dellin},
 {"wrdl",0,(CFP)wrdl},
 {"stquote8th",0,(CFP)stquote8th},
 {"stquote",0,(CFP)stquote},
 {"gotomatching",0,(CFP)gotomatching},
 {"indentl",0,(CFP)indentl},
 {"indentr",0,(CFP)indentr},
 {"undo",0,(CFP)undo},
 {"redo",0,(CFP)redo},
 {"killlin",0,(CFP)killlin},
 {"deleol",0,(CFP)deleol},
 {"ioverwrite",0,(CFP)ioverwrite},
 {"iwrap",0,(CFP)iwrap},
 {"iautoind",0,(CFP)iautoind},
 {"itabmagic",0,(CFP)itabmagic},
 {"ipic",0,(CFP)ipic},
 {"ooverwrite",0,(CFP)ooverwrite},
 {"owrap",0,(CFP)owrap},
 {"oautoind",0,(CFP)oautoind},
 {"otabmagic",0,(CFP)otabmagic},
 {"opic",0,(CFP)ipic},
 {"setrnargin",0,(CFP)setrmargin},
 {"cmdblk",0,(CFP)cmdblk},
 {"repeat",0,(CFP)repeat},
 {"macrob",0,(CFP)macrob},
 {"macroe",0,(CFP)macroe},
 {"macrodo",0,(CFP)macrodo},
 {"wait",0,(CFP)waite}
 };

CONTEXT km={0, "main", 0, 73, kkm};

/** Key sequence processing functions **/

struct kmap *curmap;
int quoteflg=0;
int quote8th=0;

int record=0;
unsigned char *kmacro=0;
int kmacrox=0;
int kmacrosz=0;

void macroadd(unsigned char c)
{
if(kmacrox+3>=kmacrosz)
 {
 if(kmacro) kmacro=(unsigned char *)realloc(kmacro,kmacrosz+=10);
 else kmacro=(unsigned char *)malloc(kmacrosz=10);
 }
if(c=='\\')
 {
 kmacro[kmacrox++]='\\';
 kmacro[kmacrox++]='\\';
 }
else
 kmacro[kmacrox++]=c;
kmacro[kmacrox]=0;
}

void macrob(void)
{
kmacrox=0;
record=1;
}

void macroe(void)
{
record=0;
}

void macrodo(void)
{
int z=repeatamnt;
if(record) return;
repeatamnt=1;
nrepeatamnt=1;
take=kmacro;
waite();
repeatamnt=z;
}

int *runfuncs;
int rsize=0;
int rptr=0;

int isrunning(int r, int n)
{
int z;
for(z=0;z!=rptr;z+=2) if(r==runfuncs[z] && n==runfuncs[z+1]) return 1;
return 0;
}

void pop(void) { rptr-=2; }

void running(int r, int n)
{
if(rsize==rptr)
 {
 if(rsize) runfuncs=(int *)realloc(runfuncs,sizeof(int)*(rsize+=10));
 else runfuncs=(int *)malloc(sizeof(int)*(rsize=10));
 }
runfuncs[rptr++]=r;
runfuncs[rptr++]=n;
}

int dokey(unsigned char k)
{
int above=curmap->len;
int below=0;
int new;
struct kmap *r;
if(quoteflg)
 {
 quoteflg=0;
 if(k>='@' && k<='_') k-='@';
 if(k>='a' && k<='z') k-='`';
 if(k=='?') k=127;
 type(k);
 goto abcd;
 }
goto in;
do
 {
 new=(above+below)/2;
 if((curmap->keys[new].k&KEYMASK)==k)
  if(curmap->keys[new].k&KEYSUB)
   {
   curmap=(KMAP *)(curmap->keys[new].n);
   return Kaccept;
   }
  else
   {
   int h=height, w=width, zz, rpt=repeatamnt;
   getsize();
   if(h!=height || w!=width) resize();
   r=curmap;
   curmap=km.kmap;
   nrepeatamnt=1;
   while(rpt--)
    {
    if(rpt>1 && have)
     {
     anext();
     msg((unsigned char *)"Repeat aborted");
     undoptr=0;
     break;
     }
    if(isrunning((int)r,new)) undoptr=0, type(k);
    else
     {
     running((int)r,new);
     for(zz=0;r->keys[new].n[zz]!= -1;zz+=2)
      {
      if(r->keys[new].n[zz])
       {
       take=(unsigned char *)r->keys[new].n[zz+1];
       }
      else
       {
       if(km.cmd[r->keys[new].n[zz+1]].func!=(CFP)redo &&
          km.cmd[r->keys[new].n[zz+1]].func!=(CFP)undo) undoptr=0;
       km.cmd[r->keys[new].n[zz+1]].func(k);
       if(leave) goto abcd;
       }
      }
     pop();
     }
    }
   abcd:
   repeatamnt=nrepeatamnt;
   if(!leave)
    {
    if(!uuu) upd=1;
    else uuu=0;
    if(!noupdate) dupdate();
    else noupdate=0;
    }
   return 0;
   }
 else if((curmap->keys[new].k&KEYMASK)>k)
  {
  above=new;
  in:
  if(above==below) break;
  }
 else if(below==new) break;
 else below=new;
 } while(1);
curmap=km.kmap;
return Kbad;
}

void edit(void)
{
newy=1;
dupdate();
imsg();
dokey(anext());
if(leave) return;
upd=1;
newy=1;
do
 dokey(anext());
 while(!leave);
}

void waite(void)
{
while(take && *take)
 {
 dokey(anext());
 if(leave) break;
 }
}

struct mpair
 {
 struct mpair *next;
 unsigned char *s;
 int wrap;
 int autoind;
 int pic;
 int overwrite;
 int tabmagic;
 TXTSIZ rmargin;
 };

struct mpair *mpairs=0;

void stmode(unsigned char *name)
{
int x=STRLEN(name);
struct mpair *mp=mpairs;
while(mp)
 if(!STRCMP(mp->s,name+x-STRLEN(mp->s)))
  {
  autoind=mp->autoind;
  wrap=mp->wrap;
  overwrite=mp->overwrite;
  pic=mp->pic;
  tabmagic=mp->tabmagic;
  rmargin=mp->rmargin;
  break;
  }
 else mp=mp->next;
}

int process(char *name, CONTEXT *cmds)
{
CONTEXT *context=0;
unsigned char buf[PATHSIZE];
KMAP *kmap;
FILE *fd=fopen((char *)name,"r");
int x,y,n,z,macrox,macrosize,*macro,macroc;
if(!fd) return -1;
printf("Processing keymap file %s ...",name);
fflush(stdout);
while(fgets((char *)buf,256,fd))
 {
 if(buf[0]=='{')
  {
  up:
  if(!fgets((char *)buf,256,fd)) break;
  if(buf[0]=='}') continue;
  if(helpsize+STRLEN(buf)>helpblksize)
   {
   if(help) help=(unsigned char *)realloc(help,helpblksize+STRLEN(buf)+320);
   else help=(unsigned char *)calloc(1,STRLEN(buf)+320);
   helpblksize+=STRLEN(buf)+320;
   }
  STRCAT(help,buf);
  helpsize=STRLEN(help);
  ++helplines;
  goto up;
  }
 if(buf[0]=='*')
  {
  struct mpair *mp=(struct mpair *)calloc(sizeof(struct mpair),1);
  int c=0;
  mp->next=mpairs;
  mp->rmargin=76;
  mpairs=mp;
  for(x=0;buf[x];x++)
   if(buf[x]==' ' || buf[x]=='\t' || buf[x]=='\n')
    {
    c=buf[x];
    buf[x]=0;
    break;
    }
  mp->s=strdupp(buf+1);
  buf[x]=c;
  while(buf[x])
   if(buf[x]!=' ' && buf[x]!='\t' && buf[x]!='\n') break;
   else x++;
  while(buf[x] && buf[x]!=' ' && buf[x]!='\t' && buf[x]!='\n')
   {
   switch(buf[x])
    {
   case 'O':
   case 'o': mp->overwrite=1;
   break;
   case 'W':
   case 'w': mp->wrap=1;
   break;
   case 'a':
   case 'A': mp->autoind=1;
   break;
   case 'p':
   case 'P': mp->pic=1;
   break;
   case '0': case '1': case '2': case '3': case '4': case '5': case '6':
   case '7': case '8': case '9':
   mp->rmargin=buf[x++]-'0';
   while(buf[x]>='0' && buf[x]<='9') mp->rmargin=mp->rmargin*10+buf[x++]-'0';
   --x;
   break;
   case 't':
   case 'T': mp->tabmagic=1;
    }
   x++;
   }
  continue;
  }
 if(buf[0]==':' && buf[1]!=' ' && buf[1]!='\t')
  {
  for(x=0;buf[x];x++)
   if(buf[x]==' ' || buf[x]=='\t' || buf[x]=='\n')
    {
    buf[x]=0;
    break;
    }
  context=cmds;
  while(STRCMP(buf+1,context->name))
   {
   context=context->next;
   if(!context)
    {
    printf("Unknown context name in keyboard file\n");
    return -1;
    }
   }
  continue;
  }
 for(x=0;buf[x];x++) if(buf[x]==' ' || buf[x]=='\t' || buf[x]=='\n' ||
                        buf[x]==',') break;
 if(buf[0]==' ' || buf[0]=='\t' || buf[0]=='\n' || !buf[x]) continue;
 if(!context)
  {
  printf("No context selected for key\n");
  return -1;
  }
 if(buf[0]=='\"')
  {
  int q;
  x=0;
  for(q= ++x;buf[q];++q)
   if((buf[q]=='\"' && buf[q-1]!='\\') || buf[q]=='\n') break;
  macroc=buf[q]; buf[q]=0;
  macro=(int *)malloc(sizeof(int)*2);
  macrosize=2;
  macrox=0;
  macro[macrox++]=1;
  macro[macrox++]=(int)strdupp(buf+x);
  buf[q]=macroc;
  if(macroc=='\"') x=q+1;
  else x=q;
  }
 else
  {
  macroc=buf[x];
  buf[x]=0;
  for(y=0;y!=context->size;y++)
    if(!STRCMP(context->cmd[y].name,buf)) goto foundit;
  printf("Key function not found %s\n",buf);
  continue;
  foundit:

  macro=(int *)malloc(sizeof(int)*2);
  macro[0]=0;
  macro[1]=y;
  macrox=2;
  macrosize=2;
  buf[x]=macroc;
  }

 fn:
 if(buf[x]==',')
  {
  if(buf[++x]=='\"')
   {
   int q;
   for(q= ++x;buf[q];++q)
    if((buf[q]=='\"' && buf[q-1]!='\\') || buf[q]=='\n') break;
   macroc=buf[q]; buf[q]=0;
   if(macrox==macrosize) macro=(int *)realloc(macro,sizeof(int)*(macrosize+=2));
   macro[macrox++]=1;
   macro[macrox++]=(int)strdupp(buf+x);
   buf[q]=macroc;
   if(macroc=='\"') x=q+1;
   else x=q;
   goto fn;
   }
  else
   {
   int q;
   for(q=x;buf[q];++q) if(buf[q]=='\t' || buf[q]==' ' || buf[q]==',') break;
   macroc=buf[q]; buf[q]=0;
   for(y=0;y!=context->size;++y)
    if(!STRCMP(context->cmd[y].name,buf+x)) goto ff;
   printf("Key function not found %s\n",buf+x);
   free(macro);
   continue;
   ff:
   if(macrox==macrosize) macro=(int *)realloc(macro,sizeof(int)*(macrosize+=2));
   macro[macrox++]=0;
   macro[macrox++]=y;
   buf[q]=macroc; x=q;
   goto fn;
   }
  }

 if(macrox==macrosize) macro=(int *)realloc(macro,sizeof(int)*(++macrosize));
 macro[macrox]= -1;
 
 kmap=0;
 n= -1;
 for(;buf[x];x++) if(buf[x]!=' ' && buf[x]!='\t') break;
 while(1)
  {
  int c;
  if(buf[x]==' ') x++;
  if(!buf[x]) break;
  if(buf[x]=='\n' || buf[x]==' ' || buf[x]=='\t') break;
  /* Got Next key */
  x++;
  if(buf[x-1]=='^')
   if(buf[x]==' ' || buf[x]=='\t' || buf[x]=='\n' || !buf[x]) c='^';
   else if(buf[x]=='?') c=127, x++;
   else c=(buf[x]&0x1f), x++;
  else if((buf[x-1]&0x5f)=='S' && (buf[x]&0x5f)=='P') c=' ', x++;
  else c=buf[x-1];
  /* Add it as if it were a submap */
  if(!kmap)
   {
   if(!(kmap=context->kmap))
    {
    kmap=(KMAP *)malloc(sizeof(KMAP));
    kmap->keys=(KEY *)malloc(4*sizeof(KEY));
    kmap->size=4;
    kmap->len=0;
    context->kmap=kmap;
    }
   }
  else
   if(kmap->keys[n].k&KEYSUB) kmap=(KMAP *)(kmap->keys[n].n);
   else
    {
    kmap->keys[n].n=(int *)malloc(sizeof(KMAP));
    kmap->keys[n].k|=KEYSUB;
    kmap=(KMAP *)(kmap->keys[n].n);
    kmap->keys=(KEY *)malloc(4*sizeof(KEY));
    kmap->len=0;
    kmap->size=4;
    }
  for(n=0;n!=kmap->len;n++)
   if((kmap->keys[n].k&KEYMASK)==c) goto sub;
   else if((kmap->keys[n].k&KEYMASK)>c) break;
  if(kmap->len==kmap->size)
   kmap->keys=(KEY *)realloc(kmap->keys,sizeof(KEY)*(kmap->size+=8));
  for(z=kmap->len;z!=n;z--) kmap->keys[z]=kmap->keys[z-1];
  kmap->len++;
  kmap->keys[n].k=c;
  kmap->keys[n].n=macro;
  sub:;
  }
 }
fclose(fd);
printf("done\n");
return 0;
}

int main(int argc, char *argv[])
{
if(process(KEYMAP,&km))
 {
 unsigned char *hh=(unsigned char *)getenv("HOME");
 if(!hh) goto in;
 STRCPY(gfnam,hh);
 STRCAT(gfnam,"/");
 STRCAT(gfnam,KEYMAP);
 if(process((char *)gfnam,&km))
  {
  in:
  if(process(KEYDEF,&km))
   {
   printf("Couldn't open keymap\n");
   return 1;
   }
  }
 }
curmap=km.kmap;
if(argc>2)
 {
 fputs("\nIllegal number of command line arguments",stderr);
 fputs("\nEditor Command Format:  joe [filename]\n",stderr);
 return 0;
 }
termtype();
curwin=(struct window *)malloc(sizeof(struct window));
topwin=curwin;
curwin->next=curwin;
curwin->prev=curwin;
markbuf=0;
curwin->height=height;
curwin->wind=0;
curwin->buffer=(struct buffer *)malloc(sizeof(struct buffer));
curbuf=curwin->buffer;
curbuf->count=1;
sigjoe();
aopen();
dopen();
fmopen();
bof();
options=0;
sstring[0]=0;
rstring[0]=0;
leave=0;

rmargin=width-4;
tabmagic=0;
wrap=1;
autoind=0;
overwrite=0;
pic=0;
stmode((unsigned char *)"");

gfnam[0]=0;

if(argc==2)
 {
 STRCPY(gfnam,argv[1]);
 stmode(gfnam);
 handle=fopen(argv[1],"r");
 if(handle)
  {
  if(!fminsfil(handle))
   omsg=(unsigned char *)"\\iError reading file\\i";
  else
   changed=0;
  fclose(handle);
  }
 else
  {
  if(errno==ENOENT)
   {
   omsg=(unsigned char *)"New File";
   backup=1;
   }
  else
   omsg=(unsigned char *)"\\iError opening file\\i";
  }
 }
else omsg=(unsigned char *)"New File";
edit();
aclose();
signorm();
return 0;
}

void tsignal(int sig)
{
unsigned char nam[PATHSIZE];
long tim=time(0);
struct window *orig=curwin;
SPRINTF(nam,"%s%d",ABORT,getpid());
handle=fopen((char *)nam,"a");
fmpoint(0);
fprintf(handle,"\n*** Files in JOE when it aborted on %s",ctime(&tim));
if(sig) fprintf(handle,"*** JOE was aborted by the signal %d\n",sig);
else fprintf(handle,"*** JOE was aborted because stdin closed\n");
if(gfnam[0]) fprintf(handle,"*** FILE: %s\n",gfnam);
else fprintf(handle,"*** FILE: (Unnamed)\n");
fmsave(handle,fmsize());
fflush(handle);
curbuf->count=0;
do
 {
 stwin(curwin);
 curwin=curwin->next;
 ldwin(curwin);
 if(curbuf->count)
  {
  fmpoint(0);
  if(gfnam[0]) fprintf(handle,"*** FILE: %s\n",gfnam);
  else fprintf(handle,"*** FILE: (Unnamed)\n");
  fmsave(handle,fmsize());
  fflush(handle);
  curbuf->count=0;
  }
 }
 while(curwin!=orig);
fclose(handle);
_exit(1);
}
