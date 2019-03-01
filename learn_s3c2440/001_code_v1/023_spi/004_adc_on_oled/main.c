#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "spi.h"
#include "oled.h"
#include "spi_flash.h"


unsigned char at24cxx_read(unsigned char address);
void at24cxx_write(unsigned char address, unsigned char data);


void delay(int val)
{
	while (val--);
}

int main()
{
    char c;
    char str[200];
    int i;
	int address;
	int data;
	unsigned int mid, pid;
    
    uart0_init();   // ²¨ÌØÂÊ115200£¬8N1(8¸öÊı¾İÎ»£¬ÎŞĞ£ÑéÎ»£¬1¸öÍ£Ö¹Î»)
    

    SPIInit();
    OLEDInit();
    OLEDPrint(0, 0, "www.baidu.com");

    SPIFlashReadID(&mid, &pid);
    sprintf(str, "MID:%2x, PID:%2x", mid, pid);
    OLEDPrint(2, 0, str);

	/* è¯»å†™spi flash æ“ä½œ */
	SPIFlashInit();
	SPIFlashEraseForData(0);
	SPIFlashPageProgram(0, "SPI Flash is ok!", 17);
    SPIFlashReadData(0, str, 17);
    OLEDPrint(4, 0, str);

	Test_Adc();	/* å°†ADCçš„æ•°æ®æ‰“å°åœ¨oledä¸Š */

    while (1);
    return 0;
}
