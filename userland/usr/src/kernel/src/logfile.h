/*
 * logfile.h -- output to a logfile
 */


#ifndef _LOGFILE_H_
#define _LOGFILE_H_


#define OUTPUT_DEVICE	((unsigned int *) 0xFF000000)


void logfileOut(char c);


#endif /* _LOGFILE_H_ */
