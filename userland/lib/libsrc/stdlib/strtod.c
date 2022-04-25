/*
 * Convert a string to a double
 * implemented by swlt16
 * 
 * NO DEPENDENCIES
 */

double strtod (const char* str, char** endptr) {
    double result1 = 0.0; /* left site of dot */
    double result2 = 0.0; /* right site of dot */
    double* cur;
    char sign = +1;
    int rsc = 0; /* right site counter */
    int quot = 1; /* quotient */

    cur = &result1;
    if( ! str) goto output;
    while(*str) {
        switch(*str) {
            case ' ':  case '\n': case '\t': 
            case '\v': case '\f': case '\r':
                if(result1 != 0.0 || result2 != 0.0) goto output; 
                /* just ignore whitespaces */
                break;
            case '+':
                if(result1 != 0.0 || result2 != 0.0) goto output;
                sign = +1;
                break;
            case '-':
                if(result1 != 0.0 || result2 != 0.0) goto output; 
                sign = -1;
                break;
            case '.':
                cur = &result2;
                rsc = 0;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                *cur *= 10;
                *cur += (*str) - '0';
                rsc++;
                break;
            default:
                goto output;
        }
        str++;
    }
    output:
    if(endptr) *endptr = (char*) str;
    /* move result2 to the right site of the dot (mathematically, if any) */
    while(rsc--) quot *= 10;
    if(result2 != 0.0) result2 /= quot;
    
    return sign * (result1 + result2);
}
