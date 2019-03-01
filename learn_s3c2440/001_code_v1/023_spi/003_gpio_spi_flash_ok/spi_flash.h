
#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H


void SPIFlashReadID(unsigned int * mid, unsigned int * pid);
void SPIFlashEraseForData(unsigned int addr);
void SPIFlashPageProgram(unsigned int addr, unsigned char * buf, int len);
void SPIFlashReadData(unsigned int addr, unsigned char * buf, int len);
void SPIFlashInit(void);



#endif
