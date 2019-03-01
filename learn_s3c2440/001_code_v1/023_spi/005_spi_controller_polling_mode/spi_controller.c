#include "s3c24xx.h"


/* 用GPIO Pin模拟spi */
/* 配置spi相关引脚, 同时2个CS pin 不能同时使能，会有冲突 
 * 配置spi controller时，GPG567为SPI相关功能
 */
static void gpio_spi_init(void)
{
	/* GPF1-OLED_CSn : output */
	GPFCON &= ~(3<<(1*2));
	GPFCON |= (1<<(1*2));
	GPFDAT |= (1<<1);

	/* GPG2-FLASH_CSn: output
	 * GPG4-OLED_DC  : output
	 * GPG5-SPIMISO  
	 * GPG6-SPIMOSI  
	 * GPG7-SPICLK   
	 */
	 GPGCON &= ~((3<<(2*2)) | (3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
	 GPGCON |= ((1<<(2*2)) | (1<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
	 GPGDAT |= (1<<2);
}

void SPI_controller_init(void)
{
	/* spi flash : 104MHz
	 * spi oled  : 100ns 10MHz
	 * Baud rate = PCLK / 2 / (Prescaler value + 1)
	 *			 = 50M / 2 / (prescaler value + 1)
	 * 			 = 10M
	 * Prescaler value = 2
	 * 实际Baud rate   = 50/2/3 = 8.3MHz
	 */
	SPPRE0 = 2;
	SPPRE1 = 2;

	/* [6:5] : 00 = polling mode
	 * [4]   : 1 = enable
	 * [3]   : 1 = master
	 * [2]   : 0 = active high (Clock Polarity)
	 * [1]   : 0 = format A    (transfer format)
	 * [0]   : 0 = normal mode, 1 = Tx auto garbage data mode
	 *		   当发送数据时，是否从DI pin上返回数据
	 */
	SPCON0 = (1<<4) | (1<<3);
	SPCON1 = (1<<4) | (1<<3);
}


void SPISendByte(unsigned char val)
{
	/* when not ready, wait */
	while (!(SPSTA1 & 1));
	SPTDAT1 = val;
}

void SPIRecvByte(unsigned char * val)
{
	SPTDAT1 = 0xff;
	while (!(SPSTA1 & 1));
	*val = SPRDAT1;
}


void SPIInit(void)
{
	gpio_spi_init();
	SPI_controller_init();
}

