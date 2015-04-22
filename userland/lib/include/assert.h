/*
 * assert.h -- diagnostics
 */


/*
 * Note:
 * There is no guard against multiple inclusion, on purpose!
 */


#undef assert


#ifdef NDEBUG

#define assert(exp)	((void) 0)

#else /* not NDEBUG */

#define assert(exp)	((exp) ? \
			 (void) 0 : \
			 _assertionFailed(#exp, __FILE__, __LINE__))

void _assertionFailed(char *exp, char *file, int line);

#endif /* NDEBUG */
