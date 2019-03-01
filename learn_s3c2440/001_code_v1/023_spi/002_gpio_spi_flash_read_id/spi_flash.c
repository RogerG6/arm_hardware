#include "s3c24xx.h"

static void SPIFlashCS(int val)
{
	if (val)
		GPGDAT |= (1<<2);
	else
		GPGDAT &= ~(1<<2);
}

static void SPIFlashSetAddr(int val)
{
	SPISendByte(val>>16);
	SPISendByte(val>>8);
	SPISendByte(val & 0xff);
}

void SPIFlashReadID(unsigned int * mid, unsigned int * pid)
{
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x90);
	SPIFlashSetAddr(0);

	SPIRecvByte(mid);
	SPIRecvByte(pid);
	
	SPIFlashCS(1);	/* deselect spi flash */
}




