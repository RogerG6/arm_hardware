# ifndef _INIT_H
# define _INIT_H


# include "s3c2440_soc.h"
void sdram_init(void);
void relocate(volatile unsigned int * src, volatile unsigned int * dest, volatile unsigned int len);
void clear_bss(volatile unsigned int * src, volatile unsigned int * dest);

# endif





