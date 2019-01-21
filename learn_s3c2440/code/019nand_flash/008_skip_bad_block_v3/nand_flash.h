
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H
# include "s3c2440_soc.h"
# include "my_printf.h"
# include "string_utils.h"
# include "uart.h"
void nand_flash_test(void);
void read_nand(unsigned int addr, unsigned char * dest, unsigned int len);
void nand_init(void);

#endif

