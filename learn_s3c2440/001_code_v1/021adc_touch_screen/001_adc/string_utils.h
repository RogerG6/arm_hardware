#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include "uart.h"

long simple_strtol(const char *cp,char **endp,unsigned int base);
char *gets(char s[]);
int get_int(void);
unsigned int get_uint(void);
int strlen(unsigned char * start);
int strcmp(const char * st1, const char * st2);


#endif /* _STRING_UTILS_H */

