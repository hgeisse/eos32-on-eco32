/*
 * logfile.c -- output to a logfile
 */


#include "../include/param.h"

#include "logfile.h"


void logfileOut(char c) {
  *OUTPUT_DEVICE = c;
}
