
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H

void nand_flash_test(void);
void read_nand(unsigned int addr, unsigned char * dest, unsigned int len);
void nand_init(void);

#endif

