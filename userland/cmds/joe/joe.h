/* JOE header file
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

/* File characteristics */

#define NL '\n'			/* End of line character */
#define TAB '\t'		/* Tab character */
#define TABWIDTH 8		/* Tab width */
#define NOHIGHBIT		/* Comment this out to send characters with
				   high bit set to terminal as is.  See
				   the function 'showas'. */

/* Types used in the file buffer */

typedef unsigned char * TXTPTR;	/* Pointer to text in the buffer */
typedef unsigned TXTSIZ;	/* Integer to hold size of file */
#define TXTFREE(a) free(a)	/* Free a buffer */
#define TXTMALLOC(a) malloc(a)	/* Allocate a buffer */
#define TXTREALLOC(a,b) realloc((a),(b))	/* Reallocate a buffer */

/* File names and characteristics */

#define PATHSIZE 256		/* Maximum path length */
#define KEYMAP ".joerc"		/* Keymap file */
/* #define KEYDEF "/usr/bin/.joerc"	Default keymap file */
#define ABORT "DEADJOE"		/* Aborted file */

/* The current file buffer */
/* When you change windows, these variables get stored in the 'struct buffer'
   associated with the old window and are load with the values in the
   'struct buffer' for the new window */

extern TXTSIZ bufsiz;		/* Size of malloc block buffer is in */
extern TXTPTR point;		/* The point (cursor) */
extern TXTPTR buffer;		/* The buffer */
extern TXTPTR filend;		/* First char not in buffer */
extern TXTPTR hole;		/* Address of hole */
extern TXTPTR ehole;		/* First char not in hole */
extern int changed;		/* Set if buffer changed */
extern int backup;		/* Set if backup file has been made */
extern unsigned char gfnam[PATHSIZE];
				/* Current edit file name.  "" for unnamed */

#define HOLESIZE 1024		/* Amount file buffer grows by */

/*******************************************************/
/* Basic file buffer manipulation functions and macros */
/*******************************************************/

void fminsu(TXTSIZ size);	/* fminsu(size) Adjust pointers by amnt inserted */
void fmdelu(TXTSIZ size);	/* fmdelu(size) Adjust pointers by amount deleted */ 
/* The pointers the above two functions currently update include:
	The pointer to start of each window which references the current
	buffer.

	The pointer to cursor in each window which reference the current
	buffer but not the one for the current window.

	The begin & end pointers to the marked block if they are in the
	current buffer.
*/
void fmdel(TXTSIZ x);
void fmcpy(unsigned char *string, int size);

void fmopen(void);		/* fmopen() Initialize current edit buffer */
void fmexpand(unsigned amount);	/* fmexpand(amount) Make buffer bigger */
void fmhole(void);		/* fmhole() Move hole to point */
void fmbig(TXTSIZ size);	/* fmbig(size) Make hole at least size */
int fmfnl(void);		/* Find first NL.  Returns 0 if not found */
				/* If at an NL already, point is not moved */
int fmrnl(void);		/* Find NL in reverse.  Rtns 0 if not found */
				/* If at an NL already, point is not moved */
void fminss(unsigned char *string, unsigned size);	/* fminss(blk,size) Insert a block at point */
int fmcmp(unsigned char *string, int size);		/* fmcmp(blk,size) return 0 if matching */
int tupp(unsigned char c);	/* tupp(c) Convert char to uppercase */
int fmicmp(unsigned char *string, int size);		/* Same as fmcmp but ignore case */
int fmsave(FILE *file, TXTSIZ size);		/* fmsave(FILE,size) Save at point in file */
int fminsfil(FILE *file);	/* fminsfil(FILE) Insert file at point */

/******************/
/* Terminal stuff */
/******************/

/* Terminal characteristics (terminal must be vt100ish) */

extern int width;		/* Screen width */
extern int height;		/* Screen height */
extern int scroll;		/* Set if terminal has scrolling regions */

/* Terminal state */

extern int smode;		/* Current character attributes */
extern int tops;		/* Scroll region top (-1 for unknown) */
extern int bots;		/* Scroll region bottem */
extern int oxpos;		/* Cursor position */
extern int oypos;
extern int *scrn;		/* Screen buffer
					-1 means unknown character
					0 - 255 means known character
				*/

extern unsigned char *omsg;	/* Opening message */
void dopen(void);		/* Open display (clear it, allocate scrn,
				   etc.) */
void dclose(void);		/* dclose(s) Show final message and close
				   display */

void cposs(int y, int x);	/* cpos(row,col) Set cursor position */
void cpos(int y, int x);	/* cpos(row,col) Set cursor position and
				   update ox/oypos */
void setregn(int top, int bot);	/* setregn(top,bot) Set scroll region */

void attrib(int x);		/* attrib(mask) Set attributes */
#define INVERSE 256
#define BLINK 512
#define UNDERLINE 1024
#define BOLD 2048

/*****************/
/* Screen update */
/*****************/

/* Flags which high-level edit functions set to control the screen
   update.  All three are initialized to 0 before an edit function
   is executed */

extern int uuu;			/* Set is no screen update needed */
extern int cntr;		/* Set to center cursor to middle of
				   screen if the screen will scroll
				   (for search/replace) */
extern int newy;		/* Set if row changed */
extern int updall;		/* Set to update all windows, not just
				   the ones with same buffer */

/* Flags which indicate the current progress of a screen update (I.E., so
   we can continue if user interrupts screen update) */

extern int upd;			/* Set if a screen update should be done */
extern int hupd;		/* Set if a help update should be done */

extern int helpon;		/* Set if help screen is on */
extern int wind;		/* Number of help lines */

extern int xpos;		/* Requested x & y positions (as determined */
extern int ypos;		/* by scroll calculator: dupdate1 */

extern TXTSIZ saddr;		/* Byte offset to first char of first screen
				   line (of current window) */
extern TXTSIZ xoffset;		/* Cols current window is scrolled to right */
extern TXTSIZ extend;		/* Column number if past end of line or in
				   tab stop */

/* Functions for doing screen update */

void clreolchk(int i, TXTSIZ j);	/* clreolchk(lin,col) Clear to end of line if needed */
int udline(int i);		/* udline(lin) Update a single line.  Return true
			   EOF reached */
int udscrn(void);		/* Update screen (returns true if it finished) */
void dupdate1(int fgf);		/* dupdate1(flg) Recalculate cursor, scroll & update
			   screen (sets cursor position if flg is set) */
void dupdatehelp(void);		/* Update help */
void dupdate(void);		/* Update help and screen */
void invalidate(int line);	/* invalidate(lin) Invalidate a line so it gets upd. */

/****************/
/* Window Stuff */
/****************/

/* Each file that's edited has a 'struct buffer' associated with it.
   This stores the buffer variables when the buffer is not the current
   buffer (I.E., when the cursor is in a window for another file).
*/

struct buffer
 {
 int count;		/* Reference count (No. windows into this buffer) */
 TXTSIZ bufsiz;		/* Size of malloc block buffer is in */
 TXTPTR buf;		/* The buffer */
 TXTPTR filend;		/* First char not in buffer */
 TXTPTR hole;		/* Address of hole */
 TXTPTR ehole;		/* First char not in hole */
 int changed;		/* Set if buffer changed */
 int backup;		/* Set if backup file has been made */
 unsigned char gfnam[PATHSIZE];	/* Current edit file name.  "" for unnamed */
 struct undorec *undorecs;
 struct undorec *redorecs;
 int nundorecs;
 };

/* Each window has a 'struct window' associated with it */

struct window
 {
 struct window *next;	/* Doubly linked list of windows */
 struct window *prev;

 struct buffer *buffer;	/* The buffer this window looks at */

 /* Screen variables for each window */

 TXTSIZ saddr;		/* Byte offset to first character of first line in
 			   window */
 TXTSIZ xoffset;	/* No. columns the screen is scrolled to the right */

 /* Window size */

 int wind;         /* Starting screen line */
			/* wind is not the same as 'wind' the number of
			   help lines */
 int height;       /* Height of window */
 int hheight;      /* Height before help turned on */

 /* Edit modes */

 int pic;
 int autoind;
 int overwrite;
 int wrap;
 int tabmagic;
 TXTSIZ rmargin;

 /* Cursor position */

 TXTSIZ extend;		/* Column number if cursor is past end of line or
 			   if it's in a tab stop */
 TXTSIZ cursor;		/* Byte offset (in buffer) to the cursor */

 };

extern struct window *curwin;	/* Current window */
extern struct buffer *curbuf;	/* Current buffer */
extern struct window *topwin;	/* First window on the screen */

/* Keyboard and command table handler */

typedef struct key KEY;
struct key
 {
 int k;                 /* Key value */
 int *n;                /* Command number or submap address */
 };

typedef struct kmap KMAP;
struct kmap
 {
 int len;          /* Number of KEY entries */
 int size;         /* Size of malloc block */
 KEY *keys;             /* KEYs.  Sorted. */
 };

/* Masks & bits for k */

#define KEYMASK 0x7fff
#define KEYSUB 0x8000	/* Set for submap */

/* A command entry */

typedef void (*CFP)(unsigned char);
typedef struct cmd CMD;
struct cmd
 {
 char *name;
 int flag;
 CFP func;
 };

/* A context (group of related commands) */

typedef struct context CONTEXT;
struct context
 {
 CONTEXT *next;		/* List of all contexts */
 char *name;		/* Name of this context */
 KMAP *kmap;		/* Top level keymap for this context */
 int size;		/* Number of entries in this context */
 CMD *cmd;		/* The entries themselves (sorted) */
 };

int dokey(unsigned char k);	/* dokey(c) Execute next key */
extern int quoteflg;	/* Set if next key is quoted */
extern int quote8th;	/* Set if next key is quoted */

/* dokey() Return values */

#define Kaccept -1	/* Key accepted but not executed */
#define Kbad -2		/* Bad key */
/* dokey() used to return a function number; now it executes the function
   itself so the return values are meaningless */

/* Messages and queries */

/* These are all hacks because they return/check for exact key values
   and don't know about the key table.  Someday a key 'context' should
   be added for these
*/

int getl(char *prompt, unsigned char *dat);
			/* getl(prompt,line) Get a line of input */
			/* Returns: -1 if user hits ^L
				     1 if user hits \n or \r
				     0 if user hits ^C
			    (yes this is a stupid hack)
			*/

void msg(unsigned char *ms);	/* msg(s) Show a message until user hits a key */
void msgout(int row, unsigned char *str, int flg, int flg1);

int askyn(char *ms);	/* askyn(s) Yes/No question 
			Returns: 'Y', 'N' or -1 for ^C */

int query(unsigned char *ms);	/* query(s) Show message, wait for user to hit a key,
			   then return key. */

int nquery(char *ms);	/* nquery(s) Same as query but leave cursor on
			   edit screen */
void imsg(void);	/* imsg() Show opening message */

/*******************************************/
/* High-level edit functions and variables */
/*******************************************/

/* Edit modes */

extern int pic;			/* Set for picture mode */
extern int autoind;		/* Set for autoindent */
extern int overwrite;		/* Set for overwrite */
extern int wrap;		/* Set for autowrap */
extern int tabmagic;		/* Set for magical tabs */
extern TXTSIZ rmargin;		/* Current right margin */

/****************************/
/* Search and replace stuff */
/****************************/

/* Search & replace options */

#define s_ignore 1		/* Ignore case */
#define s_backwards 2		/* Search backwards */
#define s_replace 4		/* Replace */
#define s_regex 8		/* Regular expression search */

extern int options;		/* Search options */
extern unsigned char sstring[PATHSIZE];	/* Search string */
extern unsigned char rstring[PATHSIZE];	/* Replace string */
extern int len;			/* Length of search string */

/**********/
/* Blocks */
/**********/

extern TXTSIZ markb;		/* Begining of block */
extern TXTSIZ marke;		/* End of block */
extern struct buffer *markbuf;	/* Buffer block is in or 0 for no block */

/**************************************/
/* High level edit function utilities */
/**************************************/

extern int leave;		/* Edit function sets this to leave the editor
				   after the function returns */

void dnarw(void);		/* Move cursor to next line */
				/* Column number is preserved */
TXTSIZ calcs(void);		/* Calculate number of whitespace columns
				   at beginning of line.  Cursor is left
				   at first non-whitespace character */
int saveit1(unsigned char *tos);	/* saveit1(s) Save buffer in file & clear
				   changed */
void itype(unsigned char ch);
void ltarw(void);		/* Move cursor left (goes to end of previous
				   line if at beginning of line) */
void uparw(void);		/* Move cursor up (preserves column) */
void rtarw(void);		/* Move cursor right (goes to beginning of
				   next line if at end of line) */

/* Return current column number of cursor */

#define getcol() (extend?extend:getrcol())

TXTSIZ getrcol(void);		/* Get column number of point */
void gocol(TXTSIZ x);		/* gocol(col) Set cursor (point/extend) to
				   column number */
void unfill(void);		/* Remove trailing spaces from line */
void fillup(void);		/* Fill to extend position (use this only
				   if extend if past end of line, not for
				   if extend is in tab stop) */

int search(void);		/* Execute a search.  Returns 1 if found,
				   0 if not */

/* Window functions */

void ldwin(struct window *window);	/* ldwin(window) load window */
void stwin(struct window *window);	/* stwin(window) save window */
void ldbuf(struct buffer *zuffer);	/* ldbuf(buf) load buf if it's not already */
void ldbuf1(struct buffer *zuffer);	/* ldbuf1(buf) load buf always */
void stbuf(struct buffer *zuffer);	/* stbuf(buf) store buffer */
void wfit(void);			/* make sure the current window is on screen */

/* High Level (user) edit functions */

void wnext(void);			/* goto next window */
void wprev(void);			/* goto previous window */
void wexplode(void);			/* show 1 or all windows */
void wgrow(void);			/* make window bigger */
void wshrink(void);			/* make window smaller */
void wedit(void);			/* edit a new file */
void wsplit(void);			/* Split window into 2 */

void rewrite(void);			/* Rewrite screen */
void thelp(void);			/* Toggle help screen */
void bof(void);			/* Goto beginning of file */
void eof(void);			/* Goto end of file */
void bol(void);			/* Goto beginning of line */
void eol(void);				/* Goto end of line */
void urtarw(void);			/* Move cursor right (scroll if need to) */
void ultarw(void);
void uuparw(void);
void udnarw(void);
void delch(void);			/* Delete character */
void type(unsigned char ch);		/* type(c) type a character */
void inss(void);			/* insert a space */
void backs(void);			/* backspace */
void eexit(void);			/* Exit & abort */
void pgup(void);				/* 1/2 Page up */
void pgdn(void);				/* 1/2 Page down */
void deleol(void);			/* Erase end of line */
void dellin(void);			/* Erase entire line */
void exsave(void);			/* Save and exit */
void saveit(void);			/* Save current file */
void findline(void);			/* Goto line No. */
void findfirst(void);			/* Find some text */
void findnext(void);			/* Find next occurance */
void setbeg(void);			/* Set beginning of block */
void setend(void);			/* Set end of block */
void writeblk(void);			/* Write block to file */
void moveblk(void);			/* Move block to point */
void cpyblk(void);			/* Copy block to point */
void delblk(void);			/* Delete block */
void insfil(void);			/* Insert a file */
void push(void);			/* Execute a shell */
void mode(void);			/* Change edit mode */
void ctrlin(void);			/* Center current line */
void reformat(void);			/* Reformat current paragraph */
void killword(void);			/* Delete word */
void backword(void);			/* Delete word to the left */
void wrdl(void);			/* goto previous word */
void wrdr(void);			/* goto next word */
void macrob(void);
void macroe(void);
void macrodo(void);
void edit(void);			/* Main edit loop */
void waite(void);
void macroadd(unsigned char c);
extern FILE *handle;		/* File handle used for many various things */
extern TXTSIZ added;		/* Number of chars autoindent added
				(obsolete?) */
void tputcc(int c);
void stmode(unsigned char *name);

/* Portable strdup() */

#define strdupp(x) \
  ((unsigned char *) STRCPY((unsigned char *) malloc(STRLEN(x) + 1), (x)))
