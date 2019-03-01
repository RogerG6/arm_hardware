#include "s3c24xx.h"


/* 用GPIO Pin模拟spi */
/* 配置spi相关引脚, 同时2个CS pin 不能同时使能，会有冲突 */
static void gpio_spi_init(void)
{
	/* GPF1-OLED_CSn : output */
	GPFCON &= ~(3<<(1*2));
	GPFCON |= (1<<(1*2));
	GPFDAT |= (1<<1);

	/* GPG2-FLASH_CSn: output
	 * GPG4-OLED_DC  : output
	 * GPG5-SPIMISO  : input
	 * GPG6-SPIMOSI  : output
	 * GPG7-SPICLK   : output
	 */
	 GPGCON &= ~((3<<(2*2)) | (3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
	 GPGCON |= ((1<<(2*2)) | (1<<(4*2)) | (1<<(6*2)) | (1<<(7*2)));
	 GPGDAT |= (1<<2);
}

static void SPISetClk(char val)
{
	if (val)
		GPGDAT |= (1<<7);
	else
		GPGDAT &= ~(1<<7);
}

static void SPISendDo(unsigned char val)   /* 发送数据的每一BIT */
{
	if (val)
		GPGDAT |= (1<<6);
	else
		GPGDAT &= ~(1<<6);
}

static unsigned char SPIRecvDI(void)   /* 接收数据的每一BIT */
{
	if (GPGDAT & (1<<5))
		return 1;
	else
		return 0;
}


void SPISendByte(unsigned char val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		SPISetClk(0);
		SPISendDo(val & 0x80);
		SPISetClk(1);
		val <<= 1;
	}
}

void SPIRecvByte(unsigned char * val)
{
	int i;
	int tmp = 0;
	for (i = 0; i < 8; i++)
	{
		tmp <<= 1;
		SPISetClk(0);
		tmp |= SPIRecvDI();
		SPISetClk(1);
	}
	*val = tmp;
}


void SPIInit(void)
{
	gpio_spi_init();
}
