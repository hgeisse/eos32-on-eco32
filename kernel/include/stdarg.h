/*
 * stdarg.h -- variable argument lists
 */


#ifndef _STDARG_H_
#define _STDARG_H_


typedef char *va_list;


#define va_start(ap, last) \
    ((ap) = (sizeof(last) < 4 ? \
                (char *) (((int *) &(last)) + 1) : \
                (char *) (&(last) + 1)))

#define va_arg(ap, type) \
    (sizeof(type) == 1 ? \
         (* (type *) (&((ap) += 4)[-1])) : \
     sizeof(type) == 2 ? \
         (* (type *) (&((ap) += 4)[-2])) : \
     sizeof(type) == 4 ? \
         (* (type *) (&((ap) += 4)[-4])) : \
         (* (type *) (&((ap) += 8)[-8])))

#define va_end(ap) \
    ((void) 0)


#endif /* _STDARG_H_ */
