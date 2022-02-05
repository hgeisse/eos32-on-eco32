/*
 * time.c -- date and time functions
 */


#include <time.h>
#include <eos32.h>
#include <assert.h>


/*
 * These routines convert time as follows.
 *
 * The epoch is 0000 Jan 1 1970 GMT.
 * The argument time is in seconds since then.
 * The localtime(t) entry returns a pointer to an array
 * containing
 *  seconds (0-59)
 *  minutes (0-59)
 *  hours (0-23)
 *  day of month (1-31)
 *  month (0-11)
 *  year-1970
 *  weekday (0-6, Sun is 0)
 *  day of the year
 *  daylight savings flag
 *
 * The routine calls the system to determine the local
 * timezone and whether Daylight Saving Time is permitted locally.
 * (DST is then determined by the current US standard rules)
 * There is a table that accounts for the peculiarities
 * undergone by daylight time in 1974-1975.
 *
 * The routine does not work
 * in Saudi Arabia which runs on Solar time.
 *
 * asctime(tvec))
 * where tvec is produced by localtime
 * returns a ptr to a character string
 * that has the ascii time in the form
 *	Thu Jan 01 00:00:00 1970n0\\
 *	01234567890123456789012345
 *	0	  1	    2
 *
 * ctime(t) just calls localtime, then asctime.
 */


/**************************************************************/


/*
 * The following table lists the number of days in a month.
 */
static int dmsize[12] = {
  31, 28, 31,
  30, 31, 30,
  31, 31, 30,
  31, 30, 31
};


/*
 * The following table is used for 1974 and 1975 and
 * gives the day number of the first day after the
 * Sunday of the change.
 */
static struct {
  int  daylb;
  int  dayle;
} daytab[] = {
  5,  333,	/* 1974: Jan 6 - last Sun. in Nov */
  58, 303,	/* 1975: Last Sun. in Feb - last Sun in Oct */
};


/*
 * Return the number of days in a given year.
 */
static int dysize(int y) {
  if (y % 4 == 0) {
    return 366;
  }
  return 365;
}


/*
 * The argument is a 0-origin day number.
 * The value is the day number of the first
 * Sunday on or after the day.
 */
static sunday(struct tm *t, int d) {
  if (d >= 58) {
    d += dysize(t->tm_year) - 365;
  }
  return d - (d - t->tm_yday + t->tm_wday + 700) % 7;
}


/*
 * Print a number with at most two digits to a buffer.
 */
static char *twoDigits(char *cp, int n) {
  cp++;
  if (n >= 10) {
    *cp++ = (n / 10) % 10 + '0';
  } else {
    *cp++ = ' ';
  }
  *cp++ = n % 10 + '0';
  return cp;
}


/**************************************************************/


clock_t clock(void) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


time_t time(time_t *tp) {
  return _time(tp);
}


time_t mktime(struct tm *tp) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}


char *asctime(const struct tm *tp) {
  static char cbuf[26];
  char *cp;
  char *ncp;
  const int *ip;

  cp = cbuf;
  for (ncp = "Day Mon 00 00:00:00 1900\n"; *cp++ = *ncp++;) ;
  ncp = &"SunMonTueWedThuFriSat"[3 * tp->tm_wday];
  cp = cbuf;
  *cp++ = *ncp++;
  *cp++ = *ncp++;
  *cp++ = *ncp++;
  cp++;
  ip = &tp->tm_mon;
  ncp = &"JanFebMarAprMayJunJulAugSepOctNovDec"[(*ip)*3];
  *cp++ = *ncp++;
  *cp++ = *ncp++;
  *cp++ = *ncp++;
  cp = twoDigits(cp, *--ip);
  cp = twoDigits(cp, *--ip+100);
  cp = twoDigits(cp, *--ip+100);
  cp = twoDigits(cp, *--ip+100);
  if (tp->tm_year >= 100) {
    cp[1] = '2';
    cp[2] = '0';
  }
  cp += 2;
  cp = twoDigits(cp, tp->tm_year + 100);
  return cbuf;
}


char *ctime(const time_t *tp) {
  return asctime(localtime(tp));
}


struct tm *gmtime(const time_t *tp) {
  static struct tm xtime;
  int d0, d1;
  long hms, day;
  int *ip;

  /*
   * break initial number into days
   */

  hms = *tp % 86400;
  day = *tp / 86400;
  if (hms < 0) {
    hms += 86400;
    day -= 1;
  }
  ip = (int *) &xtime;

  /*
   * generate hours:minutes:seconds
   */

  *ip++ = hms % 60;
  d1 = hms / 60;
  *ip++ = d1 % 60;
  d1 /= 60;
  *ip++ = d1;

  /*
   * day is the day number.
   * generate day of the week.
   * The addend is 4 mod 7 (1/1/1970 was Thursday)
   */

  xtime.tm_wday = (day + 7340036) % 7;

  /*
   * year number
   */

  if (day >= 0) {
    for (d1 = 70; day >= dysize(d1); d1++) {
      day -= dysize(d1);
    }
  } else {
    for (d1 = 70; day < 0; d1--) {
      day += dysize(d1 - 1);
    }
  }
  xtime.tm_year = d1;
  xtime.tm_yday = d0 = day;

  /*
   * generate month
   */

  if (dysize(d1) == 366) {
    dmsize[1] = 29;
  }
  for (d1 = 0; d0 >= dmsize[d1]; d1++) {
    d0 -= dmsize[d1];
  }
  dmsize[1] = 28;
  *ip++ = d0 + 1;
  *ip++ = d1;
  xtime.tm_isdst = 0;

  /*
   * return the result
   */

  return &xtime;
}


struct tm *localtime(const time_t *tp) {
  int dayno;
  struct tm *ct;
  int daylbegin, daylend;
  long copyt;
  struct timeb systime;

  ftime(&systime);
  copyt = *tp - (long) systime.timezone * 60;
  ct = gmtime(&copyt);
  dayno = ct->tm_yday;
  daylbegin = 119;	/* last Sun in Apr */
  daylend = 303;	/* Last Sun in Oct */
  if (ct->tm_year == 74 || ct->tm_year == 75) {
    daylbegin = daytab[ct->tm_year - 74].daylb;
    daylend = daytab[ct->tm_year - 74].dayle;
  }
  daylbegin = sunday(ct, daylbegin);
  daylend = sunday(ct, daylend);
  if (systime.dstflag &&
      (dayno > daylbegin || (dayno == daylbegin && ct->tm_hour >= 2)) &&
      (dayno < daylend || (dayno == daylend && ct->tm_hour < 1))) {
    copyt += 1 * 60 * 60;
    ct = gmtime(&copyt);
    ct->tm_isdst++;
  }
  return ct;
}


size_t strftime(char *s, size_t smax,
                const char *fmt,
                const struct tm *tp) {
  /* !!!!! */
  assert(1 == 0);
  return 0;
}
