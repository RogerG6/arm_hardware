# ifndef _INIT_H
# define _INIT_H


# include "s3c2440_soc.h"
void sdram_init(void);
void relocate(void);
void clear_bss(void);
void timer0_init(void);
void key_init(void);
void led_init(void);
void delay(volatile int d);

# endif





