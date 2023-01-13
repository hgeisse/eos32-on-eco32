/* Cruddy terminal interface - should be very portable though
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
#include <signal.h>

#include "async.h"
#include "joe.h"

int have=0;

void eputs(char *s)
{
fputs(s,stdout);
}

void eputc(int c)
{
putchar(c);
}

void tsignal(int sig);

void sigjoe(void)
{
signal(SIGHUP,tsignal);
signal(SIGTERM,tsignal);
signal(SIGQUIT,SIG_IGN);
signal(SIGPIPE,SIG_IGN);
signal(SIGINT,SIG_IGN);
}

void signorm(void)
{
signal(SIGHUP,SIG_DFL);
signal(SIGTERM,SIG_DFL);
signal(SIGQUIT,SIG_DFL);
signal(SIGPIPE,SIG_DFL);
signal(SIGINT,SIG_DFL);
}

void aopen(void)
{
fflush(stdout);
system("/bin/stty raw -echo");
}

void aclose(void)
{
fflush(stdout);
system("/bin/stty cooked echo");
}

void aflush(void)
{
}

unsigned char *take=0;

int anext(void)
{
unsigned char c;
if(take) {
 if(*take)
  {
  int c;
  if(*take!='\\') return *take++;
  ++take;
  if(!*take) return '\\';
  else if(*take=='r') c='\r';
  else if(*take=='b') c=8;
  else if(*take=='n') c=10;
  else if(*take=='f') c=12;
  else if(*take=='a') c=7;
  else if(*take=='\"') c='\"';
  else if(*take>='0' && *take<='7')
        {
        c= *take++-'0';
        if(*take>='0' && *take<='7')
         {
         c=c*8+*take++-'0';
         if(*take>='0' && *take<='7') c=c*8+*take++-'0';
         }
        --take;
        }
  else c= *take;
  ++take;
  return c;
  }
 else take=0;
}
fflush(stdout);
if(read(fileno(stdin),&c,1)<1) tsignal(0);
if(record) macroadd(c);
return c;
}

void getsize(void)
{
#ifdef TIOCGSIZE
struct ttysize getit;
#else
#ifdef TIOCGWINSZ
struct winsize getit;
#else
char *p;
#endif
#endif
#ifdef TIOCGSIZE
if(ioctl(fileno(stdout),TIOCGSIZE,&getit)!= -1)
 {
 if(getit.ts_lines>=3) height=getit.ts_lines;
 if(getit.ts_cols>=2) width=getit.ts_cols;
 }
#else
#ifdef TIOCGWINSZ
if(ioctl(fileno(stdout),TIOCGWINSZ,&getit)!= -1)
 {
 if(getit.ws_row>=3) height=getit.ws_row;
 if(getit.ws_col>=2) width=getit.ws_col;
 }
#else
if((p=getenv("ROWS")) != NULL) sscanf(p,"%d",&height);
if((p=getenv("COLS")) != NULL) sscanf(p,"%d",&width);
if(height<3) height=24;
if(width<2) width=80;
#endif
#endif
}

void termtype(void)
{
getsize();
}

void shell(void)
{
int x;
char *s=(char *)getenv("SHELL");
if(!s)
 {
 puts("\nSHELL variable not set");
 return;
 }
eputs("\nYou are at the command shell.  Type 'exit' to continue editing\r\n");
aclose();
if((x=fork()) != 0)
 {
 if(x!= -1) wait(0);
 }
else
 {
 signorm();
 execl(s,s,NULL);
 _exit(0);
 }
aopen();
}

void susp(void)
{
shell();
}
