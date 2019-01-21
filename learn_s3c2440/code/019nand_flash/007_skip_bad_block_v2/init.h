# ifndef _INIT_H
# define _INIT_H


# include "s3c2440_soc.h"
# include "nand_flash.h"
void sdram_init(void);
void relocate(void);
void clear_bss(void);

# endif





