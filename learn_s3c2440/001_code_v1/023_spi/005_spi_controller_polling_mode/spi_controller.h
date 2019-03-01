#ifndef  _S3C2440_SPICONTROLLER_H
#define  _S3C2440_SPICONTROLLER_H

void SPISendByte(unsigned char val);
void SPIInit(void);
void SPIRecvByte(unsigned char * val);


#endif


