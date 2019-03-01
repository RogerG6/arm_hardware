#include "s3c24xx.h"
#include "spi_controller.h"



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

void SPIFlashReadID(unsigned char * mid, unsigned char * pid)
{
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x90);
	SPIFlashSetAddr(0);

	SPIRecvByte(mid);
	SPIRecvByte(pid);
	
	SPIFlashCS(1);	/* deselect spi flash */
}

/* 0: disable, 1: enable */
static void SPIFlashWriteEnable(int enable)
{
	if (enable)
	{
		SPIFlashCS(0);	/* select spi flash */
		SPISendByte(0x06);
		SPIFlashCS(1);	/* deselect spi flash */
	}
	else
	{	
		SPIFlashCS(0);	/* select spi flash */
		SPISendByte(0x04);
		SPIFlashCS(1);	/* deselect spi flash */
	}
}

static unsigned char SPIFlashReadStatusReg1(void)
{
	unsigned char reg1;
	
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x05);
	SPIRecvByte(&reg1);
	SPIFlashCS(1);	/* deselect spi flash */

	return reg1;
}

static unsigned char SPIFlashReadStatusReg2(void)
{
	unsigned char reg2;
	
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x35);
	SPIRecvByte(&reg2);
	SPIFlashCS(1);	/* deselect spi flash */

	return reg2;
}

static void SPIFlashWaitWhenBusy(void)
{
	int reg1 = SPIFlashReadStatusReg1();

	while (reg1 & 1)
		reg1 = SPIFlashReadStatusReg1();
}

static void SPIFlashWriteStatusReg(unsigned char reg1, unsigned char reg2)
{
	SPIFlashWriteEnable(1);

	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x01);
	SPISendByte(reg1);
	SPISendByte(reg2);
	SPIFlashCS(1);	/* deselect spi flash */

	SPIFlashWaitWhenBusy();
}


static void SPIFlashClearProtectForReg(void)
{
	unsigned char reg1, reg2;
	
    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

	reg1 &= ~(1<<7);
	reg2 &= ~(1<<0);

	SPIFlashWriteStatusReg(reg1, reg2);
}

static void SPIFlashClearProtectForData(void)
{
	unsigned char reg1, reg2;
	
    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

	/* CMP = 0, BP2,1,0 = 0b000 */
	reg1 &= ~(7<<2);
	reg2 &= ~(1<<6);

	SPIFlashWriteStatusReg(reg1, reg2);
}

/* sector erase 4K */
void SPIFlashEraseForData(unsigned int addr)
{
	SPIFlashWriteEnable(1);	/* 写使能 */

	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x20);
    SPIFlashSetAddr(addr);
	SPIFlashCS(1);	/* deselect spi flash */

	SPIFlashWaitWhenBusy();
}

/* Page Program */
void SPIFlashPageProgram(unsigned int addr, unsigned char * buf, int len)
{
	int i;
	
	SPIFlashWriteEnable(1);	/* 写使能 */
	
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x02);
	SPIFlashSetAddr(addr);

	for (i = 0; i < len; i++)
		SPISendByte(buf[i]);
		
	SPIFlashCS(1);	/* deselect spi flash */

	SPIFlashWaitWhenBusy();
}

void SPIFlashReadData(unsigned int addr, unsigned char * buf, int len)
{
	int i;
	
	SPIFlashCS(0);	/* select spi flash */
	SPISendByte(0x03);
	SPIFlashSetAddr(addr);

	for (i = 0; i < len; i++)
	{
		SPIRecvByte(&buf[i]);
	}
	
	SPIFlashCS(1);	/* deselect spi flash */	
}

void SPIFlashInit(void)
{
	SPIFlashClearProtectForReg();
	SPIFlashClearProtectForData();

}

