/*
 * console.c -- console I/O
 */


#include "../include/param.h"

#include "console.h"
#include "keyboard.h"
#include "display.h"


static Bool keyboardInitialized = FALSE;
static Bool displayInitialized = FALSE;


char consoleIn(void) {
  if (!keyboardInitialized) {
    kbdinit();
    keyboardInitialized = TRUE;
  }
  return kbdin();
}


void consoleOut(char c) {
  if (!displayInitialized) {
    dspinit();
    displayInitialized = TRUE;
  }
  dspout(c);
}
