/*
 * time.h -- date and time functions
 */


#ifndef _TIME_H_
#define _TIME_H_


#include <sys/tim.h>
#include <stddef.h>


#ifndef NULL
#define NULL	((void *) 0)
#endif


#define CLOCKS_PER_SEC	50


typedef long clock_t;


struct tm {
  int tm_sec;		/* seconds after the minute (0, 61) */
  int tm_min;		/* minutes after the hour (0, 59) */
  int tm_hour;		/* hours since midnight (0, 23) */
  int tm_mday;		/* day of the month (1, 31) */
  int tm_mon;		/* months since January (0, 11) */
  int tm_year;		/* years since 1900 */
  int tm_wday;		/* days since sunday (0, 6) */
  int tm_yday;		/* days since January 1 (0, 365) */
  int tm_isdst;		/* Daylight Saving Time flag */
};


clock_t clock(void);
time_t time(time_t *tp);
time_t mktime(struct tm *tp);
char *asctime(const struct tm *tp);
char *ctime(const time_t *tp);
struct tm *gmtime(const time_t *tp);
struct tm *localtime(const time_t *tp);
size_t strftime(char *s, size_t smax,
                const char *fmt,
                const struct tm *tp);


#endif /* _TIME_H_ */
